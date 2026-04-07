/**
 * @file GPS.cpp
 * @brief GPS module implementation for Pico W — NEO-7m on UART1 (Serial2)
 *
 * Serial2.setTX/setRX must be called before Serial2.begin() in arduino-pico.
 * The PPS interrupt free-function lives in main.cpp and calls GPS::onPPS().
 */

#include "GPS.h"

// arduino-pico maps Serial2 to UART1
#define GPS_SERIAL Serial2

GPS::GPS() : initialized(false), ppsFlag(false) {}

bool GPS::begin() {
    GPS_SERIAL.setTX(PIN_GPS_TX);
    GPS_SERIAL.setRX(PIN_GPS_RX);
    GPS_SERIAL.begin(GPS_BAUD);

    // Configure PPS pin as input (interrupt attached in main.cpp)
    pinMode(PIN_GPS_PPS, INPUT);

    initialized = true;
    Serial.println("[GPS] NEO-7m on UART1 ready");
    return true;
}

void GPS::update() {
    if (!initialized) return;
    while (GPS_SERIAL.available() > 0) {
        gps.encode((char)GPS_SERIAL.read());
    }
}

bool GPS::getLocation(double& lat, double& lon) {
    if (!initialized || !gps.location.isValid()) return false;
    lat = gps.location.lat();
    lon = gps.location.lng();
    return true;
}

double GPS::getAltitude() {
    if (!initialized || !gps.altitude.isValid()) return 0.0;
    return gps.altitude.meters();
}

float GPS::getSpeed() {
    if (!initialized || !gps.speed.isValid()) return 0.0f;
    return gps.speed.kmph();
}

float GPS::getCourse() {
    if (!initialized || !gps.course.isValid()) return 0.0f;
    return gps.course.deg();
}

uint8_t GPS::getSatellites() {
    if (!initialized || !gps.satellites.isValid()) return 0;
    return (uint8_t)gps.satellites.value();
}

bool GPS::hasFix() {
    return initialized && gps.location.isValid();
}

GPSData GPS::getData() {
    GPSData data;
    data.timestamp = millis();
    if (hasFix()) {
        data.latitude   = gps.location.lat();
        data.longitude  = gps.location.lng();
        data.altitude   = gps.altitude.isValid()   ? gps.altitude.meters()         : 0.0;
        data.speed      = gps.speed.isValid()      ? gps.speed.kmph()              : 0.0f;
        data.course     = gps.course.isValid()     ? gps.course.deg()              : 0.0f;
        data.satellites = gps.satellites.isValid() ? (uint8_t)gps.satellites.value() : 0;
        data.hdop       = gps.hdop.isValid()       ? gps.hdop.value()              : 0;
        data.valid      = true;
    } else {
        data.latitude = data.longitude = data.altitude = 0.0;
        data.speed = data.course = 0.0f;
        data.satellites = gps.satellites.isValid() ? (uint8_t)gps.satellites.value() : 0;
        data.hdop  = 0;
        data.valid = false;
    }
    return data;
}

uint32_t GPS::getCharsProcessed()  { return gps.charsProcessed(); }
uint32_t GPS::getFailedChecksums() { return gps.failedChecksum(); }

