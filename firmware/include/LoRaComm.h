/**
 * @file LoRaComm.h
 * @brief LoRa communication module for B.R.A.V.O. collar and dongle
 * 
 * This module handles long-range radio communication between collars and dongle
 * using the LoRa protocol.
 */

#ifndef LORA_COMM_H
#define LORA_COMM_H

#include <Arduino.h>
#include <LoRa.h>

// LoRa pin definitions for ESP32
#define LORA_SCK    5
#define LORA_MISO   19
#define LORA_MOSI   27
#define LORA_CS     18
#define LORA_RST    14
#define LORA_DIO0   26

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
