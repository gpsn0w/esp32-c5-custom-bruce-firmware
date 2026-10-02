#include "core/powerSave.h"
#include "core/utils.h"
#include <interface.h>
#include "esp_rom_gpio.h"
#include "soc/gpio_sig_map.h"
#include "esp_attr.h"
#include "nvs_flash.h"
#include <globals.h>
#include <LittleFS.h>

// ---- crash-loop guard (survives soft resets via RTC memory) ----
RTC_NOINIT_ATTR uint32_t _bruceBootMagic;
RTC_NOINIT_ATTR uint32_t _bruceCrashCount;

/***************************************************************************************
** _setup_gpio() : initial board setup + crash-loop auto-recovery
***************************************************************************************/
void _setup_gpio() {
    // If the device has reset-looped several times before ever reaching the UI,
    // a bad saved config is almost certainly the cause -> wipe it and self-heal.
    if (_bruceBootMagic != 0xB0CE1234u) { _bruceBootMagic = 0xB0CE1234u; _bruceCrashCount = 0; }
    _bruceCrashCount++;
    if (_bruceCrashCount >= 3) {
        _bruceCrashCount = 0;
        nvs_flash_erase();
        nvs_flash_init();
        if (LittleFS.begin(true)) LittleFS.format();
    }

    pinMode(TFT_CS, OUTPUT);   digitalWrite(TFT_CS, HIGH);
    pinMode(TFT_MOSI, OUTPUT); digitalWrite(TFT_MOSI, HIGH);
    pinMode(TFT_SCLK, OUTPUT);
    pinMode(TFT_BL, OUTPUT);   digitalWrite(TFT_BL, HIGH);
    pinMode(TFT_RST, OUTPUT);  digitalWrite(TFT_RST, HIGH);
    pinMode(TFT_DC, OUTPUT);   digitalWrite(TFT_DC, HIGH);
#ifdef HAS_ANALOG_JOYSTICK
    pinMode(JOY_PWR, OUTPUT);  digitalWrite(JOY_PWR, HIGH); // power the joystick
    pinMode(JOY_BTN, INPUT_PULLUP);
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);
#endif
}

/***************************************************************************************
** _post_setup_gpio()
***************************************************************************************/
void _post_setup_gpio() {}

/***************************************************************************************
** _pre_storage_gpio() : runs right after tft.init()/tft.begin()
**  On ESP32-C5 TFT_eSPI writes pixels via direct SPI registers, which only reach the
**  panel on the FSPI IOMUX pins. This display is wired to GPIO-matrix pins, so we force
**  the FSPI CLK/DATA signals onto them, hardware-reset the panel (so soft RST also
**  recovers it), and re-send the ST7789 init so the panel actually receives it.
***************************************************************************************/
void _pre_storage_gpio() {
    pinMode(TFT_SCLK, OUTPUT);
    pinMode(TFT_MOSI, OUTPUT);
    esp_rom_gpio_connect_out_signal(TFT_SCLK, FSPICLK_OUT_IDX, false, false);
    esp_rom_gpio_connect_out_signal(TFT_MOSI, FSPID_OUT_IDX, false, false);
    // hardware reset pulse (so a soft RST re-inits the panel, not only a power-cycle)
    pinMode(TFT_RST, OUTPUT);
    digitalWrite(TFT_RST, HIGH); delay(10);
    digitalWrite(TFT_RST, LOW);  delay(20);
    digitalWrite(TFT_RST, HIGH); delay(150);
    tft.writecommand(0x01); delay(150);          // SWRESET
    tft.writecommand(0x11); delay(120);          // SLPOUT
    tft.writecommand(0x3A); tft.writedata(0x55); // COLMOD 16-bit
    tft.writecommand(0x21);                      // INVON (IPS panel)
    tft.writecommand(0x13);                      // NORON
    tft.writecommand(0x29); delay(20);           // DISPON
    tft.setRotation(ROTATION);
    tft.fillScreen(TFT_BLACK);
}

int getBattery() { return 0; } // no battery-sense divider wired
bool isCharging() { return false; }

void _setBrightness(uint8_t brightval) {
    if (brightval == 0) analogWrite(TFT_BL, brightval);
    else { int bl = MINBRIGHT + round(((255 - MINBRIGHT) * brightval / 100)); analogWrite(TFT_BL, bl); }
}

