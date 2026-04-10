#!/usr/bin/env bash
# flash.sh — Flash UF2 images to two Pico W devices via USB mass-storage copy
#
# Usage:
#   ./flash.sh [out/device1.uf2] [out/device2.uf2]
#
# The script detects Pico W boards in BOOTSEL mode by looking for RPI-RP2
# USB mass-storage mount points, then copies the UF2 files to flash them.
#
# If boards are running firmware (not already in BOOTSEL mode), the script
# automatically reboots them into BOOTSEL via `picotool reboot -f -u` so
# that no physical button press is required (headless / CI friendly).
#
# If only one board is in BOOTSEL mode the script flashes that board, then
# waits for the second board before continuing.
#
# Requires: lsblk (usually pre-installed)
# Optional: picotool (for automatic BOOTSEL reboot of running boards)

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

# picotool is optional: used only to force-reboot running boards into BOOTSEL.
# If not available, boards must already be in BOOTSEL mode.
if ! command -v picotool &>/dev/null; then
    echo "WARNING: picotool not found. Automatic BOOTSEL reboot will be skipped." >&2
    echo "  Boards must be in BOOTSEL mode (hold BOOTSEL while plugging USB)." >&2
    echo "  Install with: sudo apt-get install -y picotool" >&2
fi

# --- Helpers ---
detect_picos() {
    # List mount points of Pico W boards in BOOTSEL mode.
    # In BOOTSEL mode the Pico W exposes a USB mass-storage device labelled
    # "RPI-RP2".  Returns one mount-point path per line.
    #
    # Primary method: lsblk (most reliable, works on all systemd-based distros)
    # Fallback: scan common auto-mount directories
    if command -v lsblk &>/dev/null; then
        local found
        found=$(lsblk -o LABEL,MOUNTPOINT -rn 2>/dev/null \
            | awk '$1 == "RPI-RP2" && $2 != "" { print $2 }')
        if [ -n "$found" ]; then
            echo "$found"
            return 0
        fi
    fi
    # Fallback: scan common mount directories for RPI-RP2
    for base in "/media/${USER:-}" "/media" "/run/media/${USER:-}" "/mnt"; do
        [ -d "$base" ] || continue
        for d in "$base"/RPI-RP2*; do
            [ -d "$d" ] && echo "$d"
        done
    done
    return 0
}

force_bootsel() {
    # Force any running (non-BOOTSEL) Pico W boards into BOOTSEL mode.
    # The arduino-pico core exposes a USB vendor reset interface that
    # picotool can use to reboot the device without the physical BOOTSEL
    # button:  picotool reboot -f -u
    #   -f  target a device that is NOT in BOOTSEL mode
    #   -u  reboot into USB/BOOTSEL mode (rather than application mode)
    # We attempt up to 2 reboots (one per expected board).
    if ! command -v picotool &>/dev/null; then
        echo "  picotool not found; skipping automatic BOOTSEL reboot."
        echo "  Boards must already be in BOOTSEL mode (hold BOOTSEL while plugging USB)."
        return 0
    fi
    echo "  Forcing running Pico W board(s) into BOOTSEL mode..."
    local rebooted=0
    for _ in 1 2; do
        if picotool reboot -f -u 2>/dev/null; then
            rebooted=$((rebooted + 1))
            # Wait for the device to finish rebooting into BOOTSEL mode
            sleep 2
        else
            break
        fi
    done

    if [ "$rebooted" -gt 0 ]; then
        echo "  Rebooted $rebooted device(s) into BOOTSEL mode."
        # Extra settle time for USB re-enumeration and auto-mount
        sleep 5
    else
        echo "  No running boards found (may already be in BOOTSEL or disconnected)."
    fi
}

flash_one() {
    local uf2="$1"
    local mount_point="$2"

    echo "  Flashing $(basename "$uf2") -> $mount_point"
    cp "$uf2" "$mount_point/"
    sync
    echo "  UF2 copied; device will reboot automatically."
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
            echo "FATAL: Flash failed." >&2
            echo "Hold BOOTSEL while plugging in the USB cable, then release." >&2
            exit 1
        fi
        sleep 2
        elapsed=$((elapsed + 2))
        echo "  Waiting for Pico W in BOOTSEL mode... (${elapsed}s / ${MAX_WAIT}s)"
    done
}

# --- Force running boards into BOOTSEL mode (headless / CI) ---
echo "=== Preparing devices ==="
force_bootsel

# --- Flash Device 1 ---
echo "=== Flashing Device 1 ==="
wait_for_pico 1
PICO_1="$(detect_picos | head -1)"
flash_one "$UF2_1" "$PICO_1"
echo "  Device 1 flashed and rebooting."

# Give Device 1 time to reboot out of BOOTSEL and unmount.
sleep 5

# --- Flash Device 2 ---
echo "=== Flashing Device 2 ==="
# If device 2 is no longer in BOOTSEL (e.g. it was reset), try forcing again.
if [ "$(detect_picos | wc -l)" -lt 1 ]; then
    force_bootsel
fi
wait_for_pico 1
PICO_2="$(detect_picos | head -1)"
flash_one "$UF2_2" "$PICO_2"
echo "  Device 2 flashed and rebooting."

sleep 3
echo ""
echo "=== Flash complete ==="
