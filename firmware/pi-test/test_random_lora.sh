#!/usr/bin/env bash
# test_random_lora.sh — Verify Device 1 generates/sends random numbers over LoRa to Device 2
#
# Monitors serial output from both Pico W devices and checks for:
#   1. Both devices complete setup ("[BRAVO] Setup complete" / "[Display] init OK" / "[LoRa] init OK")
#   2. Device 1 generates and logs "[RANDOM_TEST] TX generated number: <NUM>"
#   3. Device 2 receives and logs "[RANDOM_TEST] RX received number: <NUM>"
#   4. Checks that the transmitted and received random numbers match.
#
# Exit codes:
#   0 — all checks passed
#   1 — one or more checks failed
#
# Environment variables:
#   SERIAL_DEV1  — serial device for unit 1 (default: auto-detected)
#   SERIAL_DEV2  — serial device for unit 2 (default: auto-detected)
#   TEST_TIMEOUT — seconds to wait for packets (default: 30)
#   BAUD         — serial baud rate (default: 115200)

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ARTIFACTS_DIR="${ARTIFACTS_DIR:-${SCRIPT_DIR}/artifacts}"
mkdir -p "$ARTIFACTS_DIR"

# Auto-detect target Pico W ttyACM ports
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
    mapfile -t TARGET_TTYS < <(detect_target_ttys)
    if [[ ${#TARGET_TTYS[@]} -ge 2 ]]; then
        SERIAL_DEV1="${SERIAL_DEV1:-${TARGET_TTYS[0]}}"
        SERIAL_DEV2="${SERIAL_DEV2:-${TARGET_TTYS[1]}}"
    else
        SERIAL_DEV1="${SERIAL_DEV1:-/dev/ttyACM0}"
        SERIAL_DEV2="${SERIAL_DEV2:-/dev/ttyACM1}"
    fi
fi

BAUD="${BAUD:-115200}"
TEST_TIMEOUT="${TEST_TIMEOUT:-30}"

LOG1="${ARTIFACTS_DIR}/serial_device1.log"
LOG2="${ARTIFACTS_DIR}/serial_device2.log"
RESULT_FILE="${ARTIFACTS_DIR}/test_results.txt"

cleanup() {
    local pids=()
    [[ -n "${CAT_PID1:-}" ]] && pids+=("$CAT_PID1")
    [[ -n "${CAT_PID2:-}" ]] && pids+=("$CAT_PID2")
    for pid in "${pids[@]}"; do
        if kill -0 "$pid" 2>/dev/null; then
            kill "$pid" 2>/dev/null || true
        fi
    done
}
trap cleanup EXIT

echo "=== B.R.A.V.O. Random Number LoRa & Display Test ==="
echo "  Device 1: $SERIAL_DEV1"
echo "  Device 2: $SERIAL_DEV2"
echo "  Timeout:  ${TEST_TIMEOUT}s"
echo ""

if [[ ! -e "$SERIAL_DEV1" || ! -e "$SERIAL_DEV2" ]]; then
    echo "ERROR: One or both serial devices not found ($SERIAL_DEV1, $SERIAL_DEV2)" >&2
    exit 1
fi

stty -F "$SERIAL_DEV1" "$BAUD" raw -echo -echoe -echok 2>/dev/null || true
stty -F "$SERIAL_DEV2" "$BAUD" raw -echo -echoe -echok 2>/dev/null || true

# Start capturing logs
cat "$SERIAL_DEV1" >"$LOG1" 2>/dev/null &
CAT_PID1=$!

cat "$SERIAL_DEV2" >"$LOG2" 2>/dev/null &
CAT_PID2=$!

echo "Capturing serial traffic for ${TEST_TIMEOUT}s..."
sleep "$TEST_TIMEOUT"

cleanup

echo ""
echo "=== Test Results ==="

pass=0
fail=0

check() {
    local desc="$1"
    local file="$2"
    local pattern="$3"
    if grep -q "$pattern" "$file" 2>/dev/null; then
        echo "  PASS: $desc"
        pass=$((pass + 1))
    else
        echo "  FAIL: $desc"
        fail=$((fail + 1))
    fi
}

# 1. Boot checks
check "Device 1 setup complete" "$LOG1" "\[BRAVO\] Setup complete"
check "Device 2 setup complete" "$LOG2" "\[BRAVO\] Setup complete"
check "Device 1 display init"   "$LOG1" "\[Display\] init OK"
check "Device 2 display init"   "$LOG2" "\[Display\] init OK"
check "Device 1 LoRa init"      "$LOG1" "\[LoRa\] init OK"
check "Device 2 LoRa init"      "$LOG2" "\[LoRa\] init OK"

# 2. Random number TX & RX checks
check "Device 1 generated and sent random number" "$LOG1" "\[RANDOM_TEST\] TX generated number:"
check "Device 2 received random number"           "$LOG2" "\[RANDOM_TEST\] RX received number:"

# 3. Payload verification across logs
tx_nums=$(grep -o "\[RANDOM_TEST\] TX generated number: [0-9]*" "$LOG1" | awk '{print $NF}' || true)
rx_nums=$(grep -o "\[RANDOM_TEST\] RX received number: [0-9]*" "$LOG2" | awk '{print $NF}' || true)

match_found=0
for num in $tx_nums; do
    if echo "$rx_nums" | grep -qw "$num"; then
        echo "  PASS: Random number $num successfully transmitted by Dev1 and received by Dev2"
        match_found=1
        pass=$((pass + 1))
        break
    fi
done

if [[ $match_found -eq 0 ]]; then
    echo "  FAIL: No matching random numbers verified between Device 1 and Device 2 logs"
    fail=$((fail + 1))
fi

echo ""
echo "Summary: $pass passed, $fail failed"

{
    echo "suite: random_lora"
    echo "passed: $pass"
    echo "failed: $fail"
    echo "result: $( [ "$fail" -eq 0 ] && echo PASS || echo FAIL )"
} >> "$RESULT_FILE"

[ "$fail" -eq 0 ]
