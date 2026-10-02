#!/usr/bin/env bash
# Clone Bruce and apply the ESP32-C5-joy board definition + core patches.
set -e
HERE="$(cd "$(dirname "$0")" && pwd)"

# Pin to a known-good Bruce commit. Newer HEAD pulls a FastLED version whose
# FFT code fails to compile for the ESP32-C5 toolchain.
BRUCE_COMMIT=ba519c93
if [ ! -d bruce ]; then
  git clone https://github.com/BruceDevices/firmware.git bruce
fi
cd bruce
git checkout "$BRUCE_COMMIT"

# Pin FastLED exactly: 3.10.5 breaks the ESP32-C5 build (fixed-point/FFT code).
sed -i.bak 's#fastled/FastLED @^3.10.3#fastled/FastLED @3.10.3#' platformio.ini

# 1) board definition
mkdir -p boards/ESP32-C5-joy
cp "$HERE/boards/ESP32-C5-joy/"* boards/ESP32-C5-joy/

# 2) joyKbdMode global (menus vs on-screen keyboard mapping)
grep -q joyKbdMode src/main.cpp || sed -i.bak \
  's/volatile bool AnyKeyPress = false;/volatile bool AnyKeyPress = false;\nvolatile bool joyKbdMode = false;/' src/main.cpp
grep -q joyKbdMode include/globals.h || sed -i.bak \
  's/extern volatile bool EscPress;/extern volatile bool EscPress;\nextern volatile bool joyKbdMode;/' include/globals.h
python3 - <<'PY'
k="src/core/mykeyboard.cpp"; s=open(k).read()
if "joyKbdMode = true" not in s:
    i=s.index("String generalKeyboard("); b=s.index("{", i)
    s=s[:b+1]+"\n    joyKbdMode = true;"+s[b+1:]
    s=s.replace("    return current_text;","    joyKbdMode = false;\n    return current_text;",1)
    open(k,"w").write(s)
PY

# 3) expose TFT_eSPI::writedata (needed by the display re-init)
grep -q "using TFT_eSPI::writedata;" lib/HAL/display/tftespi.h || sed -i.bak \
  's/    using TFT_eSPI::writecommand;/    using TFT_eSPI::writecommand;\n    using TFT_eSPI::writedata;/' lib/HAL/display/tftespi.h

find . -name '*.bak' -delete 2>/dev/null || true
echo "Done. Build with:  cd bruce && pio run -e esp32-c5-joy"
