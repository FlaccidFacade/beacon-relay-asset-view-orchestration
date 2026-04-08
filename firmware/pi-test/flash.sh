#!/usr/bin/env bash
# flash.sh — Flash UF2 images to two Pico W devices via picotool
#
# Usage:
#   ./flash.sh [out/device1.uf2] [out/device2.uf2]
#
# The script detects Pico W boards in BOOTSEL mode, flashes the first image
# to the first board and the second image to the second board.
#
# If only one board is in BOOTSEL mode the script flashes that board, then
# waits for the second board before continuing.
#
# Requires: picotool  (sudo apt-get install -y picotool)

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
UF2_1="${1:-${SCRIPT_DIR}/out/device1.uf2}"
UF2_2="${2:-${SCRIPT_DIR}/out/device2.uf2}"
MAX_WAIT="${FLASH_WAIT_SECS:-60}"

# --- Preflight checks ---
for f in "$UF2_1" "$UF2_2"; do
    if [ ! -f "$f" ]; then
        echo "ERROR: UF2 not found: $f" >&2
        echo "Run build.sh first." >&2
        exit 1
    fi
done

if ! command -v picotool &>/dev/null; then
    echo "ERROR: picotool not found. Install with: sudo apt-get install -y picotool" >&2
    exit 1
fi

# --- Helpers ---
detect_picos() {
    # List bus addresses of Pico W boards in BOOTSEL mode.
    # Returns one line per board: "bus:address"
    picotool info -l 2>/dev/null \
        | grep -oP 'Device at \K[0-9]+:[0-9]+' || true
}

flash_one() {
    local uf2="$1"
    local bus_addr="$2"
    local bus="${bus_addr%%:*}"
    local addr="${bus_addr##*:}"

    echo "  Flashing $uf2 -> bus $bus addr $addr"
    picotool load "$uf2" --bus "$bus" --address "$addr" --force --verify
    if ! picotool reboot --bus "$bus" --address "$addr" 2>&1; then
        echo "  WARNING: picotool reboot failed for bus $bus addr $addr (device may need manual reset)"
    fi
}

wait_for_pico() {
    local needed=$1
    local elapsed=0
    while true; do
        local count
        count=$(detect_picos | wc -l)
        if [ "$count" -ge "$needed" ]; then
            return 0
        fi
        if [ "$elapsed" -ge "$MAX_WAIT" ]; then
            echo "ERROR: Timed out waiting for $needed Pico W board(s) in BOOTSEL mode." >&2
            echo "Hold BOOTSEL while plugging in the USB cable, then release." >&2
            exit 1
        fi
        sleep 2
        elapsed=$((elapsed + 2))
        echo "  Waiting for Pico W in BOOTSEL mode... (${elapsed}s / ${MAX_WAIT}s)"
    done
}

# --- Flash Device 1 ---
echo "=== Flashing Device 1 ==="
wait_for_pico 1
PICO_1="$(detect_picos | head -1)"
flash_one "$UF2_1" "$PICO_1"
echo "  Device 1 flashed and rebooting."

# Give Device 1 time to reboot out of BOOTSEL.
sleep 3

# --- Flash Device 2 ---
echo "=== Flashing Device 2 ==="
echo "Please put the second Pico W into BOOTSEL mode now."
wait_for_pico 1
PICO_2="$(detect_picos | head -1)"
flash_one "$UF2_2" "$PICO_2"
echo "  Device 2 flashed and rebooting."

sleep 3
echo ""
echo "=== Flash complete ==="
