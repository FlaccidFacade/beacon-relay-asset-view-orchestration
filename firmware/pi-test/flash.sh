#!/usr/bin/env bash
# flash.sh — Flash two Pico W boards via Pico Debugger (SWD / OpenOCD)
#
# Each Pico W has a dedicated Pico Debugger connected via SWD.  OpenOCD
# programs the ELF directly — no BOOTSEL button press or UF2 copy required.
#
# Required environment variables:
#   PROBE1_SERIAL  — USB serial number of the Pico Debugger for device 1
#   PROBE2_SERIAL  — USB serial number of the Pico Debugger for device 2
#
# To find probe serial numbers:
#   lsusb -v -d 2e8a:000c 2>/dev/null | grep iSerial
#
# Optional positional arguments (default to out/device{1,2}.elf):
#   $1  — path to ELF for device 1
#   $2  — path to ELF for device 2

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ELF_1="${1:-$SCRIPT_DIR/out/device1.elf}"
ELF_2="${2:-$SCRIPT_DIR/out/device2.elf}"

# --- preflight: probe serial numbers (optional; auto-detect if not provided) ---
detect_probe_serials() {
    for d in /sys/bus/usb/devices/*; do
        [[ -r "$d/idVendor" && -r "$d/idProduct" ]] || continue
        [[ "$(cat "$d/idVendor")" == "2e8a" && "$(cat "$d/idProduct")" == "000c" ]] || continue
        [[ -r "$d/serial" ]] && cat "$d/serial"
    done | awk 'NF' | sort -u
}

if [[ -z "${PROBE1_SERIAL:-}" || -z "${PROBE2_SERIAL:-}" ]]; then
    mapfile -t _PROBES < <(detect_probe_serials)
    if [[ ${#_PROBES[@]} -ge 2 ]]; then
        : "${PROBE1_SERIAL:=${_PROBES[0]}}"
        : "${PROBE2_SERIAL:=${_PROBES[1]}}"
        echo "INFO: Auto-detected PROBE1_SERIAL=$PROBE1_SERIAL PROBE2_SERIAL=$PROBE2_SERIAL"
    else
        echo "ERROR: PROBE1_SERIAL and PROBE2_SERIAL must both be set (or connect two Pico Debuggers)." >&2
        echo "  Example:" >&2
        echo "    PROBE1_SERIAL=E6614103E7924134 PROBE2_SERIAL=E6614103E7924135 bash flash.sh" >&2
        echo "  To list connected probe serials:" >&2
        echo "    lsusb -v -d 2e8a:000c 2>/dev/null | grep iSerial" >&2
        exit 1
    fi
fi

# --- preflight: ELF files ---
for f in "$ELF_1" "$ELF_2"; do
    [[ -f "$f" ]] || { echo "ERROR: Missing ELF: $f" >&2; exit 1; }
done

# --- preflight: openocd available ---
if ! command -v openocd &>/dev/null; then
    echo "ERROR: openocd not found on PATH." >&2
    echo "  Install: sudo apt-get install openocd" >&2
    exit 1
fi

# --- preflight: at least one Pico Debugger on USB ---
# Pico Debugger (picoprobe v2) USB VID:PID 2e8a:000c
detect_probe() {
    if command -v lsusb &>/dev/null; then
        lsusb -d 2e8a:000c &>/dev/null
    else
        local d
        for d in /sys/bus/usb/devices/*; do
            [[ -r "$d/idVendor" && -r "$d/idProduct" ]] || continue
            [[ "$(cat "$d/idVendor")" == "2e8a" && "$(cat "$d/idProduct")" == "000c" ]] && return 0
        done
        return 1
    fi
}

if ! detect_probe; then
    echo "ERROR: No Pico Debugger detected on USB (VID:PID 2e8a:000c)." >&2
    echo "  Ensure both debuggers are connected and powered." >&2
    exit 1
fi

# --- flash via SWD ---
flash_via_swd() {
    local elf="$1"
    local probe_serial="$2"
    local label="$3"

    echo "=== Flashing $label ==="
    echo "  ELF:   $elf"
    echo "  Probe: $probe_serial"

    openocd \
        -f interface/cmsis-dap.cfg \
        -f target/rp2040.cfg \
        -c "adapter serial $probe_serial" \
        -c "program $elf verify reset exit"

    echo "$label flashed OK"
    echo ""
}

flash_via_swd "$ELF_1" "$PROBE1_SERIAL" "Device 1"
flash_via_swd "$ELF_2" "$PROBE2_SERIAL" "Device 2"

echo "=== Flash complete ==="
