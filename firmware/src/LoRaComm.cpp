/**
 * @file LoRaComm.cpp
 * @brief LoRa communication module implementation
 */

#include "LoRaComm.h"

LoRaComm::LoRaComm() : initialized(false) {
}

bool LoRaComm::begin() {
    // Setup LoRa pins
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
    LoRa.setPins(LORA_CS, LORA_RST, LORA_DIO0);

    // Initialize LoRa module
    if (!LoRa.begin(LORA_BAND)) {
        Serial.println("LoRa init failed!");
        return false;
    }

    // Configure LoRa parameters
    LoRa.setSpreadingFactor(LORA_SPREAD);
    LoRa.setSignalBandwidth(LORA_BANDWIDTH);
    LoRa.enableCrc();

    initialized = true;
    Serial.println("LoRa initialized successfully");
    return true;
}

bool LoRaComm::sendData(const uint8_t* data, size_t length) {
    if (!initialized) {
        return false;
    }

    LoRa.beginPacket();
    LoRa.write(data, length);
    return LoRa.endPacket();
}

bool LoRaComm::sendMessage(const String& message) {
    if (!initialized) {
        return false;
    }

    LoRa.beginPacket();
    LoRa.print(message);
    return LoRa.endPacket();
}

bool LoRaComm::available() {
    if (!initialized) {
        return false;
    }

    return LoRa.parsePacket() > 0;
}

int LoRaComm::receiveData(uint8_t* buffer, size_t maxLength) {
    if (!initialized || !available()) {
        return 0;
    }

    int packetSize = LoRa.parsePacket();
    int bytesRead = 0;

    while (LoRa.available() && bytesRead < maxLength) {
        buffer[bytesRead++] = LoRa.read();
    }

    return bytesRead;
}

String LoRaComm::receiveMessage() {
    if (!initialized || !available()) {
        return "";
    }

    String message = "";
    while (LoRa.available()) {
        message += (char)LoRa.read();
    }

    return message;
}

int LoRaComm::getRSSI() {
    return LoRa.packetRssi();
}

float LoRaComm::getSNR() {
    return LoRa.packetSnr();
}
