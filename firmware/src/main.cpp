/**
 * @file main.cpp
 * @brief Main firmware file for B.R.A.V.O. (Bluetooth Radio Advanced Visual Orchestration)
 * 
 * This is the main entry point for the ESP32 firmware running on both collars and dongle.
 * It integrates LoRa communication, GPS tracking, IMU motion sensing, BLE configuration,
 * OTA updates, and JSON telemetry formatting.
 * 
 * @author B.R.A.V.O. Team
 * @date 2025
 */

#include <Arduino.h>
#include "LoRaComm.h"
#include "GPS.h"
#include "BLEConfig.h"
#include "IMU.h"
#include "OTA.h"
#include "Telemetry.h"

// Device configuration
#define DEVICE_ID           "BRAVO_001"
#define DEVICE_TYPE_COLLAR  true  // Set to false for dongle

// Timing intervals (milliseconds)
#define GPS_UPDATE_INTERVAL         1000   // Update GPS every 1 second
#define IMU_UPDATE_INTERVAL         100    // Update IMU every 100ms
#define TELEMETRY_SEND_INTERVAL     10000  // Send telemetry every 10 seconds
#define STATUS_PRINT_INTERVAL       5000   // Print status every 5 seconds

// Module instances
LoRaComm lora;
GPS gps;
BLEConfig bleConfig;
IMU imu;
OTA ota;
Telemetry telemetry;

// Timing variables
unsigned long lastGPSUpdate = 0;
unsigned long lastIMUUpdate = 0;
unsigned long lastTelemetrySend = 0;
unsigned long lastStatusPrint = 0;

// Battery monitoring (placeholder - implement based on hardware)
uint8_t batteryLevel = 100;

/**
 * @brief Get battery level percentage
 * @return Battery level 0-100
 */
uint8_t getBatteryLevel() {
    // TODO: Implement actual battery monitoring via ADC
    // This is a placeholder that simulates battery drain
    static uint8_t battery = 100;
    if (battery > 0 && millis() % 60000 == 0) {
        battery--;
    }
    return battery;
}

/**
 * @brief Initialize all modules
 */
void initializeModules() {
    Serial.println("=== B.R.A.V.O. Firmware Initialization ===");
    Serial.print("Device ID: ");
    Serial.println(DEVICE_ID);
    Serial.print("Device Type: ");
    Serial.println(DEVICE_TYPE_COLLAR ? "Collar" : "Dongle");
    
    // Initialize LoRa
    Serial.println("\nInitializing LoRa...");
    if (lora.begin()) {
        Serial.println("✓ LoRa ready");
    } else {
        Serial.println("✗ LoRa failed");
    }

    // Initialize GPS
    Serial.println("\nInitializing GPS...");
    if (gps.begin()) {
        Serial.println("✓ GPS ready");
    } else {
        Serial.println("✗ GPS failed");
    }

    // Initialize IMU
    Serial.println("\nInitializing IMU...");
    if (imu.begin()) {
        Serial.println("✓ IMU ready");
    } else {
        Serial.println("✗ IMU failed");
    }

    // Initialize BLE
    Serial.println("\nInitializing BLE...");
    if (bleConfig.begin(DEVICE_ID)) {
        Serial.println("✓ BLE ready");
    } else {
        Serial.println("✗ BLE failed");
    }

    // Initialize OTA (optional - uncomment to enable WiFi OTA)
    // Serial.println("\nInitializing OTA...");
    // if (ota.connectWiFi("YourSSID", "YourPassword")) {
    //     ota.begin(DEVICE_ID);
    //     Serial.println("✓ OTA ready");
    // } else {
    //     Serial.println("✗ OTA WiFi connection failed");
    // }

    Serial.println("\n=== Initialization Complete ===\n");
}

/**
 * @brief Handle GPS updates
 */
void handleGPS() {
    gps.update();

    if (millis() - lastGPSUpdate >= GPS_UPDATE_INTERVAL) {
        lastGPSUpdate = millis();
        
        if (gps.hasFix()) {
            double lat, lon;
            gps.getLocation(lat, lon);
            // GPS data is ready for telemetry
        }
    }
}

/**
 * @brief Handle IMU updates
 */
void handleIMU() {
    if (millis() - lastIMUUpdate >= IMU_UPDATE_INTERVAL) {
        lastIMUUpdate = millis();
        
        if (imu.readSensor()) {
            // IMU data is ready for telemetry
            uint8_t activity = imu.getActivityLevel();
            
            // Check for motion events
            if (imu.isInMotion(1.0)) {
                // Motion detected - could trigger alert
            }
        }
    }
}

/**
 * @brief Handle telemetry transmission
 */
void handleTelemetry() {
    if (millis() - lastTelemetrySend >= TELEMETRY_SEND_INTERVAL) {
        lastTelemetrySend = millis();
        
        // Get current sensor data
        GPSData gpsData = gps.getData();
        IMUData imuData = imu.getData();
        batteryLevel = getBatteryLevel();

        // Create full telemetry packet
        String telemetryJson = telemetry.createFullTelemetry(
            gpsData, imuData, DEVICE_ID, batteryLevel
        );

        // Send via LoRa
        if (lora.sendMessage(telemetryJson)) {
            Serial.println("Telemetry sent via LoRa");
        }

        // Also send status to BLE if connected
        if (bleConfig.isConnected()) {
            bleConfig.sendStatus(telemetryJson);
        }
    }
}

/**
 * @brief Handle incoming LoRa messages
 */
void handleLoRaReceive() {
    if (lora.available()) {
        String message = lora.receiveMessage();
        int rssi = lora.getRSSI();
        float snr = lora.getSNR();

        Serial.println("=== LoRa Message Received ===");
        Serial.print("Message: ");
        Serial.println(message);
        Serial.print("RSSI: ");
        Serial.print(rssi);
        Serial.println(" dBm");
        Serial.print("SNR: ");
        Serial.println(snr);

        // Parse telemetry if it's JSON
        if (telemetry.parseTelemetry(message)) {
            Serial.println("Valid telemetry packet received");
        }
    }
}

/**
 * @brief Print status information
 */
void printStatus() {
    if (millis() - lastStatusPrint >= STATUS_PRINT_INTERVAL) {
        lastStatusPrint = millis();
        
        Serial.println("\n=== Status Update ===");
        Serial.print("Uptime: ");
        Serial.print(millis() / 1000);
        Serial.println(" seconds");
        
        Serial.print("Battery: ");
        Serial.print(batteryLevel);
        Serial.println("%");
        
        Serial.print("GPS Fix: ");
        Serial.println(gps.hasFix() ? "Yes" : "No");
        
        if (gps.hasFix()) {
            Serial.print("Satellites: ");
            Serial.println(gps.getSatellites());
        }
        
        Serial.print("BLE Connected: ");
        Serial.println(bleConfig.isConnected() ? "Yes" : "No");
        
        Serial.print("Activity Level: ");
        Serial.println(imu.getActivityLevel());
        
        Serial.println("====================\n");
    }
}

/**
 * @brief Arduino setup function
 */
void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n\n");

    // Initialize all modules
    initializeModules();
}

/**
 * @brief Arduino main loop function
 */
void loop() {
    // Update GPS continuously
    handleGPS();

    // Update IMU periodically
    handleIMU();

    // Handle BLE updates
    bleConfig.update();

    // Handle OTA updates (if enabled)
    // ota.handle();

    // Send telemetry periodically
    handleTelemetry();

    // Check for incoming LoRa messages
    handleLoRaReceive();

    // Print status periodically
    printStatus();

    // Small delay to prevent watchdog issues
    delay(10);
}
