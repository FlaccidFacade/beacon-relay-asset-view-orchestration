#!/usr/bin/env bash
# setup_probes.sh — Flash two regular Pico boards with picoprobe (debugprobe) firmware
#
# Run this once on the HIL PC to turn two plain Pico boards into Pico Debuggers.
# After this script completes, both boards will enumerate as VID:PID 2e8a:000c
# and flash.sh / run_all.sh will detect them automatically.
#
# Usage:
#   bash firmware/pi-test/setup_probes.sh
#
# Requirements:
#   - curl or wget
#   - lsblk (from util-linux, already present on Raspberry Pi OS)
#   - Two standard Pico or Pico W boards connected via data-capable USB cables
#
# The script downloads the latest debugprobe.uf2 from the official release and
# then walks you through flashing each board one at a time in BOOTSEL mode.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
UF2_DIR="${SCRIPT_DIR}/out"
UF2_PATH="${UF2_DIR}/debugprobe.uf2"

DEBUGPROBE_RELEASES_URL="https://api.github.com/repos/raspberrypi/picoprobe/releases/latest"

# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

log()  { echo "[setup_probes] $*"; }
info() { echo "[setup_probes] INFO:  $*"; }
warn() { echo "[setup_probes] WARN:  $*" >&2; }
die()  { echo "[setup_probes] ERROR: $*" >&2; exit 1; }

wait_for_bootsel_mount() {
    local label="$1"
    local timeout_s="${2:-60}"
    local elapsed=0
    log "Waiting up to ${timeout_s}s for ${label} to mount as RPI-RP2 ..."
    while true; do
        local mp
        mp=$(lsblk -rn -o MOUNTPOINT,LABEL 2>/dev/null \
             | awk '$2=="RPI-RP2" && $1!="" {print $1}' \
             | head -n1)
        if [[ -n "$mp" ]]; then
            echo "$mp"
            return 0
        fi
        sleep 2
        elapsed=$((elapsed + 2))
        if [[ $elapsed -ge $timeout_s ]]; then
            die "Timed out waiting for RPI-RP2 mount. Check the USB cable (must be data-capable)."
        fi
    done
}

wait_for_probe() {
    local timeout_s="${1:-30}"
    local elapsed=0
    log "Waiting up to ${timeout_s}s for debugprobe (2e8a:000c) to appear ..."
    while true; do
        if lsusb -d 2e8a:000c &>/dev/null; then
            return 0
        fi
        sleep 2
        elapsed=$((elapsed + 2))
        if [[ $elapsed -ge $timeout_s ]]; then
            warn "Timed out waiting for 2e8a:000c. The board may have rebooted but isn't enumerating yet."
            return 1
        fi
    done
}

count_probes() {
    lsusb 2>/dev/null | grep -c "2e8a:000c" || true
}

# ---------------------------------------------------------------------------
# Step 0 — check requirements
# ---------------------------------------------------------------------------
log "Checking requirements ..."
command -v lsblk &>/dev/null || die "lsblk not found. Install util-linux."

DOWNLOAD_CMD=""
if command -v curl &>/dev/null; then
    DOWNLOAD_CMD="curl"
elif command -v wget &>/dev/null; then
    DOWNLOAD_CMD="wget"
else
    die "Neither curl nor wget found. Install one to download debugprobe.uf2."
fi

# ---------------------------------------------------------------------------
# Step 1 — download debugprobe.uf2
# ---------------------------------------------------------------------------
mkdir -p "$UF2_DIR"

if [[ -f "$UF2_PATH" ]]; then
    info "debugprobe.uf2 already present at $UF2_PATH — skipping download."
