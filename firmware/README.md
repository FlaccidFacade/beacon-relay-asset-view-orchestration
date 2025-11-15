# B.R.A.V.O. Firmware

**Bluetooth Radio Advanced Visual Orchestration**

ESP32 firmware for B.R.A.V.O. beacons and relays with LoRa communication, GPS tracking, IMU motion sensing, BLE configuration, and OTA updates.

## Project Overview

This firmware powers both the beacon devices and the relay devices in the B.R.A.V.O. system. It provides:

- **LoRa Communication**: Long-range radio communication between beacons and relays (915 MHz)
- **GPS Tracking**: Real-time location tracking using GPS module
- **IMU Sensing**: Motion and activity detection using MPU6050 accelerometer/gyroscope
- **BLE Configuration**: Bluetooth Low Energy interface for device configuration via mobile app
- **OTA Updates**: Over-the-air firmware updates via WiFi
- **JSON Telemetry**: Standardized data formatting for cloud and app integration

## Hardware Requirements

### Heltec WiFi LoRa 32 V3 Development Board

- ESP32-S3FN8 processor
- Built-in SX1262 LoRa radio (863-928 MHz)
- Built-in 0.96" OLED display
- USB-C connector for programming
- LiPo battery connector with charging circuit

**Official Documentation & Pinout:**

