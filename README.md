# ESP32-C5 Custom Bruce Firmware

🌍 **English** · [Български](README.bg.md)

[Bruce](https://github.com/pr3y/Bruce) firmware ported to a **hand-wired
Waveshare ESP32-C5** board with an external **ST7789V** display and a
**KEYES analog joystick**. This repo holds the custom board definition, the
required core patches, build/flash instructions, and a ready-to-flash binary.

> ESP32-C5 is a brand-new RISC-V, dual-band Wi-Fi 6 chip. Getting Bruce running
> on a hand-wired C5 took several non-obvious fixes (display, input, stability) —
> all documented below so anyone with the same parts can reproduce it.

📖 **New here? Start with the [step-by-step Tutorial](TUTORIAL.md)** · [Урок на български](TUTORIAL.bg.md)

## ⚠️ DISCLAIMER — READ THIS FIRST

**This project is STRICTLY for EDUCATION and AUTHORIZED security testing. NOTHING ELSE.**

By downloading, building, flashing or using this firmware you AGREE that:

- You will use it **ONLY** on hardware, networks and systems that are **YOURS**, or that you have **EXPLICIT WRITTEN PERMISSION** to test.
- Unauthorized access to, jamming of, or interference with ANY device, network or system you do not own is **ILLEGAL** and can be a **serious crime** in virtually every country.
- **ALL RESPONSIBILITY IS YOURS.** The author and contributors are **NOT LIABLE** for ANY damage, data loss, legal trouble, fines, injuries, or anything else — direct or indirect — caused by the use or misuse of this project.
- This is provided **"AS IS", with ABSOLUTELY NO WARRANTY** of any kind.
- If you break the law with this, that is **100% ON YOU.** You were warned, in writing, right here.

**Don't be a criminal. Keep it legal. Stay on your own gear.** 🙂

## Hardware

| Part | Model |
|------|-------|
| MCU board | **Waveshare ESP32-C5-WiFi6-KIT** (16 MB flash, **no PSRAM**) |
| Display | **Waveshare 2" LCD Module — ST7789V, 240×320, SPI** |
| Input | **KEYES analog joystick** (X/Y + push button) |
| Power | 3.8 V LiPo + rocker switch on the battery + wire (optional) |

Full wiring and pin map: **[docs/connections.md](docs/connections.md)**

Quick pin map:
- **Display:** MOSI=IO23, SCLK=IO24, CS=IO10, DC=IO9, RST=IO8, BL=IO0
- **Joystick:** X=IO1, Y=IO4, BTN=IO5, VCC=IO6 (GPIO-powered), GND=GND

## Controls

| Action | Joystick |
|--------|----------|
| Navigate menu | Up / Down |
| **Select / OK** | **Right**, or center button |
| **Back / Esc** | **Left**, or long-press center |
| On-screen keyboard | 4 directions move the cursor, short press types, long press = done |

## Flash the prebuilt firmware

A single merged image is in [`firmware/`](firmware/). On macOS/Linux:

```bash
# find the port (USB-C "USR" port)
ls /dev/cu.usbmodem*        # macOS      (or:  ls /dev/ttyACM*  on Linux)

# flash (replace PORT)
pip install esptool
esptool --chip esp32c5 -p PORT write-flash 0x0 firmware/Bruce-esp32-c5-joy.bin
```

If it won't connect, hold **BOOT**, tap **RST**, release **BOOT**, then flash.
If it ever boot-loops after a bad config, `erase-flash` first — but the firmware
now **self-heals** (see below).

## Build from source

```bash
./setup.sh          # clones Bruce + drops in the board files + patches
cd bruce
pio run -e esp32-c5-joy
# output: Bruce-esp32-c5-joy.bin  (flash to 0x0)
```

Needs [PlatformIO](https://platformio.org/). The toolchain (pioarduino
platform + bmorcelli arduino-libs with ESP32-C5 support) is pulled automatically.

> **Note:** `setup.sh` pins Bruce to commit `ba519c93`. Newer upstream HEAD pulls a
> FastLED version (3.10.5) whose fixed-point/FFT code fails to compile for the
> ESP32-C5 toolchain; `setup.sh` also pins FastLED to 3.10.3.

## What makes this board work (the fixes)

1. **No PSRAM** — the C5 module has none; `BOARD_HAS_PSRAM` is *not* defined and
   the 8 MB partition table is used, otherwise it boot-loops.
2. **Display force-route** — TFT_eSPI drives pixels via direct SPI registers that
   only reach the panel on the FSPI IOMUX pins. The display is wired to GPIO-matrix
   pins, so `_pre_storage_gpio()` force-attaches `FSPICLK_OUT`/`FSPID_OUT` to
   IO24/IO23, then re-sends the ST7789 init. (See `boards/ESP32-C5-joy/interface.cpp`.)
3. **Soft-RST fix** — a hardware reset pulse + SWRESET on every boot so pressing
   RST re-inits the panel (no power-cycle needed).
4. **Joystick robustness** — center auto-calibration, GND-fault rejection (a loose
   GND no longer makes the menu run away), and edge-only navigation (one push = one
   step, drift-proof). Button is active-HIGH; short = Select, long = Back.
5. **On-screen keyboard** — board defines `HAS_5_BUTTONS` (otherwise the keyboard
   screen has no input path and hangs), with a `joyKbdMode` flag so menus keep the
   familiar Left=Back/Right=Select scheme while the keyboard gets clean 4-way input.
6. **Crash-loop auto-recovery** — if the device resets 3× before reaching the UI
   (e.g. a bad saved config from Evil Portal), it auto-wipes NVS + LittleFS and
   boots clean, instead of needing a PC to recover.

## Notes & limitations (ESP32-C5 specifics)

- **No USB BadUSB** — the C5 has no USB-OTG. Use **Bad BLE** (Bluetooth), or add a
  **CH9329** module for cable USB HID.
- **Sub-GHz / 2.4 GHz jam / NFC** need external modules (CC1101 / NRF24 / PN532) on
  the free pins (IO2, IO3, IO7, IO25, IO26, IO28).
- WiFi and Bluetooth/BLE features use the C5's built-in radio — no module needed.

## Credits & license

- Firmware: [Bruce](https://github.com/pr3y/Bruce) by the Bruce community.
- This port (board definition, patches, docs) follows Bruce's license.
- Built with the [pioarduino](https://github.com/pioarduino/platform-espressif32)
  platform and bmorcelli's ESP32-C5 arduino-libs.