else
    log "Fetching latest debugprobe release URL ..."
    if [[ "$DOWNLOAD_CMD" == "curl" ]]; then
        DOWNLOAD_URL=$(curl -fsSL "$DEBUGPROBE_RELEASES_URL" \
            | grep '"browser_download_url"' \
            | grep 'debugprobe\.uf2' \
            | head -n1 \
            | sed 's/.*"browser_download_url": "\(.*\)"/\1/')
    else
        DOWNLOAD_URL=$(wget -qO- "$DEBUGPROBE_RELEASES_URL" \
            | grep '"browser_download_url"' \
            | grep 'debugprobe\.uf2' \
            | head -n1 \
            | sed 's/.*"browser_download_url": "\(.*\)"/\1/')
    fi

    [[ -n "$DOWNLOAD_URL" ]] || die "Could not find debugprobe.uf2 download URL. Check your internet connection."
    log "Downloading: $DOWNLOAD_URL"

    if [[ "$DOWNLOAD_CMD" == "curl" ]]; then
        curl -fsSL -o "$UF2_PATH" "$DOWNLOAD_URL"
    else
        wget -q -O "$UF2_PATH" "$DOWNLOAD_URL"
    fi

    info "Saved to: $UF2_PATH"
fi

# ---------------------------------------------------------------------------
# Step 2 — flash probe 1
# ---------------------------------------------------------------------------
echo ""
echo "========================================================"
echo "  PROBE 1 of 2"
echo "========================================================"
echo ""
echo "  Hold the BOOTSEL button on the FIRST Pico, plug it"
echo "  into this machine via USB, then release BOOTSEL."
echo ""
read -r -p "Press Enter when ready ..."

MOUNT=$(wait_for_bootsel_mount "Probe 1")
log "Detected RPI-RP2 at: $MOUNT"
log "Copying debugprobe.uf2 ..."
cp "$UF2_PATH" "$MOUNT/"
sync
log "Copy complete. Board will reboot automatically."

wait_for_probe 30 && info "Probe 1 is now online as 2e8a:000c." \
    || warn "Probe 1 not yet seen as 2e8a:000c — it may still be enumerating."

# ---------------------------------------------------------------------------
# Step 3 — flash probe 2
# ---------------------------------------------------------------------------
echo ""
echo "========================================================"
echo "  PROBE 2 of 2"
echo "========================================================"
echo ""
echo "  Unplug the first Pico (it's done), then hold BOOTSEL"
echo "  on the SECOND Pico, plug it in, and release BOOTSEL."
echo ""
read -r -p "Press Enter when ready ..."

MOUNT=$(wait_for_bootsel_mount "Probe 2")
log "Detected RPI-RP2 at: $MOUNT"
log "Copying debugprobe.uf2 ..."
cp "$UF2_PATH" "$MOUNT/"
sync
log "Copy complete. Board will reboot automatically."

wait_for_probe 30 && info "Probe 2 is now online as 2e8a:000c." \
    || warn "Probe 2 not yet seen as 2e8a:000c — it may still be enumerating."

# ---------------------------------------------------------------------------
# Step 4 — summary
# ---------------------------------------------------------------------------
echo ""
echo "========================================================"
echo "  SETUP COMPLETE"
echo "========================================================"
echo ""

PROBE_COUNT=$(count_probes)
info "Detected ${PROBE_COUNT} probe(s) with VID:PID 2e8a:000c."

if [[ "$PROBE_COUNT" -ge 2 ]]; then
    echo ""
    log "Serial numbers (for PROBE1_SERIAL / PROBE2_SERIAL):"
    if command -v lsusb &>/dev/null; then
        lsusb -v -d 2e8a:000c 2>/dev/null | grep iSerial | awk '{print "  " $3}'
    fi
    echo ""
    info "Both probes are ready. Wire each one to its target Pico W via SWD, then run:"
    echo ""
    echo "    TEST_SUITE=all bash firmware/pi-test/run_all.sh"
    echo ""
    echo "  See firmware/pi-test/PROBE_SETUP.md for wiring instructions."
else
    warn "Fewer than 2 probes detected. Reconnect both boards and run 'lsusb | grep 2e8a:000c' to verify."
fi