- [Heltec WiFi LoRa 32 V3 Pin Map](<https://resource.heltec.cn/download/WiFi_LoRa32_V3/HTIT-WB32LA(F)_V3.png>)
- [Heltec WiFi LoRa 32 V3 Docs](https://heltec.org/project/wifi-lora-32-v3/)
- [Hardware Reference](https://docs.heltec.org/en/nodeheltec_wifi_lora_32_V3/esp32/wifi_lora_32/hardware_update_log.html#v3)

### Peripherals

- **GPS Module**: NEO-6M Navigation Satellite Positioning Module
  - High sensitivity ceramic antenna
  - UART interface (TTL level)
  - Update rate: 1Hz (default), up to 5Hz
  - Compatible with Arduino, STM32, ESP32
  - Operating voltage: 3.3V-5V
  - Cold start: ~27s, Hot start: ~1s
  - Position accuracy: 2.5m (CEP)
- **IMU**: MPU6050 accelerometer/gyroscope (I2C)
- **Power**: 3.7V LiPo battery (recommended 1000mAh+)

### Pin Connections for Heltec WiFi LoRa 32 V3

#### LoRa Module (Built-in, pre-configured)

- SCK: GPIO 9
- MISO: GPIO 11
- MOSI: GPIO 10
- CS: GPIO 8
- RST: GPIO 12
- DIO0: GPIO 14

#### GPS Module NEO-6M (UART)

**Connections:**

- GPS VCC → 3.3V or 5V (both supported by NEO-6M)
- GPS GND → GND
- GPS TX → ESP32 GPIO 33 (RX pin)
- GPS RX → ESP32 GPIO 34 (TX pin, input only on ESP32-S3)

**Configuration:**

- Baud Rate: 9600 (default)
- Protocol: NMEA 0183
- Update Rate: 1Hz (configurable)

**Note**: The NEO-6M module typically comes with a ceramic patch antenna. Ensure the antenna has clear view of the sky for optimal GPS fix acquisition.

#### IMU (I2C)

- SDA: GPIO 41
- SCL: GPIO 42

## Software Requirements

- [PlatformIO](https://platformio.org/) (recommended) or Arduino IDE
- Python 3.x (for PlatformIO)
- Git

## Project Structure

```
firmware/
├── include/              # Header files
│   ├── LoRaComm.h       # LoRa communication interface
│   ├── GPS.h            # GPS module interface
│   ├── BLEConfig.h      # BLE configuration interface
│   ├── IMU.h            # IMU sensor interface
│   ├── OTA.h            # OTA update interface
│   └── Telemetry.h      # JSON telemetry formatting
├── src/                 # Implementation files
│   ├── main.cpp         # Main application entry point
│   ├── LoRaComm.cpp     # LoRa implementation
│   ├── GPS.cpp          # GPS implementation
│   ├── BLEConfig.cpp    # BLE implementation
│   ├── IMU.cpp          # IMU implementation
│   ├── OTA.cpp          # OTA implementation
│   └── Telemetry.cpp    # Telemetry implementation
├── platformio.ini       # PlatformIO configuration
├── .gitignore          # Git ignore rules
└── README.md           # This file
```

## Getting Started

### Installation

1. **Install PlatformIO**

   ```bash
   pip install platformio
   ```

2. **Clone the repository**

   ```bash
   git clone https://github.com/FlaccidFacade/beacon-relay-asset-view-orchestration.git
   cd beacon-relay-asset-view-orchestration/firmware
   ```

3. **Install dependencies**
   ```bash
   pio lib install
   ```

### Building

```bash
pio run
```

### Uploading to ESP32

1. Connect ESP32 via USB
2. Upload firmware:
   ```bash
   pio run --target upload
   ```

### Monitoring Serial Output

```bash
pio device monitor
```

## Configuration

### Device Type

Edit `src/main.cpp` to set device type:

```cpp
#define DEVICE_TYPE  DEVICE_TYPE_BEACON  // Set to DEVICE_TYPE_BEACON or DEVICE_TYPE_RELAY
```

### Device ID

Set unique device identifier in `src/main.cpp`:

```cpp
#define DEVICE_ID  "BRAVO_001"  // Change for each device
```

### LoRa Frequency

Adjust for your region in `include/LoRaComm.h`:

```cpp
#define LORA_BAND   915E6  // 915 MHz (North America)
// or
#define LORA_BAND   868E6  // 868 MHz (Europe)
```

### WiFi OTA (Optional)

Enable OTA updates in `src/main.cpp`:

```cpp
// Uncomment these lines in setup():
if (ota.connectWiFi("YourSSID", "YourPassword")) {
    ota.begin(DEVICE_ID);
}

// Uncomment in loop():
ota.handle();
```

## Module Documentation

### LoRaComm Module

Handles long-range radio communication using LoRa modulation.

**Key Functions:**

- `bool begin()` - Initialize LoRa module
- `bool sendMessage(const String& message)` - Send text message
- `bool sendData(const uint8_t* data, size_t length)` - Send binary data
- `String receiveMessage()` - Receive text message
- `int getRSSI()` - Get signal strength of last packet
- `float getSNR()` - Get signal-to-noise ratio

**Example:**

```cpp
LoRaComm lora;
lora.begin();

// Send message
lora.sendMessage("Hello from beacon!");

// Receive message
if (lora.available()) {
    String msg = lora.receiveMessage();
    int rssi = lora.getRSSI();
}
```

### GPS Module

Manages GPS data acquisition and parsing using TinyGPS++ library.

**Key Functions:**

- `bool begin()` - Initialize GPS module
- `void update()` - Process incoming GPS data (call in loop)
- `bool getLocation(double& lat, double& lon)` - Get current coordinates
- `double getAltitude()` - Get altitude in meters
- `float getSpeed()` - Get speed in km/h
- `bool hasFix()` - Check if GPS has valid fix
- `GPSData getData()` - Get complete GPS data structure

**Example:**

```cpp
GPS gps;
gps.begin();

void loop() {
    gps.update();

    if (gps.hasFix()) {
        double lat, lon;
        gps.getLocation(lat, lon);
        Serial.printf("Location: %.6f, %.6f\n", lat, lon);
    }
}
```

### BLEConfig Module

Provides Bluetooth Low Energy interface for device configuration.

**Key Functions:**

- `bool begin(const char* deviceName)` - Initialize BLE with device name
- `void update()` - Update BLE stack
- `bool isConnected()` - Check if client connected
- `void sendStatus(const String& status)` - Send status to connected client
- `BLEConfigData getConfig()` - Get current configuration
- `void setConfig(const BLEConfigData& config)` - Update configuration

**Example:**

```cpp
BLEConfig ble;
ble.begin("BRAVO_BEACON_001");

void loop() {
    ble.update();

    if (ble.isConnected()) {
        ble.sendStatus("Battery: 85%");
    }
}
```

### IMU Module

Reads accelerometer and gyroscope data from MPU6050 sensor.

**Key Functions:**

- `bool begin()` - Initialize IMU sensor
- `bool readSensor()` - Read current sensor values
- `void getAcceleration(float& x, float& y, float& z)` - Get acceleration
- `void getGyro(float& x, float& y, float& z)` - Get gyroscope data
- `float getTemperature()` - Get sensor temperature
- `uint8_t getActivityLevel()` - Calculate activity level (0-100)
- `bool isInMotion(float threshold)` - Detect motion

**Example:**

```cpp
IMU imu;
imu.begin();

void loop() {
    if (imu.readSensor()) {
        uint8_t activity = imu.getActivityLevel();

        if (imu.isInMotion(1.0)) {
            Serial.println("Motion detected!");
        }
    }
}
```

### OTA Module

Enables over-the-air firmware updates via WiFi.

**Key Functions:**

- `bool begin(const char* hostname, const char* password)` - Initialize OTA
- `void handle()` - Process OTA requests (call in loop)
- `bool connectWiFi(const char* ssid, const char* password)` - Connect to WiFi
- `bool isWiFiConnected()` - Check WiFi status
- `void enable()` / `void disable()` - Control OTA availability

**Example:**

```cpp
OTA ota;

void setup() {
    if (ota.connectWiFi("MyNetwork", "MyPassword")) {
        ota.begin("BRAVO_BEACON_001", "secure_password");
    }
}

void loop() {
    ota.handle();  // Process OTA updates
}
```

### Telemetry Module

Formats sensor data into JSON for transmission and cloud integration.

**Key Functions:**

- `String createFullTelemetry(...)` - Create complete telemetry packet
- `String createGPSTelemetry(...)` - Create GPS-only packet
- `String createIMUTelemetry(...)` - Create IMU-only packet
- `String createStatusTelemetry(...)` - Create status packet
- `String createAlertTelemetry(...)` - Create alert packet
- `bool parseTelemetry(const String& json)` - Parse incoming telemetry

**Example:**

```cpp
Telemetry telemetry;

GPSData gpsData = gps.getData();
IMUData imuData = imu.getData();

String json = telemetry.createFullTelemetry(
    gpsData, imuData, "BRAVO_001", 85
);

lora.sendMessage(json);
```

## Telemetry Format

### Full Telemetry Packet

```json
{
  "device_id": "BRAVO_001",
  "timestamp": 123456,
  "type": "full",
  "battery": 85,
  "gps": {
    "valid": true,
    "lat": 40.7128,
    "lon": -74.006,
    "alt": 10.5,
    "speed": 5.2,
    "course": 180.0,
    "satellites": 8
  },
  "imu": {
    "accel": { "x": 0.1, "y": 0.2, "z": 9.8 },
    "gyro": { "x": 0.0, "y": 0.0, "z": 0.0 },
    "temp": 25.5
  }
}
```

## Development

### Adding New Features

1. Create header file in `include/`
2. Create implementation in `src/`
3. Include in `main.cpp` and integrate
4. Update this README with documentation

### Testing

1. **Serial Monitor**: Monitor debug output via USB

   ```bash
   pio device monitor
   ```

2. **LoRa Range Test**: Deploy two devices and test communication range

3. **GPS Accuracy**: Test GPS fix time and accuracy in different environments

4. **BLE Connection**: Test BLE connectivity with mobile app

### Debugging

Enable detailed logging by increasing debug level in `platformio.ini`:

```ini
build_flags =
    -D CORE_DEBUG_LEVEL=5  ; 0=None, 5=Verbose
```

## Troubleshooting

### LoRa Not Working

- Check wiring connections
- Verify frequency band matches your region
- Ensure antennas are connected
- Check SPI communication

### GPS Not Getting Fix (NEO-6M Module)

- **Location**: Use outdoors with clear view of the sky. NEO-6M requires line-of-sight to satellites
- **Cold Start Time**: Allow 27+ seconds for initial satellite acquisition
- **Hot Start Time**: Should acquire fix within 1 second if recently powered on same location
- **Antenna**: Ensure ceramic patch antenna is properly connected and positioned horizontally
- **UART Connections**:
  - Verify GPS TX connects to ESP32 GPIO 33
  - Verify GPS RX connects to ESP32 GPIO 34
  - Check GND connection
- **Baud Rate**: Confirm 9600 baud (NEO-6M default)
- **Power**: NEO-6M requires stable 3.3V-5V supply (typically draws 45mA when acquiring)
- **LED Indicator**: Most NEO-6M modules have LED that blinks when satellites are being tracked
- **Serial Monitor**: Check for NMEA sentences being received (use `pio device monitor`)
- **Interference**: Keep away from LoRa antenna and WiFi when testing

### IMU Not Responding

- Check I2C connections (SDA/SCL)
- Verify I2C address (default 0x68)
- Check power supply voltage

### BLE Not Advertising

- Restart device
- Check for BLE conflicts with WiFi
- Verify device name is set correctly

### OTA Upload Fails

- Check WiFi credentials
- Ensure device is on same network
- Verify OTA password
- Check firewall settings

## Security Considerations

### Important Security Notes

⚠️ **Before deploying to production:**

1. **Change OTA Password**: Update `OTA_PASSWORD` in `include/OTA.h`
2. **Secure BLE**: Implement pairing and encryption for BLE
3. **Encrypt LoRa**: Add encryption for sensitive data transmission
4. **WiFi Credentials**: Store WiFi credentials securely (not hardcoded)

## License

[Specify your license here]

## Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Submit a pull request

## Support

For issues and questions:

- Open an issue in the [main repository](https://github.com/FlaccidFacade/beacon-relay-asset-view-orchestration/issues)
- Check existing documentation
- Review PlatformIO and Arduino ESP32 documentation

## Acknowledgments

- ESP32 Arduino Core
- PlatformIO
- LoRa library by Sandeep Mistry
- TinyGPS++ by Mikal Hart
- Adafruit sensor libraries
- ArduinoJson by Benoit Blanchon
- NimBLE-Arduino

## Roadmap

- [ ] Add encryption for LoRa communications
- [ ] Implement mesh networking for multi-beacon systems
- [ ] Add geofencing and alert zones
- [ ] Implement power-saving modes
- [ ] Add data logging to SD card
- [ ] Create mobile app for configuration
- [ ] Add cloud integration (AWS IoT, Azure IoT Hub)
- [ ] Implement FOTA (Firmware Over The Air) via LoRa

---

**Version**: 1.0.0  
**Last Updated**: 2025  
**Maintainers**: B.R.A.V.O. Team
