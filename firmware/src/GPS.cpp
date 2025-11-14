/**
 * @file GPS.cpp
 * @brief GPS module implementation
 */

#include "GPS.h"

GPS::GPS() : gpsSerial(nullptr), initialized(false) {
}

bool GPS::begin() {
    // Initialize hardware serial for GPS
    gpsSerial = &Serial2;
    gpsSerial->begin(GPS_BAUD, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);

    initialized = true;
    Serial.println("GPS initialized successfully");
    return true;
}

void GPS::update() {
    if (!initialized || !gpsSerial) {
        return;
    }

    // Feed GPS parser with available data
    while (gpsSerial->available() > 0) {
        gps.encode(gpsSerial->read());
    }
}

bool GPS::getLocation(double& lat, double& lon) {
    if (!initialized || !gps.location.isValid()) {
        return false;
    }

    lat = gps.location.lat();
    lon = gps.location.lng();
    return true;
}

double GPS::getAltitude() {
    if (!initialized || !gps.altitude.isValid()) {
        return 0.0;
    }

    return gps.altitude.meters();
}

float GPS::getSpeed() {
    if (!initialized || !gps.speed.isValid()) {
        return 0.0;
    }

    return gps.speed.kmph();
}

float GPS::getCourse() {
    if (!initialized || !gps.course.isValid()) {
        return 0.0;
    }

    return gps.course.deg();
}

uint8_t GPS::getSatellites() {
    if (!initialized || !gps.satellites.isValid()) {
        return 0;
    }

    return gps.satellites.value();
}

bool GPS::hasFix() {
    return initialized && gps.location.isValid();
}

GPSData GPS::getData() {
    GPSData data;
    
    if (hasFix()) {
        data.latitude = gps.location.lat();
        data.longitude = gps.location.lng();
        data.altitude = gps.altitude.meters();
        data.speed = gps.speed.kmph();
        data.course = gps.course.deg();
        data.satellites = gps.satellites.value();
        data.hdop = gps.hdop.value();
        data.valid = true;
        data.timestamp = millis();
    } else {
        data.latitude = 0.0;
        data.longitude = 0.0;
        data.altitude = 0.0;
        data.speed = 0.0;
        data.course = 0.0;
        data.satellites = 0;
        data.hdop = 0;
        data.valid = false;
        data.timestamp = millis();
    }

    return data;
}
