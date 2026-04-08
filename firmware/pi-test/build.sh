#!/usr/bin/env bash
# build.sh — Build B.R.A.V.O. firmware for two Pico W devices
#
# Produces two UF2 images:
#   out/device1.uf2  (DEVICE_ADDRESS=1, TARGET_ADDRESS=2)
#   out/device2.uf2  (DEVICE_ADDRESS=2, TARGET_ADDRESS=1)
#
# Requires PlatformIO Core (pio) on the PATH or in ~/.platformio/penv/bin.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
FIRMWARE_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
OUT_DIR="${SCRIPT_DIR}/out"

# Resolve PlatformIO binary
PIO=""
if command -v pio &>/dev/null; then
    PIO="pio"
elif [ -x "$HOME/.platformio/penv/bin/platformio" ]; then
    PIO="$HOME/.platformio/penv/bin/platformio"
else
    echo "ERROR: PlatformIO not found. Install it first:" >&2
    echo "  curl -fsSL https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py -o get-platformio.py && python3 get-platformio.py" >&2
    exit 1
fi

echo "=== B.R.A.V.O. firmware build ==="
echo "PlatformIO: $PIO"
echo "Firmware:   $FIRMWARE_DIR"
echo ""

mkdir -p "$OUT_DIR"

# --- Build device 1 (beacon) ---
echo "--- Building Device 1 (DEVICE_ADDRESS=1, TARGET_ADDRESS=2) ---"
PLATFORMIO_BUILD_FLAGS="-D DEVICE_ADDRESS=1 -D TARGET_ADDRESS=2" \
    $PIO run -d "$FIRMWARE_DIR" -e rpicow

cp "$FIRMWARE_DIR/.pio/build/rpicow/firmware.uf2" "$OUT_DIR/device1.uf2"
echo "  -> out/device1.uf2"

# --- Build device 2 (relay) ---
echo "--- Building Device 2 (DEVICE_ADDRESS=2, TARGET_ADDRESS=1) ---"
PLATFORMIO_BUILD_FLAGS="-D DEVICE_ADDRESS=2 -D TARGET_ADDRESS=1" \
    $PIO run -d "$FIRMWARE_DIR" -e rpicow

cp "$FIRMWARE_DIR/.pio/build/rpicow/firmware.uf2" "$OUT_DIR/device2.uf2"
echo "  -> out/device2.uf2"

echo ""
echo "=== Build complete ==="
ls -lh "$OUT_DIR"/*.uf2
