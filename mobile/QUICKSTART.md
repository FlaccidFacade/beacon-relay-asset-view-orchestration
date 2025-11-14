# BRAVO Mobile - Quick Start Guide

Get up and running with the BRAVO Mobile Android app in minutes!

## Understanding the System

**Important**: The BRAVO mobile app connects to an **ESP32 relay device**, not directly to LoRa radio. 

```
GPS Collar → LoRa Radio → ESP32 Relay → USB/BLE → Your Phone
```

The phone receives telemetry data that has been:
1. Transmitted via LoRa from GPS collars/dongles
2. Received by an ESP32 relay device with LoRa module
3. Forwarded to the phone via USB or BLE connection

## Prerequisites Checklist

- [ ] Android Studio installed (Arctic Fox or later)
- [ ] Android device or emulator (API 26+)
- [ ] USB cable for device connection
- [ ] Internet connection for initial setup

## 5-Minute Setup

### Step 1: Clone and Open Project (1 min)

```bash
git clone https://github.com/beacon-relay-asset-view-orchestration/mobile.git
cd mobile
```

Open in Android Studio:
- File → Open → Select `mobile` directory

### Step 2: Sync and Build (2 min)

Wait for Gradle sync to complete automatically, then:

```bash
./gradlew build
```

Or in Android Studio: Build → Make Project (Ctrl+F9)

### Step 3: Run the App (2 min)

Connect your Android device via USB, then:

```bash
./gradlew installDebug
```

Or in Android Studio: Run → Run 'app' (Shift+F10)

That's it! The app should launch on your device.

## First Launch

When you first launch the app:

1. **Grant Permissions**: Allow Bluetooth and Location permissions when prompted
2. **Main Screen**: You'll see connection options and telemetry display area
3. **Test Connection**: Try "Connect via Bluetooth" or "Connect via USB"
4. **View Map**: Tap "View GPS Map" to see the map interface

## Connecting to ESP32

### Via Bluetooth (BLE)

1. Ensure ESP32 relay is powered on and advertising
2. Tap "Connect via Bluetooth" button
3. Select your ESP32 relay device from the list
4. Wait for "Connected" status

**Note**: The ESP32 must be configured as a LoRa relay and within BLE range.

### Via USB

1. Connect ESP32 relay to Android device using OTG cable
2. Launch the app
3. Tap "Connect via USB" button
4. Grant USB permission when prompted
5. Connection should establish automatically

**Note**: The ESP32 relay must be receiving LoRa transmissions from GPS collars for data to appear.

## Understanding the UI

### Main Screen

- **Status Indicator**: Shows connection state (Disconnected/Connecting/Connected/Receiving)
- **Connection Buttons**: Choose BLE or USB connection method
- **View Map Button**: Opens GPS tracking map
- **Telemetry Panel**: Displays received GPS and sensor data

### Map Screen

- **Map View**: Interactive OpenStreetMap display
- **Location Marker**: Current position from telemetry
- **Blue Path**: Trail showing device movement
- **Info Panel**: Real-time coordinates, altitude, speed
- **Zoom Controls**: Pinch to zoom or use on-screen buttons

## Testing Without Hardware

If you don't have ESP32 hardware yet:

1. The app will still build and run
2. Connection attempts will show "No device found"
3. Map can be viewed and navigated
4. Mock data can be added for testing (see MapActivity.updateMapWithMockData)

**To test the full system**, you need:
- ESP32 relay device with LoRa receiver module
- GPS collar/dongle with LoRa transmitter (or use for testing)

## Troubleshooting Quick Fixes

### "Bluetooth not supported" error
- Check device has BLE capability
- Try on a different device

### "Permission denied" error
- Go to Settings → Apps → BRAVO Mobile → Permissions
- Enable Location and Bluetooth permissions

### "No device found" for USB
- Verify OTG cable is working
- Try a different USB cable
- Check ESP32 is powered and running

### Map not loading
- Ensure internet connection for first tile download
- Check Storage permission is granted
- Zoom out and pan to refresh

### Gradle sync fails
- Check internet connection
- File → Invalidate Caches / Restart
- Try: `./gradlew clean build`

## Next Steps

1. **Read Full Documentation**: See [README.md](README.md) for detailed information
2. **Configure API Keys**: Set up Google Maps API if needed
3. **Connect Real Hardware**: Test with ESP32 + LoRa + GPS setup
4. **Explore Code**: Check [PROJECT_STRUCTURE.md](PROJECT_STRUCTURE.md) to understand architecture
5. **Contribute**: See [CONTRIBUTING.md](CONTRIBUTING.md) for development guidelines

## Common Commands

```bash
# Build the project
./gradlew build

# Install debug version
./gradlew installDebug

# Run tests
./gradlew test

# Clean build
./gradlew clean

# Check for updates
./gradlew dependencies
```

## Development Tips

- Use Android Studio's Logcat to view debug output
- Filter logs by "BRAVO" to see app-specific messages
- Enable USB debugging on your Android device
- Keep device plugged in during development

## Getting Help

- **Issues**: https://github.com/beacon-relay-asset-view-orchestration/mobile/issues
- **Documentation**: README.md, PROJECT_STRUCTURE.md
- **Code Examples**: Check activity implementations for usage patterns

## What's Included

This is a complete Android project with:
- ✅ Activities for UI (MainActivity, MapActivity)
- ✅ Services for BLE and USB connections to ESP32 relay
- ✅ LoRa packet parser (for forwarded data)
- ✅ Telemetry data models
- ✅ Map visualization with offline support
- ✅ Comprehensive documentation

## What's Not Included

You'll need to provide:
- ESP32 relay device with LoRa receiver module (SX1276/SX1278)
- GPS collar/dongle with LoRa transmitter
- Firmware for ESP32 relay to receive LoRa and forward via BLE/USB
- Google Maps API key (optional, OSMDroid works without it)
- App icons (using Android defaults currently)

**Note**: The phone cannot receive LoRa directly - it must connect to an ESP32 relay.

## Ready to Dive Deeper?

Check out these files:
- [README.md](README.md) - Complete documentation
- [PROJECT_STRUCTURE.md](PROJECT_STRUCTURE.md) - Architecture overview
- [CONTRIBUTING.md](CONTRIBUTING.md) - Development guidelines

Happy coding! 🚀
