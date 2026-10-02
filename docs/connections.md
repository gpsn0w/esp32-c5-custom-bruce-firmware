# Wiring / Pin map

All parts are wired to the ESP32-C5 with jumper wires. Numbers are GPIO (IOxx).

## Display — Waveshare 2" LCD Module (ST7789V, 240×320, SPI)

| Display pin | ESP32-C5 |
|-------------|:--------:|
| VCC         | 3V3      |
| GND         | GND      |
| DIN (MOSI)  | IO23     |
| CLK (SCLK)  | IO24     |
| CS          | IO10     |
| DC          | IO9      |
| RST         | IO8      |
| BL          | IO0      |

The panel is 240×320 native; the firmware runs it landscape (320×240) via `ROTATION=1`.

## Joystick — KEYES analog (2 axes + push button)

| Joystick pin | ESP32-C5 | Notes |
|--------------|:--------:|-------|
| X            | IO1      | ADC1_CH0 |
| Y            | IO4      | ADC1_CH3 |
| B (button)   | IO5      | active HIGH on this module |
| VCC          | IO6      | module is powered from this GPIO (driven HIGH) |
| GND          | GND      | **must be a solid joint** — a loose GND makes both axes read high |

## Controls

**Menus:** Up/Down scroll · **Left = Back** · **Right = Select** · center button = Select
**On-screen keyboard:** 4 directions move the cursor · short press = type · long press (~0.5 s) = done/exit

## Free GPIOs for add-on modules (SPI/I2C/UART)
IO2, IO3, IO7, IO25, IO26, IO28 — available for CC1101 (sub-GHz), PN532 (NFC, I2C),
NRF24 (2.4 GHz), etc. Do not reuse the display/joystick pins above.
