/**
 * @file LoRaComm.h
 * @brief LoRa communication module for B.R.A.V.O. beacons and relays
 * 
 * This module handles long-range radio communication between beacons and relays
 * using the LoRa protocol.
 */

#ifndef LORA_COMM_H
#define LORA_COMM_H

#include <Arduino.h>
#include "heltec.h"

// LoRa pin definitions for Heltec WiFi LoRa 32 V3
#define LORA_SCK    9
#define LORA_MISO   11
#define LORA_MOSI   10
#define LORA_CS     8
#define LORA_RST    12
#define LORA_DIO0   14

// LoRa configuration
#define LORA_BAND   915E6  // 915 MHz (North America)
#define LORA_SPREAD 7
#define LORA_BANDWIDTH 125E3

class LoRaComm {
public:
    /**
     * @brief Constructor for LoRaComm
     */
    LoRaComm();

    /**
     * @brief Initialize LoRa module
     * @return true if initialization successful, false otherwise
     */
    bool begin();

    /**
     * @brief Send data over LoRa
     * @param data Data buffer to send
     * @param length Length of data
     * @return true if send successful, false otherwise
     */
    bool sendData(const uint8_t* data, size_t length);

    /**
     * @brief Send string message over LoRa
     * @param message String message to send
     * @return true if send successful, false otherwise
     */
    bool sendMessage(const String& message);

    /**
     * @brief Check if data is available to receive
     * @return true if data available, false otherwise
     */
    bool available();

    /**
     * @brief Receive data from LoRa
     * @param buffer Buffer to store received data
     * @param maxLength Maximum length to receive
     * @return Number of bytes received
     */
    int receiveData(uint8_t* buffer, size_t maxLength);

    /**
     * @brief Receive string message from LoRa
     * @return Received message string
     */
    String receiveMessage();

    /**
     * @brief Get RSSI of last received packet
     * @return RSSI value in dBm
     */
    int getRSSI();

    /**
     * @brief Get SNR of last received packet
     * @return SNR value
     */
    float getSNR();

private:
    bool initialized;
};

#endif // LORA_COMM_H
