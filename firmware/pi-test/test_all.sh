#!/usr/bin/env bash
# test_all.sh — Run every test_*.sh suite in sequence
#
# Discovers all test_<name>.sh scripts in the same directory (excluding
# itself) and executes them one by one.  The overall exit code is non-zero
# if any individual suite fails.
#
# Exit codes:
#   0 — all suites passed
#   1 — one or more suites failed

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ARTIFACTS_DIR="${ARTIFACTS_DIR:-${SCRIPT_DIR}/artifacts}"
export ARTIFACTS_DIR
mkdir -p "$ARTIFACTS_DIR"

overall_pass=0
overall_fail=0
failed_suites=()

echo "============================================="
echo "  B.R.A.V.O. Full Test Suite (all)"
echo "  $(date -u +%Y-%m-%dT%H:%M:%SZ)"
echo "============================================="
echo ""

for test_script in "$SCRIPT_DIR"/test_*.sh; do
    suite_name="$(basename "$test_script" .sh)"

    # Skip this script to avoid infinite recursion
    if [ "$suite_name" = "test_all" ]; then
        continue
    fi

    echo "---------------------------------------------"
    echo "  Running suite: ${suite_name#test_}"
    echo "---------------------------------------------"

    suite_exit=0
    bash "$test_script" || suite_exit=$?

    if [ "$suite_exit" -eq 0 ]; then
        echo "  SUITE PASSED: ${suite_name#test_}"
        overall_pass=$((overall_pass + 1))
    else
        echo "  SUITE FAILED: ${suite_name#test_} (exit code $suite_exit)"
        overall_fail=$((overall_fail + 1))
        failed_suites+=("${suite_name#test_}")
    fi
    echo ""
done

echo "============================================="
echo "  Results: $overall_pass passed, $overall_fail failed"
if [ "${#failed_suites[@]}" -gt 0 ]; then
    echo "  Failed suites: ${failed_suites[*]}"
fi
echo "============================================="

# --- Write combined results file ---
RESULT_FILE="${ARTIFACTS_DIR}/test_all_results.txt"
{
    echo "B.R.A.V.O. Full Test Suite"
    echo "=========================="
    echo "Date:    $(date -u +%Y-%m-%dT%H:%M:%SZ)"
    echo ""
    echo "Results: $overall_pass passed, $overall_fail failed"
    if [ "${#failed_suites[@]}" -gt 0 ]; then
        echo "Failed:  ${failed_suites[*]}"
    fi
} >"$RESULT_FILE"

if [ "$overall_fail" -gt 0 ]; then
    exit 1
fi
