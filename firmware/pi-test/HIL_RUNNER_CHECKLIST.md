# Raspberry Pi Runner HIL Checklist

Use this checklist on the `self-hosted, 4b-01-bravo` runner before dispatching `Firmware Hardware Test` and while triaging failures in workflow logs.

## 1) Runner state

- [ ] Runner is online in GitHub and has labels: `self-hosted`, `4b-01-bravo`
- [ ] Repository workspace exists and is writable by runner user: `[ -n "$GITHUB_WORKSPACE" ] && [ -w "$GITHUB_WORKSPACE" ] && echo "$GITHUB_WORKSPACE"`

## 2) Required tools

- [ ] `bash --version`
- [ ] `git --version`
- [ ] `python3 --version`
- [ ] `pio --version`
- [ ] `openocd --version`
- [ ] `lsusb --version` (from `usbutils`, used to identify probe serial numbers)
- [ ] `rpicam-vid --version` (camera recording used by `pi-test/record.sh`)
- [ ] `udevadm --version`

## 3) USB / device visibility

- [ ] Two Pico W devices are connected over data-capable USB cables
- [ ] `lsusb | grep 2e8a: || echo "No RP2/RP2040 USB devices found"` (should show at least two devices)
- [ ] If flashing times out, put at least one Pico in BOOTSEL mode and verify: `lsblk -rn -o MOUNTPOINT,LABEL | awk '$2=="RPI-RP2" {print $1}'`
- [ ] Confirm the runner user can write to mounted `RPI-RP2` volume(s)

## 4) Quick local dry-run

From repo root:

```bash
cd firmware
chmod +x pi-test/*.sh
TEST_SUITE=all bash pi-test/run_all.sh
```

Expected failure mode to resolve first (seen in current CI logs):

```text
ERROR: Timeout waiting for 1 Pico(s) in BOOTSEL mode
FATAL: Flash failed.
```

## 5) If BOOTSEL timeout still occurs

- [ ] Re-seat both USB cables and avoid charge-only cables
- [ ] Check kernel events while reconnecting devices: `dmesg -w`
- [ ] Reboot the runner and re-check `lsusb` / BOOTSEL mount detection
