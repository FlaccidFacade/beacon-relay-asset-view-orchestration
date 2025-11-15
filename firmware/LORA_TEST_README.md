# LoRa Communication Test

## Setup

You need **TWO** Heltec WiFi LoRa 32 V3 boards to test LoRa communication.

### Board 1: TRANSMITTER
1. Edit `src/main.cpp`
2. Make sure this line is UNCOMMENTED:
   ```cpp
   #define LORA_TRANSMITTER  // Uncomment for TX board
   ```
3. Make sure this line is COMMENTED:
   ```cpp
   // #define LORA_RECEIVER     // Uncomment for RX board
   ```
4. Upload to first board
5. Disconnect and power via USB battery or separate power

### Board 2: RECEIVER
1. Edit `src/main.cpp`
2. Make sure this line is COMMENTED:
   ```cpp
   // #define LORA_TRANSMITTER  // Uncomment for TX board
   ```
3. Make sure this line is UNCOMMENTED:
   ```cpp
   #define LORA_RECEIVER     // Uncomment for RX board
   ```
4. Upload to second board
5. Keep connected to computer for serial monitoring

## What You'll See

### Transmitter Display:
```
TRANSMITTER
-----------
Sent: 25
Last: PKT_25
Status: OK
Time: 50s
```

### Receiver Display:
```
RECEIVER
--------
Rcvd: 25
Last: BRAVO_PKT_25
RSSI: -45 dBm
SNR: 9.5 dB
Time: 50s
```

### Serial Monitor (Receiver):
```
Received [RSSI: -45 dBm]: BRAVO_PKT_1
Received [RSSI: -46 dBm]: BRAVO_PKT_2
Received [RSSI: -44 dBm]: BRAVO_PKT_3
```

## Testing Distance

1. Start both boards next to each other
2. Verify packets are being received (RSSI around -30 to -50 dBm)
3. Slowly increase distance
4. Watch RSSI value decrease (more negative = weaker signal)
5. Typical ranges:
   - Indoor: 100-300 meters
   - Outdoor (line of sight): 1-5 km
   - Urban: 500m - 2km

## Signal Quality

- **RSSI (Received Signal Strength Indicator):**
  - -30 dBm: Excellent (very close)
  - -60 dBm: Good
  - -90 dBm: Fair
  - -120 dBm: Poor (max range)

- **SNR (Signal-to-Noise Ratio):**
  - > 5 dB: Excellent
  - 0-5 dB: Good
  - < 0 dB: Can still work with LoRa!

## Troubleshooting

### No packets received:
1. Check both boards show "LoRa initialized successfully" on display
2. Verify one board is TX and other is RX
3. Make sure both use same frequency (915 MHz)
4. Check antenna connections
5. Try moving boards closer together

### Intermittent reception:
- Normal at long distances
- Try adjusting antenna orientation
- Metal obstacles can block signal

### Low RSSI even when close:
- Check antenna is properly connected
- Antenna should be perpendicular to board
- Try outdoor test (walls absorb signal)

## Switching Back to GPS Test

```bash
cd /home/flaccidfacade/src/beacon-relay-asset-view-orchestration/firmware
mv src/main.cpp src/main_lora_test.cpp
mv main_gps_test.cpp.bak src/main.cpp
platformio run --target upload
```
