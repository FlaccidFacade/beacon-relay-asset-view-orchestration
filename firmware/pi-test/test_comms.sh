#!/usr/bin/env bash
# test_comms.sh — Verify two B.R.A.V.O. devices are communicating over LoRa
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
#   SERIAL_DEV1  — serial device for unit 1 (default /dev/ttyACM0)
#   SERIAL_DEV2  — serial device for unit 2 (default /dev/ttyACM1)
#   TEST_TIMEOUT — seconds to wait for comms (default 90)
#   BAUD         — serial baud rate (default 115200)

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ARTIFACTS_DIR="${SCRIPT_DIR}/artifacts"
mkdir -p "$ARTIFACTS_DIR"

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
    else
        echo "  FAIL: $description"
        fail=$((fail + 1))
    fi
}

# Device 1 checks
check "Device 1 booted"       "$LOG1" '\[BRAVO\] Setup complete'
check "Device 1 LoRa TX"      "$LOG1" '\[LoRa\] TX'
check "Device 1 LoRa RX"      "$LOG1" '\[LoRa\] RX'

# Device 2 checks
check "Device 2 booted"       "$LOG2" '\[BRAVO\] Setup complete'
check "Device 2 LoRa TX"      "$LOG2" '\[LoRa\] TX'
check "Device 2 LoRa RX"      "$LOG2" '\[LoRa\] RX'

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
