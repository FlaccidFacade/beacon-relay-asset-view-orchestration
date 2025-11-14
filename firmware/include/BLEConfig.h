/**
 * @file BLEConfig.h
 * @brief BLE configuration module for B.R.A.V.O. collar settings
 * 
 * This module provides Bluetooth Low Energy interface for configuring
 * collar settings via mobile app.
 */

#ifndef BLE_CONFIG_H
#define BLE_CONFIG_H

#include <Arduino.h>
#include <NimBLEDevice.h>

// BLE Service and Characteristic UUIDs
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CONFIG_UUID         "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define STATUS_UUID         "1c95d5e3-d8f7-413a-bf3d-7a2e5d7be87e"
#define COMMAND_UUID        "d8de624e-140f-4a22-8594-e2216b84a5f2"

struct BLEConfigData {
    uint16_t loraFrequency;
    uint8_t loraPower;
    uint16_t gpsInterval;
    uint16_t telemetryInterval;
    char deviceName[32];
};

class BLEConfig {
public:
    /**
     * @brief Constructor for BLEConfig
     */
    BLEConfig();

    /**
     * @brief Initialize BLE module
     * @param deviceName Name for BLE advertising
     * @return true if initialization successful, false otherwise
     */
    bool begin(const char* deviceName);

    /**
     * @brief Update BLE (call regularly in loop)
     */
    void update();

    /**
     * @brief Check if client is connected
     * @return true if client connected, false otherwise
     */
    bool isConnected();

    /**
     * @brief Get configuration data
     * @return BLEConfigData structure
     */
    BLEConfigData getConfig();

    /**
     * @brief Set configuration data
     * @param config BLEConfigData structure to set
     */
    void setConfig(const BLEConfigData& config);

    /**
     * @brief Send status update to connected client
     * @param status Status string to send
     */
    void sendStatus(const String& status);

    /**
     * @brief Start BLE advertising
     */
    void startAdvertising();

    /**
     * @brief Stop BLE advertising
     */
    void stopAdvertising();

private:
    BLEServer* pServer;
    BLEService* pService;
    BLECharacteristic* pConfigCharacteristic;
    BLECharacteristic* pStatusCharacteristic;
    BLECharacteristic* pCommandCharacteristic;
    BLEConfigData config;
    bool initialized;
    bool clientConnected;

    class ServerCallbacks;
};

#endif // BLE_CONFIG_H
