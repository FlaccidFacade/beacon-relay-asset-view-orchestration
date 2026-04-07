/**
 * @file main.cpp
 * @brief B.R.A.V.O. firmware — Raspberry Pi Pico W
 *
 * Hardware:
 *   UART0 (GP0/GP1)   — RYLR896 LoRa (AT commands, 115200 baud)
 *   I2C0  (GP4/GP5)   — SSD1306 OLED 128x64
 *   UART1 (GP8/GP9)   — GPS NEO-7m (NMEA, 9600 baud)
 *   GP14              — RYLR896 NRESET
 *   GP15              — GPS PPS (1 Hz rising edge)
 *   GP16              — Push-button (INPUT_PULLUP, active LOW)
 *   VSYS (pin 39/40)  — 5V input power
 *   Pin 36 (3V3 OUT)  — 3.3V rail for all peripherals
 *
 * Build flags (platformio.ini):
 *   -D DEVICE_ADDRESS=1   (beacon)
 *   -D TARGET_ADDRESS=2   (relay/other unit)
 *   Swap values on the second unit.
 *
 * Button: short press cycles GPS screen → Radio screen → GPS screen.
 * LoRa:   every HEARTBEAT_INTERVAL ms, sends a GPS payload to TARGET_ADDRESS.
 *         Incoming packets are displayed on the radio screen.
 */

#include <Arduino.h>
#include "PinConfig.h"
#include "GPS.h"
#include "LoRaComm.h"
#include "Display.h"

// ── Device identity (set via build flags) ────────────────────────────────────
#ifndef DEVICE_ADDRESS
#define DEVICE_ADDRESS 1
#endif
#ifndef TARGET_ADDRESS
#define TARGET_ADDRESS 2
#endif

// ── Timing ────────────────────────────────────────────────────────────────────
static const uint32_t HEARTBEAT_INTERVAL  = 5000;  // ms between LoRa TX
static const uint32_t GPS_SAMPLE_INTERVAL = 1000;  // ms between GPS snapshots
static const uint32_t DEBOUNCE_MS         = 200;

// ── Globals ───────────────────────────────────────────────────────────────────
GPS      gpsModule;
LoRaComm lora;
Display  disp;

enum ScreenMode { SCREEN_GPS = 0, SCREEN_RADIO };
volatile ScreenMode currentScreen = SCREEN_GPS;

// Button state (ISR-safe)
volatile bool     buttonPressed    = false;
volatile uint32_t lastDebounceTime = 0;

// GPS state
GPSData  latestGPS      = {};
uint32_t lastGpsSample  = 0;

// LoRa state
uint32_t txCount         = 0;
uint32_t rxCount         = 0;
int      lastRSSI        = 0;
float    lastSNR         = 0.0f;
String   lastLoRaMsg     = "(none)";
uint32_t lastHeartbeat   = 0;

// ── ISR forward declares ─────────────────────────────────────────────────────
void buttonISR();
void gpsPPS();

// ── ISRs ──────────────────────────────────────────────────────────────────────
void buttonISR() {
    uint32_t now = millis();
    if (now - lastDebounceTime > DEBOUNCE_MS) {
        buttonPressed    = true;
        lastDebounceTime = now;
    }
}

void gpsPPS() {
    gpsModule.onPPS();
}

// ── Setup ─────────────────────────────────────────────────────────────────────
void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("[BRAVO] Pico W starting...");

    // Button — GP16 INPUT_PULLUP, trigger on falling edge (press)
    pinMode(PIN_BUTTON, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), buttonISR, FALLING);

    // GPS PPS interrupt — rising edge
    pinMode(PIN_GPS_PPS, INPUT);
    attachInterrupt(digitalPinToInterrupt(PIN_GPS_PPS), gpsPPS, RISING);

    // Display first so we can show init status
    if (!disp.begin()) {
        Serial.println("[BRAVO] Display failed — continuing headless");
    }

    // GPS
    bool gpsOk = gpsModule.begin();
    disp.showInitStatus("GPS", gpsOk);
    Serial.println(gpsOk ? "[BRAVO] GPS OK" : "[BRAVO] GPS FAIL");

    // LoRa
    bool loraOk = lora.begin(DEVICE_ADDRESS);
    disp.showInitStatus("LoRa", loraOk);
    Serial.println(loraOk ? "[BRAVO] LoRa OK" : "[BRAVO] LoRa FAIL");

    disp.showMessage("Ready!");
    delay(500);
    Serial.println("[BRAVO] Setup complete");
}

// ── Loop ──────────────────────────────────────────────────────────────────────
void loop() {
    // 1) Feed GPS parser
    gpsModule.update();

    // 2) Snapshot GPS data periodically
    if (millis() - lastGpsSample >= GPS_SAMPLE_INTERVAL) {
        lastGpsSample = millis();
        latestGPS     = gpsModule.getData();
    }

    // 3) LoRa TX heartbeat — send GPS payload to the other unit
    if (lora.isReady() && (millis() - lastHeartbeat >= HEARTBEAT_INTERVAL)) {
        lastHeartbeat = millis();

        // Compact payload: ADDR|lat|lon|sats
        String payload = String(DEVICE_ADDRESS) + "|" +
                         String(latestGPS.latitude,  5) + "|" +
                         String(latestGPS.longitude, 5) + "|" +
                         String(latestGPS.satellites);

        if (lora.sendMessage(TARGET_ADDRESS, payload)) {
            txCount++;
            Serial.println("[LoRa] TX → " + payload);
        } else {
            Serial.println("[LoRa] TX failed");
        }
    }

    // 4) LoRa RX — non-blocking poll
    if (lora.isReady()) {
        LoRaPacket pkt;
        if (lora.receive(pkt)) {
            rxCount++;
            lastRSSI    = pkt.rssi;
            lastSNR     = pkt.snr;
            lastLoRaMsg = pkt.payload;
            Serial.println("[LoRa] RX from " + String(pkt.srcAddress) +
                           ": " + pkt.payload +
                           " RSSI=" + String(pkt.rssi) +
                           " SNR="  + String(pkt.snr, 1));
        }
    }

    // 5) Button — cycle screen
    if (buttonPressed) {
        buttonPressed = false;
        currentScreen = (currentScreen == SCREEN_GPS) ? SCREEN_RADIO : SCREEN_GPS;
    }

    // 6) Refresh display at the Display module's own rate
    if (disp.shouldUpdate()) {
        if (currentScreen == SCREEN_GPS) {
            disp.showGPSScreen(latestGPS);
        } else {
            disp.showRadioScreen(txCount, rxCount, lastRSSI, lastSNR, lastLoRaMsg);
        }
    }
}
