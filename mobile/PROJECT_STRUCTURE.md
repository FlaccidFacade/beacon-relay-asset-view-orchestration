# BRAVO Mobile - Project Structure

This document provides an overview of the Android project structure for the BRAVO Mobile application.

## System Architecture

The BRAVO system uses a **relay-based architecture** for LoRa communication:

```
[GPS Collar/Dongle] --LoRa--> [ESP32 Relay] --USB/BLE--> [Mobile App]
   (Transmitter)              (LoRa Receiver)          (This Android App)
```

**Key Point**: The mobile phone does NOT receive LoRa radio signals directly. LoRa information is only accessible through:
1. **ESP32 Relay Device**: Receives LoRa transmissions and forwards data via USB or BLE
2. **API/Dashboard**: Future alternative for accessing telemetry data remotely

## Directory Structure

```
mobile/
├── app/                                    # Main application module
│   ├── build.gradle                        # App-level Gradle configuration
│   ├── proguard-rules.pro                  # ProGuard/R8 obfuscation rules
│   └── src/
│       └── main/
│           ├── AndroidManifest.xml         # App manifest with permissions
│           ├── assets/                     # Raw asset files
│           ├── java/com/bravo/mobile/
│           │   ├── activities/             # UI Activity classes
│           │   │   ├── MainActivity.java   # Main screen with connection controls
│           │   │   ├── MapActivity.java    # GPS map visualization screen
│           │   │   └── SettingsActivity.java # Settings screen (placeholder)
│           │   │
│           │   ├── services/               # Background services
│           │   │   ├── BLEConnectionService.java    # BLE connection handler
│           │   │   ├── LoRaReceiverService.java     # USB/LoRa data receiver
│           │   │   └── UsbBroadcastReceiver.java    # USB device event handler
│           │   │
│           │   ├── libs/                   # Core library utilities
│           │   │   ├── LoRaReceiver.java   # LoRa packet receiver
│           │   │   ├── TelemetryParser.java # Telemetry data parser
│           │   │   └── MapVisualization.java # Map display utility
│           │   │
│           │   ├── models/                 # Data models
│           │   │   ├── TelemetryData.java  # GPS telemetry data model
│           │   │   └── LoRaPacket.java     # Raw LoRa packet model
│           │   │
│           │   └── utils/                  # Utility classes
│           │       └── Constants.java      # App-wide constants
│           │
│           └── res/                        # Android resources
│               ├── layout/                 # UI layouts
│               │   ├── activity_main.xml   # Main activity layout
│               │   └── activity_map.xml    # Map activity layout
│               │
│               ├── values/                 # Resource values
│               │   ├── strings.xml         # String resources
│               │   ├── colors.xml          # Color definitions
│               │   └── themes.xml          # App themes
│               │
│               ├── mipmap-*/               # App icon resources
│               └── drawable/               # Drawable resources
│
├── gradle/                                 # Gradle wrapper files
│   └── wrapper/
│       └── gradle-wrapper.properties       # Gradle version configuration
│
├── build.gradle                            # Root Gradle configuration
├── settings.gradle                         # Gradle project settings
├── gradle.properties                       # Gradle properties
├── .gitignore                              # Git ignore rules
├── README.md                               # Main documentation
├── CONTRIBUTING.md                         # Contribution guidelines
└── PROJECT_STRUCTURE.md                    # This file
```

## Component Overview

### Activities (UI Layer)

**MainActivity**
- Entry point of the application
- Connection controls (BLE/USB)
- Real-time telemetry display
- Navigation to map view

**MapActivity**
- Interactive map with GPS tracking
- Offline map support via OSMDroid
- Path visualization
- Telemetry info overlay

**SettingsActivity**
- Placeholder for app configuration
- Future: Device selection, LoRa settings, preferences

### Services (Background Layer)

**BLEConnectionService**
- Foreground service for BLE connectivity
- Manages connection to ESP32 relay via Bluetooth
- Receives LoRa telemetry data forwarded from the relay
- Broadcasts data to UI

**LoRaReceiverService**
- Foreground service for USB connectivity
- USB serial communication with ESP32 relay
- Receives LoRa packets forwarded by the relay
- Telemetry data distribution

**UsbBroadcastReceiver**
- Handles USB device attach/detach events
- Automatic connection on device plug-in
- Permission management

### Libraries (Core Logic)

**LoRaReceiver**
- Receives LoRa packet data from ESP32 relay via USB/BLE
- Packet parsing and validation
- Event-based listener pattern
- Support for custom packet formats
- **Note**: Does NOT receive LoRa radio directly; processes forwarded data

