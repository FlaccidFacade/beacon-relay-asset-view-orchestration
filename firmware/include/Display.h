/**
 * @file Display.h
 * @brief OLED Display module for B.R.A.V.O. system
 * 
 * This module handles the built-in 0.96" OLED display on Heltec WiFi LoRa 32 V3
 * to show GPS coordinates, device status, and other information.
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "GPS.h"
#include "IMU.h"

// Display configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
#define OLED_SDA 17
#define OLED_SCL 18
#define OLED_RST 21
#define Vext 36  // Power enable pin for OLED on Heltec V3

// Display update interval
#define DISPLAY_UPDATE_INTERVAL 1000  // Update display every 1 second

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
    void updateStatus(const GPSData& gpsData, const char* deviceId, 
                     const char* deviceType, uint8_t battery, bool bleConnected);

    /**
     * @brief Show GPS coordinates on display
     * @param gpsData GPS data structure
     */
    void showGPS(const GPSData& gpsData);

    /**
     * @brief Show device information
     * @param deviceId Device identifier
     * @param deviceType Device type string
     */
    void showDeviceInfo(const char* deviceId, const char* deviceType);

    /**
     * @brief Show initialization status
     * @param module Module name
     * @param success Success/failure status
     */
    void showInitStatus(const char* module, bool success);

    /**
     * @brief Show message on display
     * @param message Message to display
     */
    void showMessage(const char* message);

    /**
     * @brief Check if display should be updated
     * @return true if update interval has elapsed
     */
    bool shouldUpdate();

private:
    Adafruit_SSD1306* display;
    bool initialized;
    unsigned long lastUpdate;
};

#endif // DISPLAY_H
