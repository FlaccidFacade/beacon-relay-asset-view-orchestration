/**
 * @file main_hi_test.cpp
 * @brief Minimal OLED sanity check — shows "HI" on the SSD1306 display.
 *
 * Confirms a Pico W boots and its display works, independent of GPS/LoRa.
 * Build/flash with the rpicow_hi_test environment, then run on both
 * boards to visually verify displays before debugging LoRa comms.
 */

#include <Arduino.h>
#include "Display.h"

Display disp;

void setup() {
    Serial1.setTX(PIN_DEBUG_TX);
    Serial1.setRX(PIN_DEBUG_RX);
    Serial1.begin(115200);
    delay(1000);
    Serial1.println("[BRAVO] HI test starting...");

    bool ok = disp.begin();
    Serial1.println(ok ? "[Display] init OK" : "[Display] init FAIL");

    Serial1.println("[BRAVO] Setup complete");
}

void loop() {
    // Redraw continuously so the message survives any display glitch/timeout.
    disp.showMessage("HI");
    delay(1000);
}
