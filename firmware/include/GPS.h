/**
 * @file GPS.h
 * @brief GPS module for B.R.A.V.O. — NEO-7m on Raspberry Pi Pico W
 *
 * UART1 (Serial2):  GP8 TX (Pico→GPS RXD),  GP9 RX (GPS TXD→Pico)
 * PPS interrupt:    GP15 rising edge = 1 Hz timing pulse
 * Power:            Pin 36 (3V3 OUT), any GND pin
 * Baud rate:        9600 (NEO-7m default NMEA output)
 */

#ifndef GPS_H
#define GPS_H

#include <Arduino.h>
#include <TinyGPSPlus.h>
#include "PinConfig.h"

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

    /** True when a PPS pulse has arrived since last call to clearPPS() */
    bool hasPPS() const { return ppsFlag; }
    void clearPPS()     { ppsFlag = false; }

    /** ISR called by the PPS interrupt — keep public so a free function can
     *  forward the call.  Do not call directly from application code. */
    void onPPS() { ppsFlag = true; }

private:
    TinyGPSPlus gps;
    bool        initialized;
    volatile bool ppsFlag;
};

#endif // GPS_H
