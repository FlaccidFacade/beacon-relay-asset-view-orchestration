/**
 * @file Telemetry.h
 * @brief JSON telemetry module for B.R.A.V.O. data formatting
 * 
 * This module handles formatting of sensor data into JSON for transmission
 * to cloud services and mobile apps.
 */

#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "GPS.h"
#include "IMU.h"

// Telemetry packet types
enum TelemetryType {
    TELEMETRY_FULL,      // Complete telemetry with all sensors
    TELEMETRY_GPS,       // GPS data only
    TELEMETRY_IMU,       // IMU data only
    TELEMETRY_STATUS,    // Status/health data
    TELEMETRY_ALERT      // Alert/event data
};

class Telemetry {
public:
    /**
     * @brief Constructor for Telemetry
     */
    Telemetry();

    /**
     * @brief Create full telemetry JSON packet
     * @param gpsData GPS data structure
     * @param imuData IMU data structure
     * @param deviceId Device identifier
     * @param battery Battery level (0-100)
     * @return JSON string with telemetry data
     */
    String createFullTelemetry(const GPSData& gpsData, const IMUData& imuData, 
                               const char* deviceId, uint8_t battery);

    /**
     * @brief Create GPS-only telemetry packet
     * @param gpsData GPS data structure
     * @param deviceId Device identifier
     * @return JSON string with GPS data
     */
    String createGPSTelemetry(const GPSData& gpsData, const char* deviceId);

    /**
     * @brief Create IMU-only telemetry packet
     * @param imuData IMU data structure
     * @param deviceId Device identifier
     * @return JSON string with IMU data
     */
    String createIMUTelemetry(const IMUData& imuData, const char* deviceId);

    /**
     * @brief Create status telemetry packet
     * @param deviceId Device identifier
     * @param battery Battery level (0-100)
     * @param uptime Uptime in seconds
     * @param rssi Signal strength
     * @return JSON string with status data
     */
    String createStatusTelemetry(const char* deviceId, uint8_t battery, 
                                 uint32_t uptime, int rssi);

    /**
     * @brief Create alert telemetry packet
     * @param deviceId Device identifier
     * @param alertType Type of alert
     * @param message Alert message
     * @return JSON string with alert data
     */
    String createAlertTelemetry(const char* deviceId, const char* alertType, 
                                const char* message);

    /**
     * @brief Parse incoming JSON telemetry
     * @param json JSON string to parse
     * @return true if parsing successful, false otherwise
     */
    bool parseTelemetry(const String& json);

    /**
     * @brief Get last parsed telemetry type
     * @return TelemetryType of last parsed packet
     */
    TelemetryType getLastType();

private:
    StaticJsonDocument<512> doc;
    TelemetryType lastType;

    /**
     * @brief Add timestamp to JSON document
     */
    void addTimestamp();

    /**
     * @brief Add device info to JSON document
     * @param deviceId Device identifier
     */
    void addDeviceInfo(const char* deviceId);
};

#endif // TELEMETRY_H