**TelemetryParser**
- Converts LoRa packets to telemetry data
- Supports JSON and binary formats
- GPS coordinate validation
- Data sanity checks

**MapVisualization**
- Map display management
- Location marker updates
- Path tracking visualization
- Offline tile caching
- Zoom and pan controls

### Models (Data Layer)

**TelemetryData**
- GPS coordinates (lat/lon/alt)
- Speed and movement data
- Signal strength (RSSI/SNR) from LoRa transmission
- Battery level
- Timestamp and device ID
- **Source**: Data originates from GPS collars, transmitted via LoRa to ESP32 relay, then forwarded to phone

**LoRaPacket**
- Raw packet data forwarded from ESP32 relay
- Signal metadata from LoRa reception
- Frequency and spreading factor
- Reception timestamp

### Utilities

**Constants**
- BLE/USB configuration
- LoRa parameters
- Map defaults
- App-wide settings

## Key Features Implementation

### 1. Dual Connectivity (BLE/USB)
- **Files**: `BLEConnectionService.java`, `LoRaReceiverService.java`
- **Purpose**: Flexible connection options to ESP32 relay device
- **Dependencies**: Nordic BLE library, USB Serial for Android
- **Note**: Connects to relay, not directly to LoRa collars

### 2. LoRa Telemetry Reception
- **Files**: `LoRaReceiver.java`, `TelemetryParser.java`
- **Purpose**: Receive and parse GPS data forwarded from ESP32 relay
- **Formats**: JSON and binary packet formats supported
- **Data Path**: Collar/Dongle → LoRa → ESP32 Relay → USB/BLE → Phone

### 3. Map Visualization
- **Files**: `MapActivity.java`, `MapVisualization.java`
- **Purpose**: Display real-time GPS tracking on interactive map
- **Features**: Offline maps, path tracking, location markers
- **Dependencies**: OSMDroid

### 4. Background Services
- **Files**: `BLEConnectionService.java`, `LoRaReceiverService.java`
- **Purpose**: Continuous data reception even when app is minimized
- **Type**: Foreground services with notifications

## Configuration Files

### build.gradle (app-level)
- Android SDK versions (min: 26, target: 34)
- Dependencies (AndroidX, Maps, BLE, USB Serial, OSMDroid)
- Build types and signing configs
- View binding enabled

### AndroidManifest.xml
- Permissions (BLE, Location, USB, Internet)
- Activity declarations
- Service declarations
- Broadcast receiver registration
- Google Maps API key placeholder

### gradle.properties
- JVM arguments
- AndroidX migration flags
- Build optimization settings

## Resource Files

### Layouts
- `activity_main.xml`: Connection UI with buttons and telemetry display
- `activity_map.xml`: Map view with telemetry overlay

### Values
- `strings.xml`: All UI text and error messages (i18n ready)
- `colors.xml`: Material Design color palette
- `themes.xml`: App theme configuration

## Development Guidelines

1. **Adding New Features**
   - Models: Add to `models/` package
   - Business Logic: Add to `libs/` package
   - Background Tasks: Add to `services/` package
   - UI Screens: Add to `activities/` package

2. **Code Organization**
   - Follow existing package structure
   - Keep activities lightweight (delegate to services/libs)
   - Use services for background work
   - Use libs for reusable logic

3. **Testing**
   - Test on physical devices (BLE/USB require hardware)
   - Verify permissions on different Android versions
   - Test offline map functionality
   - Check memory usage in long-running services

## Dependencies

See `app/build.gradle` for complete list:
- AndroidX Core Libraries
- Material Design Components
- Google Play Services (Maps, Location)
- Nordic BLE Library
- USB Serial for Android
- Gson (JSON parsing)
- OSMDroid (Offline maps)

## Build Requirements

- Android Studio Arctic Fox or later
- JDK 8 or later
- Android SDK 26+ (API Level 26)
- Gradle 8.0
- Internet connection for initial dependency download

## Next Steps

1. Add actual app icons to `mipmap-*/` directories
2. Implement device selection dialog in MainActivity
3. Add settings UI in SettingsActivity
4. Test with real ESP32 hardware
5. Implement data persistence (SQLite/Room)
6. Add unit tests for parsers and utilities
7. Add instrumentation tests for activities

## Support

For questions about the project structure or implementation details, see:
- README.md for setup and usage
- CONTRIBUTING.md for development guidelines
- GitHub Issues for bug reports and feature requests
