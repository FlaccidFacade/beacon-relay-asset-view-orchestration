/**
 * @file Telemetry.cpp
 * @brief JSON telemetry module implementation
 */

#include "Telemetry.h"

Telemetry::Telemetry() : lastType(TELEMETRY_FULL) {
}

void Telemetry::addTimestamp() {
    doc["timestamp"] = millis();
}

void Telemetry::addDeviceInfo(const char* deviceId) {
    doc["device_id"] = deviceId;
}

String Telemetry::createFullTelemetry(const GPSData& gpsData, const IMUData& imuData, 
                                      const char* deviceId, uint8_t battery) {
    doc.clear();
    
    addDeviceInfo(deviceId);
    addTimestamp();
    doc["type"] = "full";
    doc["battery"] = battery;

    // GPS data
    JsonObject gps = doc.createNestedObject("gps");
    gps["valid"] = gpsData.valid;
    gps["lat"] = gpsData.latitude;
    gps["lon"] = gpsData.longitude;
    gps["alt"] = gpsData.altitude;
    gps["speed"] = gpsData.speed;
    gps["course"] = gpsData.course;
    gps["satellites"] = gpsData.satellites;

    // IMU data
    JsonObject imu = doc.createNestedObject("imu");
    JsonObject accel = imu.createNestedObject("accel");
    accel["x"] = imuData.accelX;
    accel["y"] = imuData.accelY;
    accel["z"] = imuData.accelZ;
    
    JsonObject gyro = imu.createNestedObject("gyro");
    gyro["x"] = imuData.gyroX;
    gyro["y"] = imuData.gyroY;
    gyro["z"] = imuData.gyroZ;
    
    imu["temp"] = imuData.temperature;

    String output;
    serializeJson(doc, output);
    return output;
}

String Telemetry::createGPSTelemetry(const GPSData& gpsData, const char* deviceId) {
    doc.clear();
    
    addDeviceInfo(deviceId);
    addTimestamp();
    doc["type"] = "gps";

    doc["valid"] = gpsData.valid;
    doc["lat"] = gpsData.latitude;
    doc["lon"] = gpsData.longitude;
    doc["alt"] = gpsData.altitude;
    doc["speed"] = gpsData.speed;
    doc["course"] = gpsData.course;
    doc["satellites"] = gpsData.satellites;

    String output;
    serializeJson(doc, output);
    return output;
}

String Telemetry::createIMUTelemetry(const IMUData& imuData, const char* deviceId) {
    doc.clear();
    
    addDeviceInfo(deviceId);
    addTimestamp();
    doc["type"] = "imu";

    JsonObject accel = doc.createNestedObject("accel");
    accel["x"] = imuData.accelX;
    accel["y"] = imuData.accelY;
    accel["z"] = imuData.accelZ;
    
    JsonObject gyro = doc.createNestedObject("gyro");
    gyro["x"] = imuData.gyroX;
    gyro["y"] = imuData.gyroY;
    gyro["z"] = imuData.gyroZ;
    
    doc["temp"] = imuData.temperature;

    String output;
    serializeJson(doc, output);
    return output;
}

String Telemetry::createStatusTelemetry(const char* deviceId, uint8_t battery, 
                                        uint32_t uptime, int rssi) {
    doc.clear();
    
    addDeviceInfo(deviceId);
    addTimestamp();
    doc["type"] = "status";
    doc["battery"] = battery;
    doc["uptime"] = uptime;
    doc["rssi"] = rssi;

    String output;
    serializeJson(doc, output);
    return output;
}

String Telemetry::createAlertTelemetry(const char* deviceId, const char* alertType, 
                                       const char* message) {
    doc.clear();
    
    addDeviceInfo(deviceId);
    addTimestamp();
    doc["type"] = "alert";
    doc["alert_type"] = alertType;
    doc["message"] = message;

    String output;
    serializeJson(doc, output);
    return output;
}

bool Telemetry::parseTelemetry(const String& json) {
    doc.clear();
    
    DeserializationError error = deserializeJson(doc, json);
    if (error) {
        Serial.print("JSON parse error: ");
        Serial.println(error.c_str());
        return false;
    }

    // Determine type
    const char* type = doc["type"];
    if (strcmp(type, "full") == 0) {
        lastType = TELEMETRY_FULL;
    } else if (strcmp(type, "gps") == 0) {
        lastType = TELEMETRY_GPS;
    } else if (strcmp(type, "imu") == 0) {
        lastType = TELEMETRY_IMU;
    } else if (strcmp(type, "status") == 0) {
        lastType = TELEMETRY_STATUS;
    } else if (strcmp(type, "alert") == 0) {
        lastType = TELEMETRY_ALERT;
    }

    return true;
}

TelemetryType Telemetry::getLastType() {
    return lastType;
}
