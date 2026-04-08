#!/usr/bin/env bash
# record.sh — Start / stop RPi camera recording for hardware tests
#
# Usage:
#   ./record.sh start [output_dir]   # begin recording
#   ./record.sh stop                 # stop recording
#
# The recording uses rpicam-vid (Raspberry Pi OS Bookworm+) with a fallback
# to libcamera-vid and then raspivid for older images.
#
# The PID of the recording process is stored in a pidfile so that `stop`
# can gracefully terminate it.

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ARTIFACTS_DIR="${2:-${SCRIPT_DIR}/artifacts}"
PIDFILE="${SCRIPT_DIR}/.camera.pid"
TIMESTAMP="$(date +%Y%m%d_%H%M%S)"
VIDEO_FILE="${ARTIFACTS_DIR}/test_recording_${TIMESTAMP}.h264"
RECORD_TIMEOUT="${RECORD_TIMEOUT_SECS:-300}"

# --- Resolve camera tool ---
resolve_camera() {
    for cmd in rpicam-vid libcamera-vid raspivid; do
        if command -v "$cmd" &>/dev/null; then
            echo "$cmd"
            return 0
        fi
    done
    return 1
}

# --- Start recording ---
cmd_start() {
    mkdir -p "$ARTIFACTS_DIR"

    local cam
    cam="$(resolve_camera)" || {
        echo "WARNING: No RPi camera tool found (rpicam-vid / libcamera-vid / raspivid)." >&2
        echo "Camera recording will be skipped." >&2
        return 0
    }

    if [ -f "$PIDFILE" ]; then
        echo "WARNING: Camera appears to be already recording (pidfile exists)." >&2
        echo "Stopping previous recording first." >&2
        cmd_stop
    fi

    echo "=== Starting camera recording ==="
    echo "  Tool:   $cam"
    echo "  Output: $VIDEO_FILE"
    echo "  Timeout: ${RECORD_TIMEOUT}s"

    local cam_log="${ARTIFACTS_DIR}/camera.log"
    $cam \
        --timeout "$((RECORD_TIMEOUT * 1000))" \
        --width 1280 --height 720 \
        --framerate 15 \
        --output "$VIDEO_FILE" 2>"$cam_log" &
    local pid=$!
    echo "$pid" >"$PIDFILE"
    echo "  PID:    $pid"
    echo "  Recording started."
}

# --- Stop recording ---
cmd_stop() {
    if [ ! -f "$PIDFILE" ]; then
        echo "No active recording (pidfile not found)."
        return 0
    fi

    local pid
    pid="$(cat "$PIDFILE")"
    echo "=== Stopping camera recording (PID $pid) ==="

    if kill -0 "$pid" 2>/dev/null; then
        kill -INT "$pid" 2>/dev/null || true
        # Wait up to 10 seconds for graceful exit
        local waited=0
        while kill -0 "$pid" 2>/dev/null && [ "$waited" -lt 10 ]; do
            sleep 1
            waited=$((waited + 1))
        done
        if kill -0 "$pid" 2>/dev/null; then
            kill -KILL "$pid" 2>/dev/null || true
        fi
    fi

    rm -f "$PIDFILE"
    echo "  Recording stopped."

    if ls "${ARTIFACTS_DIR}"/test_recording_*.h264 1>/dev/null 2>&1; then
        echo "  Recordings saved in ${ARTIFACTS_DIR}:"
        ls -lh "${ARTIFACTS_DIR}"/test_recording_*.h264
    fi
}

# --- Main ---
case "${1:-}" in
    start) cmd_start ;;
    stop)  cmd_stop ;;
    *)
        echo "Usage: $0 {start|stop} [output_dir]" >&2
        exit 1
        ;;
esac
