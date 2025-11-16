/**
 * @file LoRaComm.cpp
 * @brief LoRa communication module implementation
 */

#include "LoRaComm.h"

LoRaComm::LoRaComm() : initialized(false) {
}

bool LoRaComm::begin() {
    Serial.println("LoRaComm: Starting initialization...");
    
    Serial.print("LoRaComm: Calling Heltec.LoRa.begin(");
    Serial.print(LORA_BAND);
    Serial.println(", PABOOST=true)...");
    Serial.println("LoRaComm: This may take a moment...");
    Serial.flush();
    
    // Try with PABOOST=true first
    bool result = Heltec.LoRa.begin(LORA_BAND, true);
    
    Serial.println("LoRaComm: Heltec.LoRa.begin() returned!");
    
    if (!result) {
        Serial.println("LoRaComm: Failed with PABOOST=true, trying PABOOST=false...");
        Serial.flush();
        
        // Try without PABOOST
        result = Heltec.LoRa.begin(LORA_BAND, false);
        
        if (!result) {
            Serial.println("LoRaComm: Both attempts failed!");
            Serial.println("LoRaComm: Check:");
            Serial.println("  1. Antenna is connected");
            Serial.println("  2. LoRa radio has power");
            Serial.println("  3. Correct frequency for region");
            return false;
        }
        Serial.println("LoRaComm: Success with PABOOST=false!");
    } else {
        Serial.println("LoRaComm: Success with PABOOST=true!");
    }

    Serial.println("LoRaComm: Heltec.LoRa.begin() succeeded");
    
    // Configure LoRa parameters
    Serial.println("LoRaComm: Setting spreading factor...");
    Heltec.LoRa.setSpreadingFactor(LORA_SPREAD);
    
    Serial.println("LoRaComm: Setting bandwidth...");
    Heltec.LoRa.setSignalBandwidth(LORA_BANDWIDTH);
    
    Serial.println("LoRaComm: Enabling CRC...");
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

int LoRaComm::getLastPacketRSSI() {
    return getRSSI();
}

float LoRaComm::getLastPacketSNR() {
    return getSNR();
}
