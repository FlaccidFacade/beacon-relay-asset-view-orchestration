# Quick Start Guide - Multi-Device Setup

This guide walks you through setting up two or more B.R.A.V.O. devices that can communicate with each other while displaying GPS information.

## Overview

The updated firmware allows you to:
- Load the **same firmware** on multiple devices
- Each device records GPS info on its screen
- Devices communicate with each other via LoRa
- Scroll through multiple pages of data using the PRG button

## Hardware Requirements

For each device:
- Heltec WiFi LoRa 32 V3 board
- NEO-6M GPS module
- LoRa antenna (915 MHz for North America, 868 MHz for Europe)
- LiPo battery (optional, for portable operation)

## Step 1: Configure Device IDs

Before uploading to each device, edit `firmware/src/main.cpp` and set a unique ID:

**Device 1:**
```cpp
#define DEVICE_ID "BRAVO_001"
```

**Device 2:**
```cpp
#define DEVICE_ID "BRAVO_002"
```

**Device 3 (optional):**
```cpp
#define DEVICE_ID "BRAVO_003"
```

And so on for additional devices.

## Step 2: Upload Firmware

For each device:

1. Connect device via USB
2. Upload firmware:
   ```bash
   cd firmware
   pio run --target upload
   ```
3. Wait for upload to complete
4. Disconnect and move to next device

## Step 3: Test Individual Devices

Before testing communication, verify each device works independently:

1. Power on the device
2. Wait for initialization (GPS and LoRa should initialize successfully)
3. Press the PRG button to cycle through pages:
   - **GPS Page**: Should show "Searching..." initially
   - **Communication Page**: Should show 0 packets sent/received initially
   - **Device Info Page**: Should show correct device ID
   - **Combined Page**: Should show all info together

## Step 4: Test Device Communication

With both devices powered on and running:

1. Place devices close together (within 1-2 meters for initial test)
2. Navigate to the **Communication Page** on each device
3. Observe:
   - "Sent" counter incrementing every 3 seconds
   - "Rcvd" counter incrementing when receiving from other device
   - RSSI and SNR values appear after first reception

4. Navigate to the **Combined Page** to see GPS + communication status together

## Step 5: Test GPS Reception (Outdoor)

1. Take devices outside with clear view of sky
2. Wait 30-60 seconds for GPS fix (cold start)
3. Navigate to **GPS Page** to see:
   - Satellite count increasing
   - Status changes from "SRCH" to "LOCK"
   - Latitude and longitude coordinates appear
   - Altitude and speed data shown

4. Check **Communication Page** - transmitted packets should now include GPS coordinates

## Display Pages Reference

### Page 1: GPS Location
- Satellite count and lock status
- Latitude and longitude (6 decimal places)
- Altitude in meters
- Speed in km/h
- Status: Shows "LOCK" when GPS fix acquired, "SRCH" when searching

### Page 2: Communication
- Packets sent count
- Packets received count
- Last packet RSSI (signal strength in dBm)
- Last packet SNR (signal-to-noise ratio in dB)
- "Listening..." shown when no packets received yet

### Page 3: Device Info
- Device ID (BRAVO_001, BRAVO_002, etc.)
- Uptime in seconds
- GPS module status (Active/Search)
- LoRa module status (Active)

### Page 4: Combined View
- Device ID at top
- GPS coordinates or search status
- TX packet count
- RX packet count with RSSI
- Time since last transmission
- Time since last reception

## Troubleshooting

### Device Not Initializing
- Check USB connection
- Verify LoRa antenna is connected
- Check Serial monitor for error messages: `pio device monitor`

### No GPS Fix
- Ensure you're outdoors with clear sky view
- Allow 30-60 seconds for initial satellite acquisition
- Check GPS wiring: TX→GPIO33, RX→GPIO34, VCC→3.3V, GND→GND
- Verify GPS module has power (LED should blink)

### Devices Not Communicating
- Verify both devices initialized successfully
- Check antennas are connected on both devices
- Bring devices closer together (< 2 meters) for initial test
- Verify LoRa frequency matches your region (915 MHz or 868 MHz)
- Check Serial monitor on both devices for "Sending:" and "Received:" messages

### Display Not Showing Data
- Check Vext power (should be enabled in firmware)
- Verify I2C connections (SDA=17, SCL=18)
- Check Serial output for display initialization status

### Button Not Working
- PRG button is GPIO 0
- Press and release (don't hold)
- 300ms debounce delay between presses
- Check Serial monitor for "BUTTON PRESSED!" message

## Serial Monitor Output

Use the serial monitor to debug and observe device operation:

```bash
pio device monitor
```

You should see:
- Initialization messages for display, LoRa, and GPS
- "Sending: BRAVO_001_PKT_1..." every 3 seconds
- "Received [RSSI: -45 dBm]..." when packets arrive
- "BUTTON PRESSED!" when PRG button is pressed
- Page switching messages

## Range Testing

Once devices are working:

1. Start with devices close together
2. Gradually increase distance
3. Monitor RSSI values (more negative = weaker signal)
4. Typical ranges:
   - Line of sight: 500m - 2km
   - Urban environment: 100m - 500m
   - Indoor: 50m - 200m

## Power Consumption Tips

For battery-powered operation:
- GPS and LoRa radio are active continuously
- Display updates every 500ms
- Packet transmission every 3 seconds
- Consider adding sleep modes for extended battery life (future enhancement)

## Next Steps

- Deploy devices in your use case (pet collar, asset tracking, etc.)
- Monitor range and signal quality in different environments
- Customize packet transmission rate if needed (currently 3 seconds)
- Add additional sensors (IMU, temperature, etc.)
- Implement cloud connectivity (AWS IoT, MQTT, etc.)

## Support

For issues or questions:
- Check Serial monitor output for errors
- Review main README.md for detailed module documentation
- Open an issue on GitHub with Serial output and description
