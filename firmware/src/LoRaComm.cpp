/**
 * @file LoRaComm.cpp
 * @brief LoRa communication module implementation
 */

#include <algorithm>
#include <cstring>
#include "LoRaComm.h"

#ifndef LORA_SPI_HOST
#if defined(ARDUINO_ARCH_ESP32)
#define LORA_SPI_HOST  FSPI
#else
#define LORA_SPI_HOST  HSPI
#endif
#endif

LoRaComm::LoRaComm()
    : initialized(false),
      spi(nullptr),
      module(nullptr),
      radio(nullptr),
      pendingMessage(false),
      pendingPayload(""),
      lastPacketRSSI(0),
      lastPacketSNR(0.0f) {}

void LoRaComm::resetPending() {
    pendingMessage = false;
    pendingPayload = "";
}

bool LoRaComm::begin() {
    Serial.println("LoRaComm: Initializing SX1262 via RadioLib...");

    if (spi == nullptr) {
        spi = new SPIClass(LORA_SPI_HOST);
    }
    spi->begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);

    if (module == nullptr) {
        module = new Module(LORA_CS, LORA_DIO1, LORA_RST, LORA_BUSY, *spi);
    }
    if (radio == nullptr) {
        radio = new SX1262(module);
    }

    const float freqMHz = static_cast<float>(LORA_BAND) / 1000000.0f;
    const float bandwidthKHz = static_cast<float>(LORA_BANDWIDTH) / 1000.0f;

    int16_t state = radio->begin(freqMHz, bandwidthKHz, LORA_SPREAD, 7, RADIOLIB_SX126X_SYNC_WORD_PRIVATE, 17, 12, 1.8);
    if (state != RADIOLIB_ERR_NONE) {
        Serial.print("LoRaComm: RadioLib begin failed, code ");
        Serial.println(state);
        return false;
    }

    radio->setDio2AsRfSwitch(true);
    radio->setCRC(2);
    radio->startReceive();

    resetPending();
    initialized = true;
    Serial.println("LoRaComm: SX1262 ready");
    return true;
}

bool LoRaComm::sendData(const uint8_t* data, size_t length) {
    if (!initialized || radio == nullptr || data == nullptr || length == 0) {
        return false;
    }

    if (length > RADIOLIB_SX126X_MAX_PACKET_LENGTH) {
        Serial.println("LoRaComm: payload too large");
        return false;
    }

    uint8_t buffer[RADIOLIB_SX126X_MAX_PACKET_LENGTH];
    memcpy(buffer, data, length);

    int16_t state = radio->transmit(buffer, length);
    radio->startReceive();
    return state == RADIOLIB_ERR_NONE;
}

bool LoRaComm::sendMessage(const String& message) {
    if (!initialized || radio == nullptr) {
        return false;
    }

    int16_t state = radio->transmit(message.c_str(), message.length());
    radio->startReceive();
    return state == RADIOLIB_ERR_NONE;
}

bool LoRaComm::available() {
    if (!initialized) {
        return false;
    }

    if (pendingMessage) {
        return true;
    }

    return fetchPacket();
}

int LoRaComm::receiveData(uint8_t* buffer, size_t maxLength) {
    if (!available()) {
        return 0;
    }

    size_t toCopy = std::min(maxLength, static_cast<size_t>(pendingPayload.length()));
    memcpy(buffer, pendingPayload.c_str(), toCopy);
    resetPending();
    return static_cast<int>(toCopy);
}

String LoRaComm::receiveMessage() {
    if (!available()) {
        return "";
    }

    String message = pendingPayload;
    resetPending();
    return message;
}

int LoRaComm::getRSSI() {
    return lastPacketRSSI;
}

float LoRaComm::getSNR() {
    return lastPacketSNR;
}

int LoRaComm::getLastPacketRSSI() {
    return getRSSI();
}

float LoRaComm::getLastPacketSNR() {
    return getSNR();
}
bool LoRaComm::fetchPacket() {
    if (!radio) {
        return false;
    }

    // Ask RadioLib how long the last received packet was (if any).
    // For SX1262, getPacketLength(true) will return 0 when there is no pending packet.
    size_t length = radio->getPacketLength(true);
    if (length == 0 || length > RADIOLIB_SX126X_MAX_PACKET_LENGTH) {
        // No packet or bogus length, keep listening
        radio->startReceive();
        return false;
    }

    uint8_t buffer[RADIOLIB_SX126X_MAX_PACKET_LENGTH + 1];
    int16_t state = radio->readData(buffer, length);
    if (state != RADIOLIB_ERR_NONE) {
        Serial.print("LoRaComm: readData failed code ");
        Serial.println(state);
        radio->startReceive();
        return false;
    }

    buffer[length] = '\0';
    pendingPayload = String(reinterpret_cast<char*>(buffer));
    pendingMessage = true;

    // These are public PhysicalLayer / SX126x methods
    lastPacketRSSI = static_cast<int>(radio->getRSSI());
    lastPacketSNR = radio->getSNR();

    // Go back to RX for the next packet
    radio->startReceive();
    return true;
}