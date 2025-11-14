# B.R.A.V.O. Firmware

**Bluetooth Radio Advanced Visual Orchestration**

ESP32 firmware for B.R.A.V.O. collars and dongle with LoRa communication, GPS tracking, IMU motion sensing, BLE configuration, and OTA updates.

## Project Overview

This firmware powers both the collar devices and the dongle in the B.R.A.V.O. system. It provides:

- **LoRa Communication**: Long-range radio communication between collars and dongle (915 MHz)
- **GPS Tracking**: Real-time location tracking using GPS module
- **IMU Sensing**: Motion and activity detection using MPU6050 accelerometer/gyroscope
- **BLE Configuration**: Bluetooth Low Energy interface for device configuration via mobile app
- **OTA Updates**: Over-the-air firmware updates via WiFi
- **JSON Telemetry**: Standardized data formatting for cloud and app integration

## Hardware Requirements

### ESP32 Development Board
- ESP32-DevKitC or compatible board
- USB cable for programming

### Peripherals
- **LoRa Module**: SX1276/SX1278 (915 MHz for North America, 868 MHz for Europe)
- **GPS Module**: NEO-6M or compatible UART GPS
- **IMU**: MPU6050 accelerometer/gyroscope (I2C)
- **Power**: LiPo battery with charging circuit (recommended 3.7V 1000mAh+)

### Pin Connections

#### LoRa Module (SPI)
- SCK: GPIO 5
- MISO: GPIO 19
- MOSI: GPIO 27
- CS: GPIO 18
- RST: GPIO 14
- DIO0: GPIO 26

#### GPS Module (UART)
- RX: GPIO 16
- TX: GPIO 17
- Baud: 9600

#### IMU (I2C)
- SDA: GPIO 21
- SCL: GPIO 22

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
   git clone https://github.com/BT-Radio-Advanced-Visual-Orchestration/firmware.git
   cd firmware
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
#define DEVICE_TYPE_COLLAR  true  // Set to false for dongle
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
lora.sendMessage("Hello from collar!");

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
ble.begin("BRAVO_COLLAR_001");

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
        ota.begin("BRAVO_COLLAR_001", "secure_password");
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
    "lon": -74.0060,
    "alt": 10.5,
    "speed": 5.2,
    "course": 180.0,
    "satellites": 8
  },
  "imu": {
    "accel": {"x": 0.1, "y": 0.2, "z": 9.8},
    "gyro": {"x": 0.0, "y": 0.0, "z": 0.0},
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

### GPS Not Getting Fix
- Use outdoors with clear sky view
- Allow 1-2 minutes for cold start
- Check UART connections
- Verify baud rate (9600)

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
- Open an issue on GitHub
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
- [ ] Implement mesh networking for multi-collar systems
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
