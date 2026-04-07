# Quick Start Guide - Multi-Device Setup (Pico W)

This guide walks you through setting up two Raspberry Pi Pico W devices that share GPS location over LoRa.

## Overview

Each B.R.A.V.O. device:

- Reads GPS coordinates from a NEO-7m module
- Broadcasts its GPS location over LoRa (RYLR896) every 5 seconds
- Displays live GPS and radio stats on a 128×64 SSD1306 OLED

## Hardware Requirements

For **each** device you need:

| Component           | Notes                     |
| ------------------- | ------------------------- |
| Raspberry Pi Pico W | RP2040 + CYW43439 Wi-Fi   |
| REYAX RYLR896       | UART LoRa module, 915 MHz |
| u-blox NEO-7m GPS   | 9600 baud NMEA, 1 Hz PPS  |
| SSD1306 OLED 128×64 | I2C, address 0x3C         |
| Push-button         | Momentary, normally open  |
| LoRa antenna        | 915 MHz, SMA or U.FL      |
| 5V USB power supply | Powers VSYS, ≥500 mA      |
| Jumper wires        |                           |

**Programmer**: Raspberry Pi 4B connected to Pico W via USB micro-B or USB-C.

## Wiring (per device)

All peripherals run at **3.3V from Pico W Pin 36**.  
Power the Pico W itself via **VSYS (Pin 39/40) with 5V**.

| Signal         | Pico W GPIO | Physical Pin |
| -------------- | ----------- | ------------ |
| RYLR896 RXD ←  | GP0 (TX)    | 1            |
| RYLR896 TXD →  | GP1 (RX)    | 2            |
| OLED SDA       | GP4         | 6            |
| OLED SCL       | GP5         | 7            |
| GPS RXD ←      | GP8 (TX)    | 11           |
| GPS TXD →      | GP9 (RX)    | 12           |
| RYLR896 NRESET | GP14        | 19           |
| GPS PPS        | GP15        | 20           |
| Button → GND   | GP16        | 21           |
| 3.3V supply    | 3V3 OUT     | 36           |
| GND            | GND         | 38           |
| 5V in          | VSYS        | 39           |

## Step 1: Configure Device Addresses

The two units exchange GPS payloads using LoRa addresses. Edit `firmware/platformio.ini` **before uploading to each unit**:

**Unit 1 (Beacon):**

```ini
build_flags =
    -D DEVICE_ADDRESS=1
    -D TARGET_ADDRESS=2
```

**Unit 2 (Relay):**

```ini
build_flags =
    -D DEVICE_ADDRESS=2
    -D TARGET_ADDRESS=1
```

## Step 2: Upload Firmware from Raspberry Pi 4B

For **each** device:

1. Connect the Pico W to the Raspberry Pi 4B via USB **while holding BOOTSEL**.
2. Release the BOOTSEL button — the Pico W appears as a mass-storage device (`RPI-RP2`).
3. Run the upload helper:

   ```bash
   cd firmware
   chmod +x upload.sh
   ./upload.sh
   ```

   The script detects the Pico W, builds the firmware, and flashes it automatically.

4. After flashing, the Pico W reboots and starts running. Unplug USB.
5. Change `DEVICE_ADDRESS` / `TARGET_ADDRESS` in `platformio.ini` for the second unit and repeat.

## Step 3: Test Individual Devices

Power on each device and check the OLED:

1. **GPS screen** (default): "Fix: NO", satellite count climbs as the device acquires signal.
2. Press the **button** (GP16) to switch to the **Radio screen**.
3. Serial monitor (optional):

   ```bash
   pio device monitor --environment rpicow
   # or
   minicom -b 115200 -D /dev/ttyACM0
   ```

   You should see:

   ```
   [BRAVO] Pico W starting...
   [Display] SSD1306 OK
   [GPS] NEO-7m on UART1 ready
   [LoRa] RYLR896 ready
   [BRAVO] Setup complete
   ```

## Step 4: Test Device Communication

With both devices powered on:

1. Place devices within a few metres of each other for the initial test.
2. Navigate to the **Radio screen** on each (press button).
3. Observe:
   - **TX** counter incrementing every 5 seconds.
   - **RX** counter incrementing as each device receives the other's GPS payload.
   - **RSSI** and **SNR** values appear after the first reception.
4. Serial monitor shows:

   ```
   [LoRa] TX → 1|40.71280|-74.00600|8
   [LoRa] RX from 2: 2|40.71285|-74.00598|8 RSSI=-45 SNR=9.5
   ```

## Step 5: Test GPS Outdoors

1. Take both devices outside with a clear view of the sky.
2. Allow **30–60 seconds** for cold-start GPS acquisition.
3. The GPS screen shows:
   - `Fix: YES` once a valid position is acquired.
   - Live latitude, longitude, and altitude values.
4. GPS coordinates are automatically included in every LoRa heartbeat.

## Display Pages Reference

### GPS Screen (default)

```
-- GPS --
Fix: YES
Sat: 9
Lat: 40.71280
Lon: -74.00600
Alt: 12.3m
```

### Radio Screen (press button to switch)

```
-- RADIO --
TX: 12
RX: 11
RSSI: -52
SNR:  8.5
Msg: 2|40.71285
```

## Troubleshooting

### Pico W Not Detected on Raspberry Pi 4B

- Use a data-capable USB cable (not charge-only).
- Run `lsusb` — in BOOTSEL mode you should see `2e8a:0003 Raspberry Pi RP2 Boot`.
- Add your user to `dialout` for serial access:
  ```bash
  sudo usermod -aG dialout $USER
  # log out and back in
  ```

### No GPS Fix

- Go outdoors — GPS won't work through walls.
- Allow 27+ seconds for cold-start satellite acquisition.
- Check wiring: GP8 → NEO-7m RXD, GP9 ← NEO-7m TXD.

### Devices Not Communicating

- Confirm the LoRa antennas are attached.
- Verify one unit has `DEVICE_ADDRESS=1, TARGET_ADDRESS=2` and the other has the reverse.
- Ensure both units use the same `LORA_NETWORK_ID` and `LORA_FREQ_HZ` (see `PinConfig.h`).
- Check the serial monitor on both devices for `[LoRa] No response from RYLR896`.

### Display Not Showing

- Verify wiring: GP4 (SDA), GP5 (SCL), 3.3V (Pin 36), GND.
- Confirm OLED I2C address is `0x3C` (`OLED_I2C_ADDR` in `PinConfig.h`).

## Range Expectations

| Environment                | Typical Range |
| -------------------------- | ------------- |
| Line-of-sight (open field) | 500 m – 2 km  |
| Urban / suburban           | 100 m – 500 m |
| Indoor                     | 50 m – 200 m  |

Monitor RSSI on the Radio screen — values closer to 0 dBm indicate a stronger link.

## Next Steps

- Increase LoRa transmit power in `PinConfig.h` (`AT+CRFOP` parameter).
- Adjust `HEARTBEAT_INTERVAL` in `src/main.cpp` for faster or slower GPS updates.
- Enable the Pico W's Wi-Fi to relay GPS data to a cloud backend (MQTT / HTTP).
- Add additional devices by assigning unique `DEVICE_ADDRESS` values (0–65535).
