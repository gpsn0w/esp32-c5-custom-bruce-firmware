# Tutorial — Build your ESP32-C5 Bruce device

🌍 **English** · [Български](TUTORIAL.bg.md)

A complete walkthrough: from loose parts to a working Bruce multitool on a
hand-wired Waveshare ESP32-C5. No prior firmware experience needed.

---

## 1. What you'll build

A pocket pentest/hacker multitool running [Bruce](https://github.com/pr3y/Bruce):
WiFi attacks (deauth, evil portal, beacon spam), Bluetooth/BLE (BLE spam, Bad BLE
= wireless BadUSB), a web UI, scripts, and more — driven by a joystick on a 2" screen.

## 2. Parts you need

| Part | Model |
|------|-------|
| MCU board | Waveshare **ESP32-C5-WiFi6-KIT** (16 MB, no PSRAM) |
| Display | Waveshare **2" LCD Module — ST7789V, 240×320, SPI** |
| Input | **KEYES analog joystick** (X/Y + button) |
| Wires | female-female Dupont jumper wires |
| (optional) | 3.8 V LiPo + a ≥1 A rocker switch on the battery + wire |

## 3. Wire it up

Connect with jumper wires (GPIO numbers):

**Display (ST7789V):**
`VCC→3V3, GND→GND, DIN→IO23, CLK→IO24, CS→IO10, DC→IO9, RST→IO8, BL→IO0`

**Joystick (KEYES):**
`X→IO1, Y→IO4, B→IO5, VCC→IO6, GND→GND`

> ⚠️ The joystick **GND must be a solid solder joint**. A loose GND makes both
> axes read high and the menu misbehaves. This firmware tolerates it, but a good
> joint is best.

Full table: [docs/connections.md](docs/connections.md).

## 4. Install PlatformIO

```bash
pip install platformio      # or install the VS Code PlatformIO extension
```

## 5. Build the firmware

```bash
git clone https://github.com/gpsn0w/esp32-c5-custom-bruce-firmware
cd esp32-c5-custom-bruce-firmware
./setup.sh                  # clones Bruce (pinned) + applies the patches
cd bruce
pio run -e esp32-c5-joy
```

The flashable image appears as `Bruce-esp32-c5-joy.bin`.
**Prefer not to build?** Use the ready-made `firmware/Bruce-esp32-c5-joy.bin`.

## 6. Flash it

```bash
ls /dev/cu.usbmodem*        # macOS   (Linux: ls /dev/ttyACM*)

esptool --chip esp32c5 -p PORT write-flash 0x0 firmware/Bruce-esp32-c5-joy.bin
```

If it won't connect: hold **BOOT**, tap **RST**, release **BOOT**, then flash.

## 7. First boot & controls

Press **RST** — you should see the Bruce menu.

| Action | Joystick |
|--------|----------|
| Navigate | Up / Down |
| **Select / OK** | **Right**, or center button |
| **Back** | **Left**, or long-press center |
| Keyboard (type text) | 4 directions move, short press types, long press = done |

> Tip: hold the stick still for ~1 s at boot — that's when the center is calibrated.

## 8. Try these first (free, built-in radio)

- **WiFi → Wifi Atks → Deauth** — disconnect devices from *your* network.
- **Bluetooth → BLE Spam** — flood nearby phones with fake BLE pop-ups.
- **Bluetooth → Bad BLE** — pick a `.txt` script, pair the target over Bluetooth, it types the script (this is "BadUSB" over Bluetooth — the C5 has no USB-OTG for cable BadUSB).
- **Files → WebUI** — browse/upload files from a browser (`http://172.0.0.1`).

## 9. Expand it (free GPIOs: IO2, IO3, IO7, IO25, IO26, IO28)

- **CC1101** → sub-GHz (433 MHz garages/remotes) — RF menu.
- **PN532** → NFC/RFID (access cards, door fobs, Amiibo) — I²C, only 2 pins.
- **NRF24L01+** → 2.4 GHz jam / MouseJack.
- **CH9329** → cable USB BadUSB (since the C5 can't do it natively).

## 10. Troubleshooting

- **Backlit but black screen** → the FSPI force-route handles this; a hardware RST
  pulse re-inits the panel, so just press **RST**.
- **Menu drifts on its own** → loose joystick GND; this build is edge-triggered so
  drift causes at most one step. Re-solder GND for perfection.
- **Boot-loops after using a feature (e.g. Evil Portal)** → the firmware
  **auto-recovers**: after 3 resets it wipes the bad config and boots clean. No PC needed.
- **BadUSB doesn't type** → use **Bad BLE** and pair from the target's Bluetooth
  *"Add device"* screen (not the main list).

---

Made for a Waveshare ESP32-C5 + ST7789V + KEYES joystick. See the
[README](README.md) for the technical details of every fix.
