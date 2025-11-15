/**
 * @file LoRaComm.cpp
 * @brief LoRa communication module implementation
 */

#include "LoRaComm.h"

LoRaComm::LoRaComm() : initialized(false) {
}

bool LoRaComm::begin() {
    // Initialize LoRa using Heltec library (handles pins internally)
    // Note: Heltec.begin() should be called in main.cpp before this
    // Second parameter is PABOOST - true for high power mode
    
    // Initialize LoRa module
    if (!Heltec.LoRa.begin(LORA_BAND, true)) {
        Serial.println("LoRa init failed!");
        return false;
    }

    // Configure LoRa parameters
    Heltec.LoRa.setSpreadingFactor(LORA_SPREAD);
    Heltec.LoRa.setSignalBandwidth(LORA_BANDWIDTH);
    Heltec.LoRa.enableCrc();

    initialized = true;
    Serial.println("LoRa initialized successfully");
    return true;
}

bool LoRaComm::sendData(const uint8_t* data, size_t length) {
    if (!initialized) {
        return false;
    }

    Heltec.LoRa.beginPacket();
    Heltec.LoRa.write(data, length);
    return Heltec.LoRa.endPacket();
}

bool LoRaComm::sendMessage(const String& message) {
    if (!initialized) {
        return false;
    }

    Heltec.LoRa.beginPacket();
    Heltec.LoRa.print(message);
    return Heltec.LoRa.endPacket();
}

bool LoRaComm::available() {
    if (!initialized) {
        return false;
    }

    return Heltec.LoRa.parsePacket() > 0;
}

int LoRaComm::receiveData(uint8_t* buffer, size_t maxLength) {
    if (!initialized || !available()) {
        return 0;
    }

    int packetSize = Heltec.LoRa.parsePacket();
    int bytesRead = 0;

    while (Heltec.LoRa.available() && bytesRead < maxLength) {
        buffer[bytesRead++] = Heltec.LoRa.read();
    }

    return bytesRead;
}

String LoRaComm::receiveMessage() {
    if (!initialized || !available()) {
        return "";
    }

    String message = "";
    while (Heltec.LoRa.available()) {
        message += (char)Heltec.LoRa.read();
    }

    return message;
}

int LoRaComm::getRSSI() {
    return Heltec.LoRa.packetRssi();
}

float LoRaComm::getSNR() {
    return Heltec.LoRa.packetSnr();
}
