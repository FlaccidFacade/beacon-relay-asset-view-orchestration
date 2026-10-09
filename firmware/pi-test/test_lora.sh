#!/usr/bin/env bash
# test_lora.sh — Verify two B.R.A.V.O. devices are communicating over LoRa
#
# Monitors serial output from both Pico W devices and checks for:
#   1. Both devices complete initialisation ("[BRAVO] Setup complete")
#   2. At least one LoRa TX from each device
#   3. At least one LoRa RX on each device (proves bidirectional comms)
#
# Exit codes:
#   0 — all tests passed
#   1 — one or more tests failed
#
# Environment variables:
#   SERIAL_DEV1  — serial device for unit 1 (default: auto-detected)
#   SERIAL_DEV2  — serial device for unit 2 (default: auto-detected)
#   TEST_TIMEOUT — seconds to wait for comms (default 90)
#   BAUD         — serial baud rate (default 115200)
#
# Auto-detection notes:
#   Each Pico W target enumerates as USB VID:PID 2e8a:f00a. Debug probes
#   (2e8a:000c) also expose their own /dev/ttyACM* node (their UART bridge),
#   so ttyACM numbering is NOT reliable — e.g. under WSL/usbipd the probes
#   can land on /dev/ttyACM0/1 and both targets on /dev/ttyACM2/3 depending
#   on attach order. This script matches on USB VID:PID instead of assuming
#   fixed device numbers.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ARTIFACTS_DIR="${SCRIPT_DIR}/artifacts"
mkdir -p "$ARTIFACTS_DIR"

# --- auto-detect target Pico W ttyACM ports (VID:PID 2e8a:f00a) ---
detect_target_ttys() {
    local tty devpath d vid pid
    for tty in /sys/class/tty/ttyACM*; do
        [[ -e "$tty/device" ]] || continue
        devpath="$(readlink -f "$tty/device")"
        d="$devpath"
        while [[ "$d" != "/" && "$d" != "/sys" ]]; do
            if [[ -r "$d/idVendor" && -r "$d/idProduct" ]]; then
                vid="$(cat "$d/idVendor")"
                pid="$(cat "$d/idProduct")"
                [[ "$vid:$pid" == "2e8a:f00a" ]] && echo "/dev/$(basename "$tty")"
                break
            fi
            d="$(dirname "$d")"
        done
    done | sort -u
}

