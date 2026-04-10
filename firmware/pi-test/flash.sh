#!/usr/bin/env bash
# flash.sh — CI-safe dual Pico W UF2 flasher using BOOTSEL mounts

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
UF2_1="${1:-$SCRIPT_DIR/out/device1.uf2}"
UF2_2="${2:-$SCRIPT_DIR/out/device2.uf2}"
MAX_WAIT="${FLASH_WAIT_SECS:-60}"

# --- checks ---
for f in "$UF2_1" "$UF2_2"; do
    [[ -f "$f" ]] || { echo "ERROR: Missing UF2: $f" >&2; exit 1; }
done

# --- detect BOOTSEL mounts (clean parsing) ---
detect_mounts() {
    lsblk -rn -o MOUNTPOINT,LABEL | awk '$2=="RPI-RP2" {print $1}'
}

# --- force running boards into BOOTSEL (CI/headless safe) ---
force_bootsel() {
    echo "=== Forcing boards into BOOTSEL mode ==="
    local rebooted=0

    for _ in 1 2; do
        if picotool reboot -f -u 2>/dev/null; then
            rebooted=$((rebooted + 1))
            sleep 2
        else
            break
        fi
    done

    if [[ "$rebooted" -gt 0 ]]; then
        echo "  Rebooted $rebooted device(s) into BOOTSEL"
        sleep 3
    else
        echo "  No running devices found or already in BOOTSEL"
    fi
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

flash_to_mount() {
    local uf2="$1"
    local mount="$2"

    if [[ ! -w "$mount" ]]; then
        echo "ERROR: Mount not writable: $mount" >&2
        exit 1
    fi

    echo "Flashing $uf2 -> $mount"

    if ! cp "$uf2" "$mount/"; then
        echo "ERROR: Failed to copy $uf2 to $mount" >&2
        exit 1
    fi

    sync
}

# --- MAIN ---
echo "=== Preparing devices ==="
force_bootsel

# --- Device 1 ---
echo "=== Flashing Device 1 ==="
wait_for_devices 1
M1=$(detect_mounts | head -n 1)

flash_to_mount "$UF2_1" "$M1"
echo "Device 1 flashed"

sleep 2

# --- Device 2 ---
echo "=== Flashing Device 2 ==="
wait_for_devices 1

# safer exclusion (exact match, not regex-based)
M2=$(detect_mounts | grep -Fvx "$M1" | head -n 1)

if [[ -z "$M2" ]]; then
    echo "ERROR: No second Pico mount found (Device 1 may not have unmounted yet)" >&2
    exit 1
fi

flash_to_mount "$UF2_2" "$M2"
echo "Device 2 flashed"

echo "=== Flash complete ==="
