/**
 * @file LoRaComm.cpp
 * @brief REYAX RYLR896 LoRa driver — AT command interface via SerialPIO (Pico W)
 *
 * Hardware connections (Pico W):
 *   GP16 (SerialPIO TX) → RYLR896 RXD
 *   GP17 (SerialPIO RX) ← RYLR896 TXD
 *   GP14                → RYLR896 NRESET  (active LOW)
 *   Pin 36 (3V3)        → RYLR896 VCC
 *   Any GND pin         → RYLR896 GND
 *
 * SerialPIO is used because GP16/GP17 are UART0 alternate pins on RP2040;
 * hardware UART0 (Serial1) is reserved for debug output on GP0/GP1.
 */

#include "LoRaComm.h"
#include <SerialPIO.h>

// PIO-based software UART — pins and 256-byte RX FIFO bound at construction
static SerialPIO loraSerial(PIN_LORA_TX, PIN_LORA_RX, 256);
#define LORA_SERIAL loraSerial

LoRaComm::LoRaComm()
    : initialized(false), lastRSSI(0), lastSNR(0.0f), rxBuffer("") {}

// ── Private helpers ──────────────────────────────────────────────────────────

void LoRaComm::hardwareReset() {
    pinMode(PIN_LORA_RESET, OUTPUT);
    digitalWrite(PIN_LORA_RESET, LOW);
    delay(200);
    digitalWrite(PIN_LORA_RESET, HIGH);
    delay(1000);  // datasheet: wait ≥1 s after reset for module to be ready
}

/**
 * Write an AT command, then block until a line containing `expectedPrefix`
 * arrives or `timeoutMs` elapses.  Returns the full matching line, or "".
 */
String LoRaComm::sendAT(const String& cmd,
                         const String& expectedPrefix,
                         uint32_t      timeoutMs) {
    // Flush stale bytes
    while (LORA_SERIAL.available()) LORA_SERIAL.read();

    LORA_SERIAL.print(cmd);
    LORA_SERIAL.print("\r\n");

    uint32_t deadline = millis() + timeoutMs;
    String   line;
    while (millis() < deadline) {
        while (LORA_SERIAL.available()) {
            char c = (char)LORA_SERIAL.read();
            if (c == '\n') {
                line.trim();
                if (line.startsWith(expectedPrefix)) {
                    return line;
                }
                line = "";
            } else if (c != '\r') {
                line += c;
            }
        }
    }
    return "";  // timed out
}

// ── Public API ────────────────────────────────────────────────────────────────

bool LoRaComm::begin(uint16_t deviceAddress) {
    // SerialPIO pins are bound in the constructor; only baud rate is set here
    LORA_SERIAL.begin(LORA_BAUD);

    Serial1.println("[LoRa] Resetting RYLR896...");
    hardwareReset();

    // Verify module is alive
    String resp = sendAT("AT", "+OK", 2000);
    if (resp == "") {
        // Try once more — some modules produce "Ready" rather than "+OK"
        resp = sendAT("AT", "Ready", 1500);
        if (resp == "") {
            Serial1.println("[LoRa] No response from RYLR896");
            return false;
        }
    }

    // Set device address
    resp = sendAT("AT+ADDRESS=" + String(deviceAddress), "+ADDRESS=", 2000);
    Serial1.println("[LoRa] ADDRESS → " + resp);

    // Set network ID
    resp = sendAT("AT+NETWORKID=" + String(LORA_NETWORK_ID), "+NETWORKID=", 2000);
    Serial1.println("[LoRa] NETWORKID → " + resp);

    // Set carrier frequency (Hz)
    resp = sendAT("AT+BAND=" + String(LORA_FREQ_HZ), "+BAND=", 2000);
    Serial1.println("[LoRa] BAND → " + resp);

    // Set RF parameters: SF, BW, CR, Preamble
    String paramCmd = "AT+PARAMETER=" +
                      String(LORA_PARAM_SF) + "," +
                      String(LORA_PARAM_BW) + "," +
                      String(LORA_PARAM_CR) + "," +
                      String(LORA_PARAM_PP);
    resp = sendAT(paramCmd, "+PARAMETER=", 2000);
    Serial1.println("[LoRa] PARAMETER → " + resp);

    initialized = true;
    Serial1.println("[LoRa] RYLR896 ready");
    return true;
}

bool LoRaComm::sendMessage(uint16_t targetAddress, const String& message) {
    if (!initialized) return false;

    if (message.length() > RYLR_MAX_PAYLOAD) {
        Serial1.println("[LoRa] Payload too large");
        return false;
    }

    String cmd = "AT+SEND=" + String(targetAddress) + "," +
                 String(message.length())          + "," +
                 message;

    String resp = sendAT(cmd, "+SEND=", 3000);
    if (resp == "") {
        Serial1.println("[LoRa] sendMessage: no ACK");
        return false;
    }
    return true;
}

/**
 * Non-blocking receive.  Accumulates characters into rxBuffer and checks for
 * a complete "+RCV=..." line each call.
 */
bool LoRaComm::receive(LoRaPacket& out) {
    while (LORA_SERIAL.available()) {
        char c = (char)LORA_SERIAL.read();
        if (c == '\n') {
            rxBuffer.trim();
            if (rxBuffer.startsWith("+RCV=")) {
                bool parsed = parseRCV(rxBuffer, out);
                rxBuffer = "";
                if (parsed) {
                    lastRSSI = out.rssi;
                    lastSNR  = out.snr;
                    return true;
                }
            }
            rxBuffer = "";
        } else if (c != '\r') {
            rxBuffer += c;
        }
    }
    return false;
}

/**
 * Parse:  +RCV=<addr>,<len>,<payload>,<RSSI>,<SNR>
 */
bool LoRaComm::parseRCV(const String& line, LoRaPacket& out) {
    // Strip "+RCV="
    String body = line.substring(5);  // after "+RCV="

    // addr
    int sep1 = body.indexOf(',');
    if (sep1 < 0) return false;
    out.srcAddress = (uint16_t)body.substring(0, sep1).toInt();

    // len
    int sep2 = body.indexOf(',', sep1 + 1);
    if (sep2 < 0) return false;
    int payloadLen = body.substring(sep1 + 1, sep2).toInt();

    // payload  (length chars after sep2+1)
    if (sep2 + 1 + payloadLen > (int)body.length()) return false;
    out.payload = body.substring(sep2 + 1, sep2 + 1 + payloadLen);

    // RSSI
    int sep3 = body.indexOf(',', sep2 + 1 + payloadLen);
    if (sep3 < 0) return false;
    int sep4 = body.indexOf(',', sep3 + 1);
    if (sep4 < 0) return false;
    out.rssi = body.substring(sep3 + 1, sep4).toInt();

    // SNR
    out.snr   = body.substring(sep4 + 1).toFloat();
    out.valid = true;
    return true;
}
