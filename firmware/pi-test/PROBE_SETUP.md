# Probe Setup Guide

This guide covers turning two standard Pico (or Pico W) boards into Pico Debuggers using the [debugprobe](https://github.com/raspberrypi/picoprobe) firmware, and wiring them to the two target Pico W boards for HIL testing.

> **On Windows 10 + WSL?** See [`WSL_SETUP.md`](./WSL_SETUP.md) first — you need to
> attach both target Pico W boards and both debug probes to WSL via `usbipd-win`
> before `flash.sh` / `run_all.sh` can see them.

## Automated setup

Run this once on the HIL PC from the repo root:

```bash
bash firmware/pi-test/setup_probes.sh
```

The script will:
1. Download the latest `debugprobe.uf2` from the official release
2. Walk you through putting each probe Pico into BOOTSEL mode and copying the firmware
3. Confirm both boards enumerate as `2e8a:000c` (Pico Debugger)
4. Print the USB serial numbers you need for `PROBE1_SERIAL` / `PROBE2_SERIAL`

After the script completes, `flash.sh` will auto-detect the probes and no environment variables are needed.

## Wiring

Wire each probe Pico to its target Pico W using three jumper wires:

| Probe Pico pin | Target Pico W pin | Signal |
|---|---|---|
| GP2 | SWDIO | SWD data |
| GP3 | SWCLK | SWD clock |
| GND | GND | Ground |

Optionally connect the UART for serial capture during tests:

| Probe Pico pin | Target Pico W pin | Signal |
|---|---|---|
| GP0 (TX) | GP1 (RX) | UART from target |
| GP1 (RX) | GP0 (TX) | UART to target |

> **Note:** Keep the probe Pico powered via its own USB connection to the Pi. Do not power the target Pico W from the probe — connect the target's USB separately (or power it from the probe's VSYS pin if your cable setup allows, but a separate USB connection is simpler and more reliable).

## Verify

After wiring and connecting all four boards:

```bash
# Should show two 2e8a:000c entries
lsusb | grep 2e8a:000c

# Print serial numbers
lsusb -v -d 2e8a:000c 2>/dev/null | grep iSerial
```

## Running HIL tests

Once both probes are detected:

```bash
cd firmware
TEST_SUITE=all bash pi-test/run_all.sh
```

`flash.sh` auto-detects and assigns probe serials. To pin a specific probe to a specific device, set the environment variables explicitly:

```bash
PROBE1_SERIAL=E661... PROBE2_SERIAL=E661... bash pi-test/run_all.sh
```
