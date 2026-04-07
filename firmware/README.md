# B.R.A.V.O. Firmware

**Bluetooth Radio Advanced Visual Orchestration**

Raspberry Pi Pico W firmware for B.R.A.V.O. beacons and relays with LoRa communication and GPS tracking.

## Project Overview

This firmware powers both the beacon devices and the relay devices in the B.R.A.V.O. system. It provides:

- **LoRa Communication**: Long-range radio communication between beacons and relays (915 MHz) via REYAX RYLR896 over UART AT commands
- **GPS Tracking**: Real-time location tracking using GPS NEO-7m module
- **OLED Display**: Live GPS and radio stats on a 128×64 SSD1306 display
- **GPS over LoRa**: Each device transmits its GPS coordinates to the other unit every 5 seconds

## Hardware Requirements

### Raspberry Pi Pico W

- RP2040 dual-core ARM Cortex-M0+ processor
- Built-in 2.4 GHz Wi-Fi (CYW43439)
- 2 MB flash, 264 KB SRAM
- 26 GPIO pins
- 2× UART, 2× SPI, 2× I2C
- USB-C / micro-USB for programming from Raspberry Pi 4B

**Official Documentation & Pinout:**

- [Pico W Datasheet](https://datasheets.raspberrypi.com/picow/pico-w-datasheet.pdf)
- [Pico W Pinout](https://datasheets.raspberrypi.com/picow/PicoW-A4-Pinout.pdf)
- [Getting Started with Pico](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf)

### Peripherals

- **LoRa Module**: REYAX RYLR896 (UART AT commands, 915 MHz)
  - UART interface (TTL 3.3V)
  - Up to 240-byte payload
  - Addressable device-to-device messaging
- **GPS Module**: u-blox NEO-7m (NMEA UART, 9600 baud)
  - 1 Hz position update with PPS output
  - Position accuracy: 2.5 m (CEP)
  - Cold start: ~27 s, Hot start: ~1 s
- **OLED Display**: SSD1306 128×64 (I2C, addr 0x3C)
- **Push-button**: Momentary switch on GP16 (cycles display screens)

### Pin Connections (Pico W)

All peripherals run at **3.3V** from Pin 36 (3V3 OUT).
Power the Pico W with 5V on **VSYS** (Pin 39 or 40).

| Signal | Pico W Pin | GPIO | Notes |
|--------|-----------|------|-------|
| RYLR896 RXD | 1 | GP0 | UART0 TX |
| RYLR896 TXD | 2 | GP1 | UART0 RX |
| OLED SDA | 6 | GP4 | I2C0 SDA |
| OLED SCL | 7 | GP5 | I2C0 SCL |
| GPS RXD | 11 | GP8 | UART1 TX |
| GPS TXD | 12 | GP9 | UART1 RX |
| RYLR896 NRESET | 19 | GP14 | Active LOW |
| GPS PPS | 20 | GP15 | 1 Hz rising edge |
| Button | 21 | GP16 | INPUT_PULLUP → GND |
| 3.3V OUT | 36 | — | Powers OLED, GPS, LoRa |
| VSYS (5V in) | 39/40 | — | External 5V supply |
| GND | 3/8/13/18/23/28/33/38 | — | Any GND pin |

## Software Requirements

- [PlatformIO](https://platformio.org/) (recommended) or Arduino IDE
- Python 3.x (for PlatformIO)
- `picotool` (for flashing from Raspberry Pi 4B — installed automatically by `upload.sh`)

## Project Structure

```
firmware/
├── include/              # Header files
│   ├── PinConfig.h      # Pico W GPIO pin assignments and LoRa/GPS constants
│   ├── LoRaComm.h       # RYLR896 AT-command LoRa interface
│   ├── GPS.h            # NEO-7m GPS module interface
│   └── Display.h        # SSD1306 OLED display interface
├── src/                 # Implementation files
│   ├── main.cpp         # Main application entry point
│   ├── LoRaComm.cpp     # RYLR896 UART AT driver
│   ├── GPS.cpp          # GPS NMEA parser (TinyGPS++)
│   └── Display.cpp      # OLED rendering
├── platformio.ini       # PlatformIO configuration (rpicow target)
├── upload.sh            # One-command flash helper for Raspberry Pi 4B
└── README.md            # This file
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
   pio pkg install
   ```

### Building

```bash
pio run --environment rpicow
```

### Uploading to Pico W from Raspberry Pi 4B

**Option A — Automated upload script (recommended):**

```bash
# 1. Hold BOOTSEL on the Pico W while plugging USB into the Raspberry Pi 4B
# 2. Release BOOTSEL
# 3. Run:
chmod +x upload.sh
./upload.sh
```

**Option B — Manual drag-and-drop:**

1. Hold the **BOOTSEL** button on the Pico W
2. Plug USB into the Raspberry Pi 4B
3. Release BOOTSEL — the Pico W mounts as `RPI-RP2`
4. Copy the generated UF2:

   ```bash
   cp .pio/build/rpicow/firmware.uf2 /media/pi/RPI-RP2/
   ```

**Option C — PlatformIO direct upload:**

```bash
pio run --environment rpicow --target upload
```

### Monitoring Serial Output

```bash
pio device monitor --environment rpicow
# or
minicom -b 115200 -D /dev/ttyACM0
```

## Operation

### Device Roles

Each device is **both beacon and relay simultaneously**:

1. **GPS**: Continuously reads NMEA sentences from the NEO-7m and updates the `GPSData` struct.
2. **LoRa TX (beacon)**: Every `HEARTBEAT_INTERVAL` (5 s), sends a compact GPS payload to `TARGET_ADDRESS`:
   ```
   <DEVICE_ADDRESS>|<lat>|<lon>|<satellites>
   ```
3. **LoRa RX (relay)**: Non-blocking poll for incoming `+RCV=` packets from the other unit.
4. **OLED**: Two-screen display cycled by the push-button:
   - **GPS screen** — fix status, satellites, lat/lon, altitude
   - **Radio screen** — TX count, RX count, RSSI, SNR, last message

### Two-Device Setup

| | Unit 1 (Beacon) | Unit 2 (Relay) |
|-|-----------------|----------------|
| `DEVICE_ADDRESS` | 1 | 2 |
| `TARGET_ADDRESS` | 2 | 1 |
| Build flag | `-D DEVICE_ADDRESS=1 -D TARGET_ADDRESS=2` | `-D DEVICE_ADDRESS=2 -D TARGET_ADDRESS=1` |

Edit `build_flags` in `platformio.ini` before uploading to each unit, or pass them on the CLI:

```bash
# Unit 1
pio run --environment rpicow --target upload \
    -e "build_flags=-D DEVICE_ADDRESS=1 -D TARGET_ADDRESS=2"

# Unit 2
pio run --environment rpicow --target upload \
    -e "build_flags=-D DEVICE_ADDRESS=2 -D TARGET_ADDRESS=1"
```

### Display Navigation

- **Button (GP16)**: Short press cycles GPS screen ↔ Radio screen.
- Display refreshes every 1 s automatically.

## Configuration

### LoRa Frequency

Adjust for your region in `include/PinConfig.h`:

```cpp
#define LORA_FREQ_HZ    915000000   // 915 MHz (North America)
// or
#define LORA_FREQ_HZ    868000000   // 868 MHz (Europe)
```

### Heartbeat Interval

Change how often GPS location is broadcast in `src/main.cpp`:

```cpp
static const uint32_t HEARTBEAT_INTERVAL = 5000;  // ms between LoRa TX
```

### RF Parameters

Spread-factor, bandwidth, coding rate, and preamble length are set in `include/PinConfig.h`:

```cpp
#define LORA_PARAM_SF   9    // Spreading Factor (7–12; higher = longer range, slower)
#define LORA_PARAM_BW   7    // Bandwidth index per RYLR896 datasheet
                             // 0=7.8kHz,1=10.4kHz,2=15.6kHz,3=20.8kHz,4=31.25kHz,
                             // 5=41.7kHz,6=62.5kHz,7=125kHz,8=250kHz,9=500kHz
#define LORA_PARAM_CR   1    // Coding Rate (1=4/5, 2=4/6, 3=4/7, 4=4/8)
#define LORA_PARAM_PP   12   // Preamble length
```

## Module Documentation

### LoRaComm Module

Drives the REYAX RYLR896 via UART0 AT commands.

**Key Functions:**

- `bool begin(uint16_t deviceAddress)` — Reset and configure the RYLR896
- `bool sendMessage(uint16_t targetAddress, const String& message)` — Send GPS payload
- `bool receive(LoRaPacket& out)` — Non-blocking poll for incoming packet
- `int getLastRSSI()` / `float getLastSNR()` — Signal quality of last RX
- `bool isReady()` — Returns true after successful `begin()`

### GPS Module

Parses NMEA sentences from the NEO-7m on UART1 via TinyGPS++.

**Key Functions:**

- `bool begin()` — Configure Serial2 (UART1) and PPS pin
- `void update()` — Feed characters from Serial2 into TinyGPS++
- `GPSData getData()` — Snapshot of current fix: lat, lon, alt, speed, satellites
- `bool hasFix()` — True if location data is valid
- `void onPPS()` — ISR callback for 1 Hz PPS pulse

### Display Module

Renders GPS and radio information on the SSD1306 128×64 OLED over I2C0.

**Key Functions:**

- `bool begin()` — Configure Wire (I2C0) and initialise SSD1306
- `void showGPSScreen(const GPSData&)` — GPS fix screen
- `void showRadioScreen(txCount, rxCount, rssi, snr, lastMsg)` — Radio stats screen
- `void showInitStatus(module, success)` — Boot-time status splash
- `bool shouldUpdate()` — Returns true every `DISPLAY_UPDATE_INTERVAL` ms

## Troubleshooting

### Pico W Not Detected on Raspberry Pi 4B

- Make sure USB cable supports data (not charge-only).
- Hold **BOOTSEL** before plugging in USB to enter mass-storage mode.
- Run `lsusb` — you should see `2e8a:0003 Raspberry Pi RP2 Boot`.
- Ensure your user is in the `dialout` group: `sudo usermod -aG dialout $USER` (log out/in).

### LoRa Module Not Responding

- Check wiring: GP0→RYLR896 RXD, GP1←RYLR896 TXD, GP14→NRESET.
- Power the RYLR896 from Pin 36 (3.3V) — **not 5V**.
- Confirm both devices use the same `LORA_FREQ_HZ` and `LORA_NETWORK_ID`.
- Open serial monitor and look for `[LoRa] No response from RYLR896`.

### GPS Not Getting Fix

- Use **outdoors** with clear sky view.
- Allow 27+ seconds for cold start.
- Check wiring: GP8→NEO-7m RXD, GP9←NEO-7m TXD.
- Confirm 9600 baud (`GPS_BAUD` in `PinConfig.h`).
- Serial monitor will print `[GPS] NEO-7m on UART1 ready` on success.

### Display Not Initialising

- Check I2C wiring: GP4 (SDA), GP5 (SCL), 3.3V, GND.
- Verify I2C address — most SSD1306 modules use `0x3C` (`OLED_I2C_ADDR` in `PinConfig.h`).
- Serial monitor will print `[Display] FAIL — check I2C wiring (GP4/GP5) and address 0x3C`.

## License

[Specify your license here]

## Acknowledgments

- [arduino-pico](https://github.com/earlephilhower/arduino-pico) by Earle F. Philhower III
- [platform-raspberrypi](https://github.com/maxgerhardt/platform-raspberrypi) by Maximilian Gerhardt
- TinyGPS++ by Mikal Hart
- Adafruit SSD1306 / GFX libraries

## Roadmap

- [ ] Add encryption for LoRa communications
- [ ] Implement power-saving sleep between heartbeats
- [ ] Add data logging to flash/SD
- [ ] Cloud integration (MQTT over Wi-Fi using the Pico W's CYW43439)
- [ ] Over-the-air firmware update via Wi-Fi

