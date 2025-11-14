/**
 * @file OTA.h
 * @brief OTA (Over-The-Air) update module for B.R.A.V.O. firmware
 * 
 * This module handles wireless firmware updates via WiFi for remote
 * collar and dongle updates.
 */

#ifndef OTA_H
#define OTA_H

#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>

// Default OTA settings
#define OTA_PORT        3232
#define OTA_PASSWORD    "bravo123"  // Change in production!

class OTA {
public:
    /**
     * @brief Constructor for OTA
     */
    OTA();

    /**
     * @brief Initialize OTA module
     * @param hostname Hostname for OTA device
     * @param password Password for OTA updates (optional)
     * @return true if initialization successful, false otherwise
     */
    bool begin(const char* hostname, const char* password = OTA_PASSWORD);

    /**
     * @brief Handle OTA updates (call regularly in loop)
     */
    void handle();

    /**
     * @brief Connect to WiFi network
     * @param ssid WiFi SSID
     * @param password WiFi password
     * @param timeout Connection timeout in milliseconds
     * @return true if connected, false otherwise
     */
    bool connectWiFi(const char* ssid, const char* password, unsigned long timeout = 10000);

    /**
     * @brief Check if WiFi is connected
     * @return true if connected, false otherwise
     */
    bool isWiFiConnected();

    /**
     * @brief Disconnect from WiFi
     */
    void disconnectWiFi();

    /**
     * @brief Enable OTA updates
     */
    void enable();

    /**
     * @brief Disable OTA updates
     */
    void disable();

    /**
     * @brief Check if OTA is enabled
     * @return true if enabled, false otherwise
     */
    bool isEnabled();

private:
    bool initialized;
    bool enabled;
    String hostname;

    // OTA event callbacks
    static void onStart();
    static void onEnd();
    static void onProgress(unsigned int progress, unsigned int total);
    static void onError(ota_error_t error);
};

#endif // OTA_H
