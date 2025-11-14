/**
 * @file BLEConfig.cpp
 * @brief BLE configuration module implementation
 */

#include "BLEConfig.h"

// Server callbacks class implementation
class BLEConfig::ServerCallbacks : public NimBLEServerCallbacks {
private:
    BLEConfig* parent;

public:
    ServerCallbacks(BLEConfig* p) : parent(p) {}

    void onConnect(NimBLEServer* pServer) {
        Serial.println("BLE client connected");
        parent->clientConnected = true;
    }

    void onDisconnect(NimBLEServer* pServer) {
        Serial.println("BLE client disconnected");
        parent->clientConnected = false;
        // Restart advertising
        pServer->startAdvertising();
    }
};

BLEConfig::BLEConfig() : pServer(nullptr), pService(nullptr), 
                         pConfigCharacteristic(nullptr), 
                         pStatusCharacteristic(nullptr),
                         pCommandCharacteristic(nullptr),
                         initialized(false), clientConnected(false) {
    // Initialize default config
    config.loraFrequency = 915;
    config.loraPower = 20;
    config.gpsInterval = 5000;
    config.telemetryInterval = 10000;
    strcpy(config.deviceName, "BRAVO_COLLAR");
}

bool BLEConfig::begin(const char* deviceName) {
    // Initialize BLE
    NimBLEDevice::init(deviceName);

    // Create BLE Server
    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks(this));

    // Create BLE Service
    pService = pServer->createService(SERVICE_UUID);

    // Create characteristics
    pConfigCharacteristic = pService->createCharacteristic(
        CONFIG_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::WRITE
    );

    pStatusCharacteristic = pService->createCharacteristic(
        STATUS_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );

    pCommandCharacteristic = pService->createCharacteristic(
        COMMAND_UUID,
        NIMBLE_PROPERTY::WRITE
    );

    // Start service
    pService->start();

    // Start advertising
    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->start();

    initialized = true;
    Serial.println("BLE initialized successfully");
    return true;
}

void BLEConfig::update() {
    // BLE stack handles updates automatically
    // This function is for future extensions
}

bool BLEConfig::isConnected() {
    return clientConnected;
}

BLEConfigData BLEConfig::getConfig() {
    return config;
}

void BLEConfig::setConfig(const BLEConfigData& newConfig) {
    config = newConfig;
}

void BLEConfig::sendStatus(const String& status) {
    if (!initialized || !pStatusCharacteristic) {
        return;
    }

    pStatusCharacteristic->setValue(status.c_str());
    if (clientConnected) {
        pStatusCharacteristic->notify();
    }
}

void BLEConfig::startAdvertising() {
    if (initialized) {
        NimBLEDevice::getAdvertising()->start();
    }
}

void BLEConfig::stopAdvertising() {
    if (initialized) {
        NimBLEDevice::getAdvertising()->stop();
    }
}
