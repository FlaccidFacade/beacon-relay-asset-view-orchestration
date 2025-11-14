/**
 * @file IMU.cpp
 * @brief IMU module implementation
 */

#include "IMU.h"
#include <Wire.h>

IMU::IMU() : initialized(false) {
    memset(&currentData, 0, sizeof(IMUData));
}

bool IMU::begin() {
    // Initialize I2C
    Wire.begin(IMU_SDA_PIN, IMU_SCL_PIN);

    // Initialize MPU6050
    if (!mpu.begin()) {
        Serial.println("Failed to find MPU6050 chip");
        return false;
    }

    Serial.println("MPU6050 Found!");

    // Configure MPU6050
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    initialized = true;
    Serial.println("IMU initialized successfully");
    return true;
}

bool IMU::readSensor() {
    if (!initialized) {
        return false;
    }

    sensors_event_t accel, gyro, temp;
    mpu.getEvent(&accel, &gyro, &temp);

    currentData.accelX = accel.acceleration.x;
    currentData.accelY = accel.acceleration.y;
    currentData.accelZ = accel.acceleration.z;
    currentData.gyroX = gyro.gyro.x;
    currentData.gyroY = gyro.gyro.y;
    currentData.gyroZ = gyro.gyro.z;
    currentData.temperature = temp.temperature;
    currentData.timestamp = millis();

    return true;
}

void IMU::getAcceleration(float& x, float& y, float& z) {
    x = currentData.accelX;
    y = currentData.accelY;
    z = currentData.accelZ;
}

void IMU::getGyro(float& x, float& y, float& z) {
    x = currentData.gyroX;
    y = currentData.gyroY;
    z = currentData.gyroZ;
}

float IMU::getTemperature() {
    return currentData.temperature;
}

IMUData IMU::getData() {
    return currentData;
}

uint8_t IMU::getActivityLevel() {
    if (!initialized) {
        return 0;
    }

    // Calculate magnitude of acceleration vector
    float magnitude = sqrt(
        currentData.accelX * currentData.accelX +
        currentData.accelY * currentData.accelY +
        currentData.accelZ * currentData.accelZ
    );

    // Subtract gravity (9.8 m/s²) and normalize to 0-100
    float activity = abs(magnitude - 9.8);
    uint8_t level = (uint8_t)(min(activity * 10.0, 100.0));

    return level;
}

bool IMU::isInMotion(float threshold) {
    if (!initialized) {
        return false;
    }

    float magnitude = sqrt(
        currentData.accelX * currentData.accelX +
        currentData.accelY * currentData.accelY +
        currentData.accelZ * currentData.accelZ
    );

    // Check if acceleration differs from gravity by more than threshold
    return abs(magnitude - 9.8) > threshold;
}