/*********************************************************************
** InputHandler : dual-mode analog KEYES joystick
**   Menus:    Up/Down scroll, Left = Back, Right = Select, button = Select
**   Keyboard: 4 directions move the cursor, button types, long-press = done
**   - center auto-calibrated at boot (skips GND-fault samples)
**   - GND fault (both axes railed high) is ignored, so a loose GND never
**     makes the menu run away
**   - edge-only (one push = one step) so axis drift cannot auto-scroll
**********************************************************************/
void InputHandler(void) {
#ifdef HAS_ANALOG_JOYSTICK
    static bool firstRun = true;
    if (firstRun) { _bruceCrashCount = 0; firstRun = false; } // reached running UI -> stable

    static bool calibrated = false;
    static int cx = 1600, cy = 1600;
    static int8_t lastDirX = 0, lastDirY = 0;
    static unsigned long btnDownAt = 0;
    static bool btnLongFired = false;

    int xVal = analogRead(JOY_X);
    int yVal = analogRead(JOY_Y);

    if (!calibrated) {
        long sx = 0, sy = 0; int n = 0;
        for (int i = 0; i < 96 && n < 32; i++) {
            int rx = analogRead(JOY_X), ry = analogRead(JOY_Y);
            if (!(rx > JOY_FAULT_HI && ry > JOY_FAULT_HI)) { sx += rx; sy += ry; n++; }
            delay(2);
        }
        if (n > 0) { cx = sx / n; cy = sy / n; }
        calibrated = true;
    }

    unsigned long now = millis();
    const unsigned long LONG_MS = 450;
    const unsigned long DEBOUNCE = 40;

    // center button: short = Select, long = Esc/Back
    bool btnNow = (digitalRead(JOY_BTN) == BTN_ACT);
    bool selBtn = false, escBtn = false;
    if (btnNow) {
        if (btnDownAt == 0) { btnDownAt = now; btnLongFired = false; }
        else if (!btnLongFired && now - btnDownAt >= LONG_MS) { escBtn = true; btnLongFired = true; }
    } else {
        if (btnDownAt != 0 && !btnLongFired && now - btnDownAt >= DEBOUNCE) selBtn = true;
        btnDownAt = 0; btnLongFired = false;
    }

    // GND fault: both axes railed high together -> ignore stick, keep button
    bool fault = (xVal > JOY_FAULT_HI && yVal > JOY_FAULT_HI);
    if (fault) {
        AnyKeyPress = selBtn || escBtn;
        PrevPress = NextPress = UpPress = DownPress = NextPagePress = false;
        SelPress = selBtn; EscPress = escBtn;
        return;
    }

    const int DZ = JOY_DEADZONE;
    const int RELEASE = JOY_DEADZONE / 2;
    int dx = xVal - cx;
    int dy = yVal - cy;
#ifdef JOY_INVERT_X
    dx = -dx;
#endif
#ifdef JOY_INVERT_Y
    dy = -dy;
#endif

    // vertical (edge only: one push = one step -> drift-proof)
    bool upR = false, downR = false;
    int8_t dirY = (dy < -DZ) ? -1 : (dy > DZ) ? 1 : 0;
    if (dirY == 0 && abs(dy) < RELEASE) lastDirY = 0;
    if (dirY != 0 && dirY != lastDirY) { if (dirY < 0) upR = true; else downR = true; lastDirY = dirY; }
    else if (dirY != 0) lastDirY = dirY;

    // horizontal (edge only)
    bool leftE = false, rightE = false, leftR = false, rightR = false;
    int8_t dirX = (dx < -DZ) ? -1 : (dx > DZ) ? 1 : 0;
    if (dirX == 0 && abs(dx) < RELEASE) lastDirX = 0;
    if (dirX != 0 && dirX != lastDirX) {
        if (dirX < 0) { leftE = true; leftR = true; } else { rightE = true; rightR = true; }
        lastDirX = dirX;
    } else if (dirX != 0) lastDirX = dirX;

    bool any = upR || downR || leftR || rightR || btnNow;
    if (any && wakeUpScreen()) return;
    AnyKeyPress = any;

    if (joyKbdMode) {
        // on-screen keyboard: clean 4-way, button types, long-press exits
        UpPress = upR; DownPress = downR;
        PrevPress = leftR; NextPress = rightR;
        SelPress = selBtn; EscPress = escBtn;
        NextPagePress = false;
    } else {
        // menus: Up/Down scroll, Left = Back, Right = Select
        PrevPress = upR; NextPress = downR;
        UpPress = false; DownPress = false;
        EscPress = leftE || escBtn;
        SelPress = selBtn || rightE;
        NextPagePress = false;
    }
#endif
}

void powerOff() {}
void checkReboot() {}
