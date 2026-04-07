/**
 * @file Display.cpp
 * @brief SSD1306 128x64 OLED driver for Pico W — I2C0 on GP4 (SDA) / GP5 (SCL)
 *
 * Wire.setSDA / Wire.setSCL must be called before Wire.begin() in arduino-pico.
 * The display object is stack-allocated so no heap allocation is needed.
 */

#include "Display.h"

Display::Display()
    : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1),
      initialized(false), lastUpdate(0) {}

bool Display::begin() {
    Serial.println("[Display] Initializing SSD1306...");

    Wire.setSDA(PIN_OLED_SDA);
    Wire.setSCL(PIN_OLED_SCL);
    Wire.begin();

    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR)) {
        Serial.println("[Display] FAIL — check I2C wiring (GP4/GP5) and address 0x3C");
        return false;
    }

    initialized = true;
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("B.R.A.V.O.");
    display.println("Starting...");
    display.display();
    Serial.println("[Display] SSD1306 OK");
    return true;
}

void Display::clear() {
    if (!initialized) return;
    display.clearDisplay();
    display.display();
}

// ── Screen A: GPS ───────────────────────────────────────────────────────────

void Display::showGPSScreen(const GPSData& gpsData) {
    if (!initialized) return;
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);  display.println("-- GPS --");
    display.print("Fix: ");   display.println(gpsData.valid ? "YES" : "NO ");
    display.print("Sat: ");   display.println(gpsData.satellites);
    display.print("Lat: ");   display.println(gpsData.valid ? String(gpsData.latitude,  5) : "--");
    display.print("Lon: ");   display.println(gpsData.valid ? String(gpsData.longitude, 5) : "--");
    display.print("Alt: ");
    if (gpsData.valid) { display.print(gpsData.altitude, 1); display.println("m"); }
    else               { display.println("--"); }

    display.display();
    lastUpdate = millis();
}

// ── Screen B: LoRa Radio ─────────────────────────────────────────────────────

void Display::showRadioScreen(uint32_t txCount, uint32_t rxCount,
                               int rssi, float snr,
                               const String& lastMsg) {
    if (!initialized) return;
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    display.setCursor(0, 0);
    display.println("-- RADIO --");
    display.print("TX: ");   display.println(txCount);
    display.print("RX: ");   display.println(rxCount);
    display.print("RSSI: "); display.println(rxCount > 0 ? String(rssi)    : "--");
    display.print("SNR:  "); display.println(rxCount > 0 ? String(snr, 1)  : "--");
    display.print("Msg: ");
    display.println(lastMsg.length() > 14 ? lastMsg.substring(0, 14) : lastMsg);

    display.display();
    lastUpdate = millis();
}

// ── Utility screens ──────────────────────────────────────────────────────────

void Display::updateStatus(const GPSData& gpsData, const char* deviceId,
                            const char* deviceType) {
    if (!initialized || !shouldUpdate()) return;
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(deviceId);
    display.println(deviceType);
    display.print("Fix: ");  display.println(gpsData.valid ? "YES" : "NO");
    display.print("Sats: "); display.println(gpsData.satellites);
    display.display();
    lastUpdate = millis();
}

void Display::showInitStatus(const char* module, bool success) {
    if (!initialized) return;
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 20);
    display.print(module);
    display.print(": ");
    display.println(success ? "OK" : "FAIL");
    display.display();
    delay(600);
}

void Display::showMessage(const char* message) {
    if (!initialized) return;
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 20);
    display.println(message);
    display.display();
}

bool Display::shouldUpdate() {
    return (millis() - lastUpdate >= DISPLAY_UPDATE_INTERVAL);
}
