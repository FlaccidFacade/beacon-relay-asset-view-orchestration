#!/bin/bash
# Helper script to upload B.R.A.V.O. firmware to a Raspberry Pi Pico W
# Run this on a Raspberry Pi 4B with the Pico W connected via USB.
# Requires sudo for picotool installation if not already present.

set -e

echo "========================================="
echo "  B.R.A.V.O. Pico W Upload Helper"
echo "========================================="
echo ""

# ── Check for picotool ───────────────────────────────────────────────────────
if ! command -v picotool &> /dev/null; then
    echo "  picotool not found."
    read -r -p "  Install it now via apt-get? [y/N] " answer
    case "$answer" in
        [yY][eE][sS]|[yY])
            if ! sudo apt-get update -q && sudo apt-get install -y picotool; then
                echo "  ✗ Failed to install picotool."
                echo "    Install manually: sudo apt-get install picotool"
                echo "    Or use BOOTSEL drag-and-drop (see README.md)."
                exit 1
            fi
            ;;
        *)
            echo "  Skipping picotool install."
            echo "  Install manually: sudo apt-get install picotool"
            echo "  Or use BOOTSEL drag-and-drop (see README.md)."
            ;;
    esac
fi

# ── Detect Pico W ────────────────────────────────────────────────────────────
# In BOOTSEL mode the Pico W exposes a mass-storage device labelled RPI-RP2.
# Detect the mount point dynamically rather than assuming a fixed path.
PICO_MOUNT=""
if command -v lsblk &> /dev/null; then
    # Look for a block device labelled RPI-RP2 and find its mount point
    PICO_MOUNT=$(lsblk -o LABEL,MOUNTPOINT 2>/dev/null \
        | awk '/^RPI-RP2/ { print $2; exit }')
fi

if [ -n "$PICO_MOUNT" ] && [ -d "$PICO_MOUNT" ]; then
    echo "✓ Pico W found in BOOTSEL mode at $PICO_MOUNT"
else
    # Not in BOOTSEL — check for CDC serial (normal operation)
    SERIAL_PORT=""
    for p in /dev/ttyACM0 /dev/ttyACM1; do
        if [ -e "$p" ]; then
            SERIAL_PORT="$p"
            echo "✓ Pico W CDC serial found at $SERIAL_PORT"
            break
        fi
    done
    if [ -z "$SERIAL_PORT" ]; then
        echo ""
        echo "  ✗ Pico W not detected."
        echo "    Please connect the Pico W via USB while holding the BOOTSEL button,"
        echo "    then release BOOTSEL and run this script again."
        echo ""
        echo "    Alternatively, build first and then flash manually in BOOTSEL mode:"
        echo "      pio run --environment rpicow"
        echo "      picotool load .pio/build/rpicow/firmware.uf2 --force"
        exit 1
    fi
fi

# ── Build & upload via PlatformIO ─────────────────────────────────────────────
echo "Building and uploading firmware..."
~/.platformio/penv/bin/platformio run --environment rpicow --target upload

echo ""
echo "========================================="
echo "  ✓ Upload Successful!"
echo "  Monitor serial output with:"
echo "    pio device monitor --environment rpicow"
echo "  or:"
echo "    minicom -b 115200 -D /dev/ttyACM0"
echo "========================================="
