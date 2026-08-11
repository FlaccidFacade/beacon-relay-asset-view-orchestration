#!/usr/bin/env bash
# cleanup.sh — remove artifacts

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ARTIFACTS_DIR="${SCRIPT_DIR}/artifacts"
export ARTIFACTS_DIR

rm -r "${ARTIFACTS_DIR}"

echo "cleaned up ${ARTIFACTS_DIR} dir"
