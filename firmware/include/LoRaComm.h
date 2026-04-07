/**
 * @file LoRaComm.h
 * @brief LoRa communication module for B.R.A.V.O. — REYAX RYLR896
 *
 * The RYLR896 is a UART-based LoRa transceiver controlled entirely via
 * AT commands at 115 200 baud.  No SPI or external library required.
 *
 * AT command summary used here:
 *   AT+RESET                          software reset
 *   AT+ADDRESS=<n>                    set device address (0–65535)
 *   AT+NETWORKID=<n>                  set network ID    (0–9, 18)
 *   AT+BAND=<hz>                      carrier frequency in Hz
 *   AT+PARAMETER=<SF>,<BW>,<CR>,<PP>  RF parameters
 *   AT+SEND=<addr>,<len>,<payload>    transmit
 *   → incoming: +RCV=<addr>,<len>,<payload>,<RSSI>,<SNR>
 */

#ifndef LORA_COMM_H
#define LORA_COMM_H

#include <Arduino.h>
#include "PinConfig.h"

// Maximum AT payload the RYLR896 can accept (bytes)
#define RYLR_MAX_PAYLOAD 240

struct LoRaPacket {
    uint16_t srcAddress;
    String   payload;
    int      rssi;
    float    snr;
    bool     valid;
};

class LoRaComm {
public:
    LoRaComm();

    /**
     * Initialise UART0, hardware-reset the module, and configure RF params.
     * @param deviceAddress  This device's LoRa address (0–65535)
     * @return true on success
     */
    bool begin(uint16_t deviceAddress);

    /**
     * Send a string payload to a specific address.
     * @param targetAddress  Destination LoRa address
     * @param message        Payload string (max RYLR_MAX_PAYLOAD chars)
     * @return true if "+SEND=" acknowledgement received
     */
    bool sendMessage(uint16_t targetAddress, const String& message);

    /**
     * Poll the UART receive buffer for an incoming +RCV packet.
     * Non-blocking; call every loop iteration.
     * @param out  Populated if a complete packet was parsed
     * @return true if a new packet is available in `out`
     */
    bool receive(LoRaPacket& out);

    /** Last packet RSSI (dBm) */
    int  getLastRSSI() const { return lastRSSI; }
    /** Last packet SNR  (dB)  */
    float getLastSNR()  const { return lastSNR;  }
    /** Returns true if begin() succeeded */
    bool isReady()      const { return initialized; }

private:
    bool     initialized;
    int      lastRSSI;
    float    lastSNR;
    String   rxBuffer;

    /** Send raw AT command and wait up to `timeoutMs` for a line starting
     *  with `expectedPrefix`.  Returns the matched response line. */
    String sendAT(const String& cmd,
                  const String& expectedPrefix,
                  uint32_t      timeoutMs = 2000);

    void   hardwareReset();
    bool   parseRCV(const String& line, LoRaPacket& out);
};

#endif // LORA_COMM_H
