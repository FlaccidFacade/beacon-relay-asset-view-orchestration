#!/usr/bin/env bash
# run_all.sh — Full hardware-in-the-loop test on a Raspberry Pi 4B runner
#
# Orchestrates:
#   1. Build firmware for both device addresses
#   2. Start RPi camera recording (captures flash + test)
#   3. Flash both Pico W boards
#   4. Run selected test suite
#   5. Stop camera recording
#   6. Collect artifacts
#
# Exit code reflects the test result (0 = pass, non-zero = fail).
# All logs and recordings are placed in firmware/pi-test/artifacts/.
#
# Environment variables:
#   TEST_SUITE — name of the test to run (default: "comms")
#                Maps to firmware/pi-test/test_<name>.sh
#                Examples: "comms", "display"

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ARTIFACTS_DIR="${SCRIPT_DIR}/artifacts"
export ARTIFACTS_DIR

TEST_SUITE="${TEST_SUITE:-comms}"
TEST_SCRIPT="${SCRIPT_DIR}/test_${TEST_SUITE}.sh"

if [ ! -f "$TEST_SCRIPT" ]; then
    echo "FATAL: Unknown test suite '${TEST_SUITE}'." >&2
    echo "  Expected script: ${TEST_SCRIPT}" >&2
    echo "  Available suites:" >&2
    for f in "$SCRIPT_DIR"/test_*.sh; do
        name="$(basename "$f" .sh)"
        echo "    ${name#test_}" >&2
    done
    exit 1
fi

mkdir -p "$ARTIFACTS_DIR"

echo "============================================="
echo "  B.R.A.V.O. Hardware-in-the-Loop Test"
echo "  Suite: ${TEST_SUITE}"
echo "  $(date -u +%Y-%m-%dT%H:%M:%SZ)"
echo "============================================="
echo ""

# ------------------------------------------------------------------
# Step 1 — Build
# ------------------------------------------------------------------
echo ">>> Step 1/6: Build firmware"
if ! bash "$SCRIPT_DIR/build.sh" 2>&1 | tee "$ARTIFACTS_DIR/build.log"; then
    echo "FATAL: Build failed." >&2
    exit 1
fi
echo ""

# ------------------------------------------------------------------
# Step 2 — Start camera (before flash so the full process is recorded)
# ------------------------------------------------------------------
echo ">>> Step 2/6: Start camera recording"
bash "$SCRIPT_DIR/record.sh" start "$ARTIFACTS_DIR" || true
echo ""

# ------------------------------------------------------------------
# Step 3 — Flash
# ------------------------------------------------------------------
echo ">>> Step 3/6: Flash devices"
if ! bash "$SCRIPT_DIR/flash.sh" 2>&1 | tee "$ARTIFACTS_DIR/flash.log"; then
    echo "FATAL: Flash failed." >&2
    bash "$SCRIPT_DIR/record.sh" stop || true
    exit 1
fi
echo ""

# ------------------------------------------------------------------
# Step 4 — Run selected test suite
# ------------------------------------------------------------------
echo ">>> Step 4/6: Run test suite '${TEST_SUITE}'"
TEST_EXIT=0
bash "$TEST_SCRIPT" || TEST_EXIT=$?
echo ""

# ------------------------------------------------------------------
# Step 5 — Stop camera
# ------------------------------------------------------------------
echo ">>> Step 5/6: Stop camera recording"
bash "$SCRIPT_DIR/record.sh" stop || true
echo ""

# ------------------------------------------------------------------
# Summary
# ------------------------------------------------------------------
echo "============================================="
if [ "$TEST_EXIT" -eq 0 ]; then
    echo "  ALL TESTS PASSED"
else
    echo "  TESTS FAILED (exit code $TEST_EXIT)"
fi
echo "============================================="
echo ""
echo "Artifacts:"
ls -lhR "$ARTIFACTS_DIR" 2>/dev/null || true

exit "$TEST_EXIT"
