/**
 * @file main_random_lora_test.cpp
 * @brief Standalone LoRa & OLED test firmware.
 *
 * Device 1 generates a random number every 5 seconds, displays it on its
 * SSD1306 OLED, and sends it via LoRa (REYAX RYLR896) to Device 2.
 * Device 2 receives the LoRa packet and displays the number along with
 * signal metrics (RSSI, SNR) on its SSD1306 OLED screen.
 *
 * Roles are configured at compile time:
 *   Device 1 (Sender):   -D DEVICE_ADDRESS=1 -D TARGET_ADDRESS=2 -D LORA_TX_ENABLED=1 -D LORA_RX_ENABLED=0
 *   Device 2 (Receiver): -D DEVICE_ADDRESS=2 -D TARGET_ADDRESS=1 -D LORA_TX_ENABLED=0 -D LORA_RX_ENABLED=1
 */

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "PinConfig.h"
#include "LoRaComm.h"

// Fallback definitions if not provided via build flags
#ifndef DEVICE_ADDRESS
#define DEVICE_ADDRESS 1
#endif

#ifndef TARGET_ADDRESS
#define TARGET_ADDRESS 2
#endif

#ifndef LORA_TX_ENABLED
#define LORA_TX_ENABLED 1
#endif

#ifndef LORA_RX_ENABLED
#define LORA_RX_ENABLED 1
#endif

// Interval between sending random numbers (5 seconds)
static const unsigned long TX_INTERVAL_MS = 5000;

// OLED Display (SSD1306 128x64 on I2C0: GP4 SDA, GP5 SCL)
static Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
static bool displayOk = false;

// LoRa Comm instance
static LoRaComm lora;

// State tracking
static unsigned long lastTxTime = 0;
static uint32_t txCount = 0;
static uint32_t rxCount = 0;
static long currentRandomNumber = 0;
static long lastReceivedNumber = 0;
static int lastRssi = 0;
static float lastSnr = 0.0f;
static bool lastTxSuccess = false;
static bool hasReceivedData = false;

static void updateDisplaySender() {
    if (!displayOk) return;

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    // Header
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("--- RANDOM TX TEST ---");
    display.printf("Dev: %d -> Target: %d\n", DEVICE_ADDRESS, TARGET_ADDRESS);
    display.println("---------------------");

    // Random Number (Large font)
    display.setTextSize(2);
    display.setCursor(0, 24);
    display.printf("%ld\n", currentRandomNumber);

    // TX Stats
    display.setTextSize(1);
    display.setCursor(0, 46);
    display.printf("TX #%lu: %s\n", (unsigned long)txCount, lastTxSuccess ? "OK" : "FAIL");
    display.printf("Interval: %lus", TX_INTERVAL_MS / 1000);

    display.display();
}

static void updateDisplayReceiver() {
    if (!displayOk) return;

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    // Header
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("--- RANDOM RX TEST ---");
    display.printf("Dev: %d (Listen:%d)\n", DEVICE_ADDRESS, TARGET_ADDRESS);
    display.println("---------------------");

    if (hasReceivedData) {
        // Random Number (Large font)
        display.setTextSize(2);
        display.setCursor(0, 24);
        display.printf("%ld\n", lastReceivedNumber);

        // Signal metrics & count
        display.setTextSize(1);
        display.setCursor(0, 46);
        display.printf("RSSI:%d SNR:%.1f\n", lastRssi, lastSnr);
        display.printf("RX #%lu packets", (unsigned long)rxCount);
    } else {
        display.setTextSize(1);
        display.setCursor(0, 28);
        display.println("Waiting for LoRa...");
        display.setCursor(0, 48);
        display.println("No packets yet");
    }

    display.display();
}

void setup() {
    // Debug UART (GP0 TX / GP1 RX)
    Serial1.setTX(PIN_DEBUG_TX);
    Serial1.setRX(PIN_DEBUG_RX);
    Serial1.begin(115200);
    delay(1000);

    Serial1.println("========================================");
    Serial1.printf("[RANDOM_TEST] Starting on Device Address %d (Target %d)\n", DEVICE_ADDRESS, TARGET_ADDRESS);
    Serial1.printf("[RANDOM_TEST] Role: TX_ENABLED=%d, RX_ENABLED=%d\n", LORA_TX_ENABLED, LORA_RX_ENABLED);
    Serial1.println("========================================");

    // Initialize OLED Display (I2C0: GP4 SDA, GP5 SCL)
    Wire.setSDA(PIN_OLED_SDA);
    Wire.setSCL(PIN_OLED_SCL);
    Wire.begin();

    if (display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR)) {
        displayOk = true;
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.println("B.R.A.V.O. Test");
        display.println("Init LoRa...");
        display.display();
        Serial1.println("[Display] init OK");
    } else {
        Serial1.println("[Display] init FAIL");
    }

    // Initialize LoRa
    bool loraOk = lora.begin(DEVICE_ADDRESS);
    Serial1.println(loraOk ? "[LoRa] init OK" : "[LoRa] init FAIL");

    // Seed pseudo-random number generator
    uint32_t seed = rp2040.hw_rand32();
    randomSeed(seed);

    if (LORA_TX_ENABLED && !LORA_RX_ENABLED) {
        updateDisplaySender();
    } else {
        updateDisplayReceiver();
    }

    Serial1.println("[BRAVO] Setup complete");
}

void loop() {
    unsigned long now = millis();

    // SENDER ROLE: Generate and transmit random number every 5s
    if (LORA_TX_ENABLED) {
        if (now - lastTxTime >= TX_INTERVAL_MS) {
            lastTxTime = now;

            // Generate 5-digit random number (10000 - 99999)
            currentRandomNumber = random(10000, 100000);
            txCount++;

            Serial1.printf("[RANDOM_TEST] TX generated number: %ld (count #%lu)\n",
                           currentRandomNumber, (unsigned long)txCount);

            // Format payload: "RND:<number>"
            String payload = "RND:" + String(currentRandomNumber);

            // Send payload via LoRa
            lastTxSuccess = lora.sendMessage(TARGET_ADDRESS, payload);
            Serial1.printf("[RANDOM_TEST] TX result: %s\n", lastTxSuccess ? "SUCCESS" : "FAIL");

            // Update display with newly generated number
            updateDisplaySender();
        }
    }

    // RECEIVER ROLE: Poll for incoming LoRa packets
    if (LORA_RX_ENABLED) {
        LoRaPacket packet;
        if (lora.receive(packet)) {
            Serial1.printf("[LoRa] RX raw from %d: %s (RSSI:%d, SNR:%.1f)\n",
                           packet.srcAddress, packet.payload.c_str(), packet.rssi, packet.snr);

            if (packet.payload.startsWith("RND:")) {
                String numStr = packet.payload.substring(4);
                lastReceivedNumber = numStr.toInt();
                lastRssi = packet.rssi;
                lastSnr = packet.snr;
                hasReceivedData = true;
                rxCount++;

                Serial1.printf("[RANDOM_TEST] RX received number: %ld from dev %d (RSSI: %d, SNR: %.1f)\n",
                               lastReceivedNumber, packet.srcAddress, lastRssi, lastSnr);

                // Update receiver display immediately upon packet arrival
                updateDisplayReceiver();
            }
        }
    }

    delay(10);
}
