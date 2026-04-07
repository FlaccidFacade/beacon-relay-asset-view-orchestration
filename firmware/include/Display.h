/**
 * @file Display.h
 * @brief OLED Display module for B.R.A.V.O. — SSD1306 128x64 on Pico W
 *
 * I2C0: GP4 SDA, GP5 SCL  (set in PinConfig.h)
 * Power: Pin 36 (3V3 OUT) supplies 3.3V; any GND pin provides ground
 * No external reset pin — pass -1 to Adafruit_SSD1306 constructor.
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "GPS.h"
#include "PinConfig.h"

// Display update interval (ms)
#define DISPLAY_UPDATE_INTERVAL 1000

class Display {
public:
    /**
     * @brief Constructor for Display
     */
    Display();

    /**
     * @brief Initialize OLED display
     * @return true if initialization successful, false otherwise
     */
    bool begin();

    /**
     * @brief Clear the display
     */
    void clear();

    /**
     * @brief Update display with current system status
     * @param gpsData Current GPS data
     * @param deviceId Device identifier
     * @param deviceType Device type (beacon or relay)
     * @param battery Battery level (0-100)
     * @param bleConnected BLE connection status
     */
    /**
     * Screen A: GPS coordinates + satellite count.
     */
    void showGPSScreen(const GPSData& gpsData);

    /**
     * Screen B: LoRa radio stats (tx count, rx count, RSSI, SNR).
     */
    void showRadioScreen(uint32_t txCount, uint32_t rxCount,
                         int rssi, float snr,
                         const String& lastMsg);

    void updateStatus(const GPSData& gpsData, const char* deviceId,
                     const char* deviceType);

    void showInitStatus(const char* module, bool success);
    void showMessage(const char* message);
    bool shouldUpdate();

private:
    Adafruit_SSD1306 display;   // Stack-allocated (no dynamic allocation)
    bool             initialized;
    unsigned long    lastUpdate;
};

#endif // DISPLAY_H
