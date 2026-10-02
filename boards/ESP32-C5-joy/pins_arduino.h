#ifndef Pins_Arduino_h
#define Pins_Arduino_h
#include "soc/soc_caps.h"
#include <stdint.h>

// ============================================================================
//  Waveshare ESP32-C5-WiFi6-KIT (16MB flash, NO PSRAM)
//  + Waveshare 2" ST7789V 240x320 SPI display
//  + KEYES analog joystick (X/Y + button)
//  Pin map matches the hand-soldered wiring (see README / connections.md).
//  Strapping pins avoided: IO2 IO7 IO27 IO28 | USB D-/D+: IO13 IO14
//  Flash/PSRAM reserved:   IO15..IO22        | ADC1 only on: IO1..IO6
// ============================================================================

#define PIN_RGB_LED 27
static const uint8_t LED_BUILTIN = SOC_GPIO_PIN_COUNT + PIN_RGB_LED;
#define BUILTIN_LED LED_BUILTIN
#define LED_BUILTIN LED_BUILTIN
#define RGB_BUILTIN LED_BUILTIN
#define RGB_BRIGHTNESS 64

static const uint8_t TX = 11;
static const uint8_t RX = 12;
static const uint8_t USB_DM = 13;
static const uint8_t USB_DP = 14;
static const uint8_t SDA = 4;
static const uint8_t SCL = 5;
static const uint8_t SS = 10;
static const uint8_t MOSI = 23;
static const uint8_t MISO = -1;
static const uint8_t SCK = 24;
static const uint8_t A0 = 1;
static const uint8_t A1 = 2;
static const uint8_t A2 = 3;
static const uint8_t A3 = 4;
static const uint8_t A4 = 5;
static const uint8_t A5 = 6;

#define HAS_RGB_LED 1
#define LED_ORDER GRB
#define LED_TYPE_IS_RGBW 1
#define LED_COUNT 1
#define LED_TYPE WS2812
#define LED_COLOR_STEP 15
#define RGB_LED 27

#define SERIAL_TX 11
#define SERIAL_RX 12
#define GROVE_SDA 4
#define GROVE_SCL 5
#define SPI_SCK_PIN 24
#define SPI_MOSI_PIN 23
#define SPI_MISO_PIN -1
#define SPI_SS_PIN -1

/* ================= TFT: ST7789V 240x320, viewed landscape ================= */
#define HAS_SCREEN 1
#define ROTATION 1
#define MINBRIGHT (uint8_t)1
#define USER_SETUP_LOADED 1
#define ST7789_DRIVER 1
#define TFT_WIDTH 240
#define TFT_HEIGHT 320
#define TFT_RGB_ORDER TFT_BGR
#define TFT_BACKLIGHT_ON 1
#define TFT_MOSI 23
#define TFT_SCLK 24
#define TFT_CS   10
#define TFT_DC    9
#define TFT_RST   8
#define TFT_BL    0
#define TFT_MISO -1
#define SMOOTH_FONT 1
#define SPI_FREQUENCY 20000000
#define SPI_READ_FREQUENCY 20000000

/* ================= KEYES analog joystick ================= */
#define HAS_ANALOG_JOYSTICK 1
#define HAS_5_BUTTONS 1          // enables the on-screen keyboard input path
#define JOY_X   1                // ADC1_CH0
#define JOY_Y   4                // ADC1_CH3
#define JOY_BTN 5                // push button, active HIGH on this module
#define JOY_PWR 6                // module VCC fed from this GPIO (driven HIGH)
#define JOY_ADC_MAX 4095
#define JOY_DEADZONE 700
#define JOY_FAULT_HI 2800        // both axes above this = GND fault -> ignore
#define BTN_ACT HIGH
#define DEEPSLEEP_WAKEUP_PIN JOY_BTN
// Uncomment if an axis feels reversed:
// #define JOY_INVERT_X 1
// #define JOY_INVERT_Y 1

/* ================= Unused peripherals -> disabled ================= */
#define BAD_RX 4
#define BAD_TX 5
#define GPS_SERIAL_TX 5
#define GPS_SERIAL_RX 4
#define RXLED -1
#define TXLED -1
#define LED_ON HIGH
#define LED_OFF LOW
#define SDCARD_CS -1
#define SDCARD_SCK -1
#define SDCARD_MISO -1
#define SDCARD_MOSI -1
#define CC1101_GDO0_PIN -1
#define CC1101_SS_PIN -1
#define CC1101_MOSI_PIN -1
#define CC1101_SCK_PIN -1
#define CC1101_MISO_PIN -1
#define NRF24_CE_PIN -1
#define NRF24_SS_PIN -1
#define NRF24_MOSI_PIN -1
#define NRF24_SCK_PIN -1
#define NRF24_MISO_PIN -1
#define W5500_INT_PIN -1
#define W5500_SS_PIN -1
#define W5500_MOSI_PIN -1
#define W5500_SCK_PIN -1
#define W5500_MISO_PIN -1
#endif /* Pins_Arduino_h */
