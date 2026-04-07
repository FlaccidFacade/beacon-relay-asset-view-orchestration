#!/usr/bin/env bash
# run_all.sh — Full hardware-in-the-loop test on a Raspberry Pi 4B runner
#
# Orchestrates:
#   1. Build firmware for both device addresses
#   2. Flash both Pico W boards
#   3. Start RPi camera recording
#   4. Run communication tests
#   5. Stop camera recording
#   6. Collect artifacts
#
# Exit code reflects the test result (0 = pass, non-zero = fail).
# All logs and recordings are placed in firmware/pi-test/artifacts/.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ARTIFACTS_DIR="${SCRIPT_DIR}/artifacts"
export ARTIFACTS_DIR

mkdir -p "$ARTIFACTS_DIR"

echo "============================================="
echo "  B.R.A.V.O. Hardware-in-the-Loop Test"
echo "  $(date -u +%Y-%m-%dT%H:%M:%SZ)"
echo "============================================="
echo ""

# ------------------------------------------------------------------
# Step 1 — Build
# ------------------------------------------------------------------
echo ">>> Step 1/5: Build firmware"
if ! bash "$SCRIPT_DIR/build.sh" 2>&1 | tee "$ARTIFACTS_DIR/build.log"; then
    echo "FATAL: Build failed." >&2
    exit 1
fi
echo ""

# ------------------------------------------------------------------
# Step 2 — Flash
# ------------------------------------------------------------------
echo ">>> Step 2/5: Flash devices"
if ! bash "$SCRIPT_DIR/flash.sh" 2>&1 | tee "$ARTIFACTS_DIR/flash.log"; then
    echo "FATAL: Flash failed." >&2
    exit 1
fi
echo ""

# ------------------------------------------------------------------
# Step 3 — Start camera
# ------------------------------------------------------------------
echo ">>> Step 3/5: Start camera recording"
bash "$SCRIPT_DIR/record.sh" start "$ARTIFACTS_DIR" || true
echo ""

# ------------------------------------------------------------------
# Step 4 — Communication tests
# ------------------------------------------------------------------
echo ">>> Step 4/5: Run communication tests"
TEST_EXIT=0
bash "$SCRIPT_DIR/test_comms.sh" || TEST_EXIT=$?
echo ""

# ------------------------------------------------------------------
# Step 5 — Stop camera
# ------------------------------------------------------------------
echo ">>> Step 5/5: Stop camera recording"
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
