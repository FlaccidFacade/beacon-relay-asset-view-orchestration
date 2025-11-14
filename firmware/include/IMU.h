/**
 * @file IMU.h
 * @brief IMU (Inertial Measurement Unit) module for B.R.A.V.O. collar
 * 
 * This module handles accelerometer and gyroscope data from the MPU6050
 * for activity tracking and motion detection.
 */

#ifndef IMU_H
#define IMU_H

#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// IMU I2C pins for ESP32
#define IMU_SDA_PIN 21
#define IMU_SCL_PIN 22

struct IMUData {
    float accelX;
    float accelY;
    float accelZ;
    float gyroX;
    float gyroY;
    float gyroZ;
    float temperature;
    uint32_t timestamp;
};

class IMU {
public:
    /**
     * @brief Constructor for IMU
     */
    IMU();

    /**
     * @brief Initialize IMU module
     * @return true if initialization successful, false otherwise
     */
    bool begin();

    /**
     * @brief Read sensor data from IMU
     * @return true if read successful, false otherwise
     */
    bool readSensor();

    /**
     * @brief Get acceleration values
     * @param x Reference to store X acceleration (m/s²)
     * @param y Reference to store Y acceleration (m/s²)
     * @param z Reference to store Z acceleration (m/s²)
     */
    void getAcceleration(float& x, float& y, float& z);

    /**
     * @brief Get gyroscope values
     * @param x Reference to store X rotation (rad/s)
     * @param y Reference to store Y rotation (rad/s)
     * @param z Reference to store Z rotation (rad/s)
     */
    void getGyro(float& x, float& y, float& z);

    /**
     * @brief Get temperature
     * @return Temperature in Celsius
     */
    float getTemperature();

    /**
     * @brief Get complete IMU data structure
     * @return IMUData structure with all sensor readings
     */
    IMUData getData();

    /**
     * @brief Calculate activity level based on acceleration
     * @return Activity level (0-100)
     */
    uint8_t getActivityLevel();

    /**
     * @brief Detect if collar is in motion
     * @param threshold Motion detection threshold
     * @return true if motion detected, false otherwise
     */
    bool isInMotion(float threshold = 0.5);

private:
    Adafruit_MPU6050 mpu;
    IMUData currentData;
    bool initialized;
};

#endif // IMU_H
