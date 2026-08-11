# Running HIL Flash/Test Locally from Windows 10 + WSL

The `pi-test/` scripts (`build.sh`, `flash.sh`, `test_lora.sh`, `run_all.sh`)
are plain bash and work fine inside WSL — the only WSL-specific step is
getting the USB devices attached to the Linux side, since WSL2 has no native
USB passthrough.

You have **4 USB connections** to manage (2 per Pico W):

| Connection                              | Shows up as                    | Used by              |
| --------------------------------------- | ------------------------------ | -------------------- |
| Pico W #1 — direct USB (CDC serial)     | `/dev/ttyACM0` (or similar)    | `test_lora.sh`       |
| Pico W #2 — direct USB (CDC serial)     | `/dev/ttyACM1` (or similar)    | `test_lora.sh`       |
| Debug probe #1 — USB (SWD to Pico W #1) | `2e8a:000c` "Pico Debug Probe" | `flash.sh` (openocd) |
| Debug probe #2 — USB (SWD to Pico W #2) | `2e8a:000c` "Pico Debug Probe" | `flash.sh` (openocd) |

## 1. One-time Windows host setup

1. Install [usbipd-win](https://github.com/dorssel/usbipd-win/releases) (run the installer as Administrator).
2. Restart your WSL distro: `wsl --shutdown` from PowerShell, then reopen your terminal.
3. Inside WSL, install the client tools that match your distro's `usbip` version:
   ```bash
   sudo apt-get update
   sudo apt-get install -y linux-tools-generic hwdata usbutils
   sudo update-alternatives --install /usr/local/bin/usbip usbip \
       "$(compgen -G "/usr/lib/linux-tools/*/usbip" | head -n1)" 20
   ```

## 2. Attach all 4 devices

From an **elevated (Administrator)** PowerShell on Windows:

```powershell
usbipd list
```

Identify the BUSIDs for:

- Two entries that look like `USB Serial Device` / `RP2 Boot` (the Pico W targets)
- Two entries that look like `Pico Debug Probe` / `CMSIS-DAP` (the debuggers)

Bind each once (one-time, persists across reboots), then attach:

```powershell
usbipd bind --busid <busid>
usbipd attach --wsl --busid <busid> --auto-attach
```

Repeat for all 4 BUSIDs. Use `--auto-attach` so the device automatically
reattaches after it resets/re-enumerates — this matters here because:

- Flashing over SWD resets the target Pico W's USB CDC connection.
- BOOTSEL fallback flashing causes the target to disconnect and remount.

Keep a PowerShell window open running `usbipd attach --wsl --busid <busid> --auto-attach`
per device (or use `usbipd attach --wsl --busid <busid> --auto-attach` in the
background) so reconnects are picked up automatically during repeated
flash/test cycles.

## 3. Verify inside WSL

```bash
lsusb | grep 2e8a
#   ... 2e8a:000c ... (x2 — the two debug probes)

ls /dev/ttyACM*
#   /dev/ttyACM0  /dev/ttyACM1  (the two Pico W CDC serial ports)

sudo usermod -aG dialout "$USER"   # then log out/in, or `newgrp dialout`
```

## 4. Install the build/flash toolchain in WSL

```bash
pip install --user platformio
sudo apt-get install -y openocd picotool
openocd --version
```

## 5. Build, flash, and test (both devices in parallel)

```bash
cd firmware
chmod +x pi-test/*.sh
./pi-test/build.sh                 # builds device1.elf/uf2 and device2.elf/uf2
./pi-test/flash.sh                 # flashes BOTH probes concurrently over SWD
TEST_SUITE=lora bash pi-test/run_all.sh
```

`flash.sh` auto-detects probe serials (`PROBE1_SERIAL` / `PROBE2_SERIAL`) and
now flashes both devices **in parallel** (each `openocd` instance uses
distinct gdb/tcl/telnet ports so they don't collide). `test_lora.sh` already
captures both devices' serial output concurrently and checks for LoRa TX/RX
on each side to confirm wireless communication.

If you don't have debug probes wired up, unset/omit `PROBE1_SERIAL` /
`PROBE2_SERIAL` and `flash.sh` falls back to BOOTSEL flashing — but that
requires manually holding BOOTSEL and remounting each device as `RPI-RP2`,
which is harder to do reliably through WSL's block-device passthrough. SWD
flashing via the debug probes is strongly recommended on WSL.

## Troubleshooting

- **Device vanishes from WSL after flashing**: the `--auto-attach` flag
  should reattach it within a few seconds; if not, run
  `usbipd attach --wsl --busid <busid>` again from PowerShell.
- **`ERROR: No Pico Debugger detected on USB (VID:PID 2e8a:000c)`**: the
  probes aren't attached to WSL yet — check `usbipd list` on Windows and
  re-attach.
- **Permission denied on `/dev/ttyACM*`**: make sure your WSL user is in the
  `dialout` group (see step 3) and reopen the terminal.
- **`openocd: address already in use`**: an old `openocd` process from a
  previous run is still bound to a port — `pkill openocd` and retry.
