#!/usr/bin/env bash
# test_display.sh — Verify OLED display output on a B.R.A.V.O. device
#
# Captures serial output from a single Pico W and checks for display-related
# log lines.  This is a minimal stub — extend the checks below as the
# firmware's display code matures.
#
# Exit codes:
#   0 — all checks passed
#   1 — one or more checks failed
#
# Environment variables:
#   SERIAL_DEV1  — serial device (default /dev/ttyACM0)
#   TEST_TIMEOUT — seconds to capture (default 60)
#   BAUD         — serial baud rate (default 115200)

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ARTIFACTS_DIR="${ARTIFACTS_DIR:-${SCRIPT_DIR}/artifacts}"
mkdir -p "$ARTIFACTS_DIR"

SERIAL_DEV1="${SERIAL_DEV1:-/dev/ttyACM0}"
BAUD="${BAUD:-115200}"
TEST_TIMEOUT="${TEST_TIMEOUT:-60}"

LOG1="${ARTIFACTS_DIR}/serial_device1.log"
RESULT_FILE="${ARTIFACTS_DIR}/test_results.txt"

cleanup() {
    local pid="${CAT_PID1:-}"
    if [ -n "$pid" ] && kill -0 "$pid" 2>/dev/null; then
        kill "$pid" 2>/dev/null || true
    fi
}
trap cleanup EXIT

# --- Preflight ---
echo "=== B.R.A.V.O. display test ==="
echo "  Device: $SERIAL_DEV1"
echo "  Timeout: ${TEST_TIMEOUT}s"
echo ""

if [ ! -e "$SERIAL_DEV1" ]; then
    echo "ERROR: Serial device not found: $SERIAL_DEV1" >&2
    echo "  Ensure the Pico W board is connected and has booted." >&2
    exit 1
fi

# Configure serial port
stty -F "$SERIAL_DEV1" "$BAUD" raw -echo -echoe -echok 2>/dev/null || true

# --- Start serial capture ---
cat "$SERIAL_DEV1" >"$LOG1" 2>/dev/null &
CAT_PID1=$!

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

check "Device booted"        "$LOG1" '\[BRAVO\] Setup complete'
check "Display initialised"  "$LOG1" '\[Display\] init'

echo ""
echo "Passed: $pass / $((pass + fail))"

# --- Write results file ---
{
    echo "B.R.A.V.O. Hardware Display Test"
    echo "================================"
    echo "Date:   $(date -u +%Y-%m-%dT%H:%M:%SZ)"
    echo "Device: $SERIAL_DEV1"
    echo ""
    echo "Results: $pass passed, $fail failed"
} >"$RESULT_FILE"

echo ""
echo "Logs saved to:"
echo "  $LOG1"
echo "  $RESULT_FILE"

if [ "$fail" -gt 0 ]; then
    exit 1
fi
