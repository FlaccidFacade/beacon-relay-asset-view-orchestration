#!/usr/bin/env bash
# flash.sh — CI-safe dual Pico W UF2 flasher (robust BOOTSEL handling)
#
# Supports two modes:
#   1. Direct USB BOOTSEL (default) — Pico W held in BOOTSEL at plug-in.
#   2. Pico Probe SWD     — A Pico H running picoprobe forces BOOTSEL
#      remotely via `picotool reboot -f -u`, eliminating the need to
#      solder the BOOTSEL pad or physically press the button.
#
# The script auto-detects a connected picoprobe (USB VID:PID 2e8a:0004).

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
UF2_1="${1:-$SCRIPT_DIR/out/device1.uf2}"
UF2_2="${2:-$SCRIPT_DIR/out/device2.uf2}"
MAX_WAIT="${FLASH_WAIT_SECS:-60}"

# --- checks ---
for f in "$UF2_1" "$UF2_2"; do
    [[ -f "$f" ]] || { echo "ERROR: Missing UF2: $f" >&2; exit 1; }
done

# --- picoprobe detection ---
# Pico H running picoprobe exposes USB VID:PID 2e8a:0004.
detect_picoprobe() {
    if command -v lsusb &>/dev/null; then
        lsusb -d 2e8a:0004 &>/dev/null
    else
        # Fall back to sysfs when lsusb is not available
        grep -rqs "2e8a" /sys/bus/usb/devices/*/idVendor 2>/dev/null \
            && grep -rqs "0004" /sys/bus/usb/devices/*/idProduct 2>/dev/null
    fi
}

PICOPROBE_DETECTED=false
if detect_picoprobe; then
    PICOPROBE_DETECTED=true
    echo "✓ Pico Probe (picoprobe) detected on USB"
else
    echo "  No Pico Probe detected — using direct USB BOOTSEL mode"
fi

# --- detect BOOTSEL mounts ---
detect_mounts() {
    lsblk -rn -o MOUNTPOINT,LABEL \
        | awk '$2=="RPI-RP2" {print $1}'
}

wait_for_devices() {
    local needed=$1
    local elapsed=0

    while true; do
        count=$(detect_mounts | wc -l | tr -d ' ')

        if [[ "$count" -ge "$needed" ]]; then
            return 0
        fi

        if [[ "$elapsed" -ge "$MAX_WAIT" ]]; then
            echo "ERROR: Timeout waiting for $needed Pico(s) in BOOTSEL mode" >&2
            exit 1
        fi

        echo "Waiting for Pico(s) in BOOTSEL mode... (${elapsed}s / ${MAX_WAIT}s)"
        sleep 2
        elapsed=$((elapsed + 2))
    done
}

force_bootsel() {
    echo "=== Forcing BOOTSEL mode ==="
    local rebooted=0

    if [[ "$PICOPROBE_DETECTED" == "true" ]]; then
        echo "  Using Pico Probe (SWD) to force BOOTSEL — no physical button press needed"
    fi

    for _ in 1 2; do
        if picotool reboot -f -u 2>/dev/null; then
            rebooted=$((rebooted + 1))
            sleep 2
        else
            break
        fi
    done

    echo "  Rebooted $rebooted device(s) (if any running)"
    sleep 3
}

snapshot_mounts() {
    detect_mounts | sort
}

flash_to_mount() {
    local uf2="$1"
    local mount="$2"

    [[ -n "$mount" ]] || {
        echo "ERROR: Empty mount target" >&2
        exit 1
    }

    [[ -w "$mount" ]] || {
        echo "ERROR: Mount not writable: $mount" >&2
        exit 1
    }

    echo "Flashing $uf2 -> $mount"

    if ! cp "$uf2" "$mount/"; then
        echo "ERROR: Failed to copy $uf2 -> $mount" >&2
        exit 1
    fi

    sync
}

# --- MAIN ---
echo "=== Preparing devices ==="
force_bootsel

# --- DEVICE 1 ---
echo "=== Flashing Device 1 ==="
wait_for_devices 1

mapfile -t BEFORE < <(snapshot_mounts)
M1="${BEFORE[0]:-}"

flash_to_mount "$UF2_1" "$M1"
echo "Device 1 flashed"

# allow full USB re-enumeration after reboot
sleep 5
sync

# ensure OS settles USB state
udevadm settle 2>/dev/null || true

# --- DEVICE 2 ---
echo "=== Flashing Device 2 ==="

wait_for_devices 1

mapfile -t AFTER < <(snapshot_mounts)

# pick first DIFFERENT mount from device 1
M2=""
for m in "${AFTER[@]}"; do
    if [[ "$m" != "$M1" ]]; then
        M2="$m"
        break
    fi
done

if [[ -z "$M2" ]]; then
    echo "ERROR: Could not isolate second Pico mount" >&2
    echo "Detected mounts:" >&2
    printf '%s\n' "${AFTER[@]}" >&2
    exit 1
fi

flash_to_mount "$UF2_2" "$M2"
echo "Device 2 flashed"

echo "=== Flash complete ==="