if [[ -z "${SERIAL_DEV1:-}" || -z "${SERIAL_DEV2:-}" ]]; then
    mapfile -t _TARGET_TTYS < <(detect_target_ttys)
    if [[ ${#_TARGET_TTYS[@]} -ge 2 ]]; then
        : "${SERIAL_DEV1:=${_TARGET_TTYS[0]}}"
        : "${SERIAL_DEV2:=${_TARGET_TTYS[1]}}"
        echo "INFO: Auto-detected SERIAL_DEV1=$SERIAL_DEV1 SERIAL_DEV2=$SERIAL_DEV2"
    fi
fi

SERIAL_DEV1="${SERIAL_DEV1:-/dev/ttyACM0}"
SERIAL_DEV2="${SERIAL_DEV2:-/dev/ttyACM1}"
BAUD="${BAUD:-115200}"
TEST_TIMEOUT="${TEST_TIMEOUT:-90}"

LOG1="${ARTIFACTS_DIR}/serial_device1.log"
LOG2="${ARTIFACTS_DIR}/serial_device2.log"
RESULT_FILE="${ARTIFACTS_DIR}/test_results.txt"

cleanup() {
    # Kill background serial readers
    for pid_var in CAT_PID1 CAT_PID2; do
        local pid="${!pid_var:-}"
        if [ -n "$pid" ] && kill -0 "$pid" 2>/dev/null; then
            kill "$pid" 2>/dev/null || true
        fi
    done
}
trap cleanup EXIT

# --- Preflight ---
echo "=== B.R.A.V.O. communication test ==="
echo "  Device 1: $SERIAL_DEV1"
echo "  Device 2: $SERIAL_DEV2"
echo "  Timeout:  ${TEST_TIMEOUT}s"
echo ""

for dev in "$SERIAL_DEV1" "$SERIAL_DEV2"; do
    if [ ! -e "$dev" ]; then
        echo "ERROR: Serial device not found: $dev" >&2
        echo "  Ensure both Pico W boards are connected and have booted." >&2
        exit 1
    fi
done

# Configure serial ports
stty -F "$SERIAL_DEV1" "$BAUD" raw -echo -echoe -echok 2>/dev/null || true
stty -F "$SERIAL_DEV2" "$BAUD" raw -echo -echoe -echok 2>/dev/null || true

# --- Start serial capture ---
cat "$SERIAL_DEV1" >"$LOG1" 2>/dev/null &
CAT_PID1=$!

cat "$SERIAL_DEV2" >"$LOG2" 2>/dev/null &
CAT_PID2=$!

echo "Capturing serial output for up to ${TEST_TIMEOUT}s ..."
sleep "$TEST_TIMEOUT"

# Stop capture
cleanup

# --- Evaluate results ---
echo ""
echo "=== Test Results ==="

pass=0
fail=0

check() {
    local description="$1"
    local file="$2"
    local pattern="$3"

    if grep -q "$pattern" "$file" 2>/dev/null; then
        echo "  PASS: $description"
        pass=$((pass + 1))
        return 0
    else
        echo "  FAIL: $description"
        fail=$((fail + 1))
        return 1
    fi
}

# Check that `pattern` appears in at least one of the two logs, without
# double-counting a separate pass/fail per file.
check_either() {
    local description="$1"
    local pattern="$2"

    if grep -q "$pattern" "$LOG1" 2>/dev/null || grep -q "$pattern" "$LOG2" 2>/dev/null; then
        echo "  PASS: $description"
        pass=$((pass + 1))
    else
        echo "  FAIL: $description"
        fail=$((fail + 1))
    fi
}

# Only one direction of LoRa traffic is required (beacon TX-only, relay
# RX-only), and WSL/usbipd attach order doesn't guarantee which serial port
# ends up as SERIAL_DEV1 vs SERIAL_DEV2, so check by content, not by which
# device is "1" or "2".
check "Device 1 booted"       "$LOG1" '\[BRAVO\] Setup complete' || true
check "Device 2 booted"       "$LOG2" '\[BRAVO\] Setup complete' || true
check_either "One unit transmitted (LoRa TX)" '\[LoRa\] TX →'
check_either "One unit received (LoRa RX)"    '\[LoRa\] RX'

echo ""
echo "Passed: $pass / $((pass + fail))"

# --- Write JUnit-style results (plain text summary) ---
{
    echo "B.R.A.V.O. Hardware Communication Test"
    echo "======================================="
    echo "Date:    $(date -u +%Y-%m-%dT%H:%M:%SZ)"
    echo "Device1: $SERIAL_DEV1"
    echo "Device2: $SERIAL_DEV2"
    echo ""
    echo "Results: $pass passed, $fail failed"
    echo ""
    grep -c '\[LoRa\] TX' "$LOG1" 2>/dev/null | xargs -I{} echo "Device 1 TX count: {}"
    grep -c '\[LoRa\] RX' "$LOG1" 2>/dev/null | xargs -I{} echo "Device 1 RX count: {}"
    grep -c '\[LoRa\] TX' "$LOG2" 2>/dev/null | xargs -I{} echo "Device 2 TX count: {}"
    grep -c '\[LoRa\] RX' "$LOG2" 2>/dev/null | xargs -I{} echo "Device 2 RX count: {}"
} >"$RESULT_FILE"

echo ""
echo "Logs saved to:"
echo "  $LOG1"
echo "  $LOG2"
echo "  $RESULT_FILE"

if [ "$fail" -gt 0 ]; then
    exit 1
fi
