/**
 * @file GPS.h
 * @brief GPS module for B.R.A.V.O. beacon location tracking
 * 
 * This module handles GPS data acquisition and parsing for tracking
 * beacon locations.
 */

#ifndef GPS_H
#define GPS_H

#include <Arduino.h>
#include <TinyGPSPlus.h>

// GPS pin definitions for Heltec WiFi LoRa 32 V3
// Using available GPIOs - adjust based on your wiring
#define GPS_RX_PIN  33  // GPIO33 - UART RX for GPS TX
#define GPS_TX_PIN  34  // GPIO34 - UART TX for GPS RX (input only, for monitoring)
#define GPS_BAUD    9600

struct GPSData {
    double latitude;
    double longitude;
    double altitude;
    float speed;
    float course;
    uint8_t satellites;
    uint32_t hdop;
    bool valid;
    uint32_t timestamp;
};

class GPS {
public:
    /**
     * @brief Constructor for GPS
     */
    GPS();

    /**
     * @brief Initialize GPS module
     * @return true if initialization successful, false otherwise
     */
    bool begin();

    /**
     * @brief Update GPS data (call regularly in loop)
     */
    void update();

    /**
     * @brief Get current GPS location
     * @param lat Reference to store latitude
     * @param lon Reference to store longitude
     * @return true if location is valid, false otherwise
     */
    bool getLocation(double& lat, double& lon);

    /**
     * @brief Get current altitude
     * @return Altitude in meters
     */
    double getAltitude();

    /**
     * @brief Get current speed
     * @return Speed in km/h
     */
    float getSpeed();

    /**
     * @brief Get current course
     * @return Course in degrees
     */
    float getCourse();

    /**
     * @brief Get number of satellites
     * @return Number of satellites in view
     */
    uint8_t getSatellites();

    /**
     * @brief Check if GPS has valid fix
     * @return true if GPS has fix, false otherwise
     */
    bool hasFix();

    /**
     * @brief Get complete GPS data structure
     * @return GPSData structure with all GPS information
     */
    GPSData getData();
    
    /**
     * @brief Get number of characters processed from GPS
     * @return Number of characters processed
     */
    uint32_t getCharsProcessed();
    
    /**
     * @brief Get number of failed checksums
     * @return Number of failed checksums
     */
    uint32_t getFailedChecksums();

private:
    TinyGPSPlus gps;
    HardwareSerial* gpsSerial;
    bool initialized;
};

#endif // GPS_H
