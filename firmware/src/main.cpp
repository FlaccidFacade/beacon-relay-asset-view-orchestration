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
void gppsPPS();

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


enum DeviceRole {
  ROLE_BEACON = 0,
  ROLE_RELAY = 1,
  ROLE_BOTH = 2
};

#ifndef DEVICE_ROLE
#define DEVICE_ROLE ROLE_BOTH
#endif

const char* DEVICE_ID = "BRAVO_TEST";

enum ScreenMode {
  SCREEN_GPS = 0,
  SCREEN_RADIO = 1
};

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);
GPS gps;
LoRaComm lora;

// Button state
volatile bool buttonPressed = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;

// Screen / data state
ScreenMode currentScreen = SCREEN_GPS;
unsigned long lastDisplayRefresh = 0;
const unsigned long DISPLAY_REFRESH_INTERVAL = 1000;

// GPS state
bool gpsReady = false;
GPSData latestGpsData = {};
unsigned long lastGpsSample = 0;
const unsigned long GPS_SAMPLE_INTERVAL = 1000;

// LoRa state
bool loraReady = false;
unsigned long lastHeartbeat = 0;
const unsigned long HEARTBEAT_INTERVAL = 5000;
uint32_t txCount = 0;
uint32_t rxCount = 0;
int lastRSSI = 0;
float lastSNR = 0.0f;
String lastLoRaMessage = "(none)";
bool lastSendSuccess = false;

void IRAM_ATTR buttonISR();
void updateDisplay(bool force = false);
void drawGpsScreen();
void drawRadioScreen();
void handleLoRa();
const char* deviceRoleString();
bool isBeaconRole();
bool isRelayRole();

void IRAM_ATTR buttonISR() {
  unsigned long currentTime = millis();
  if (currentTime - lastDebounceTime > debounceDelay) {
    buttonPressed = true;
    lastDebounceTime = currentTime;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Starting BRAVO status display...");
  
  // Setup button
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);
  
  // Enable Vext power
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
  delay(100);

  Serial.println("Initializing Heltec core (peripherals only)...");
  Heltec.begin(
    false,      // Display handled manually
    false,      // Skip Heltec LoRa init (we handle separately)
    true,       // Serial
    true,       // PA boost enabled when we start LoRa ourselves
    LORA_BAND
  );
  Serial.println("Heltec core ready");
  
  // Initialize display
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("Display failed!");
    while(1) {
      delay(1000);
    }
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("BRAVO Display");
  display.println("Initializing...");
  display.display();
  Serial.println("Display OK!");

  // Initialize modules
  Serial.println("Bringing up GPS module...");
  gpsReady = gps.begin();
  Serial.println(gpsReady ? "GPS ready" : "GPS failed");

  Serial.println("Bringing up LoRa module...");
  loraReady = lora.begin();
  Serial.println(loraReady ? "LoRa ready" : "LoRa failed");
  latestGpsData = gps.getData();

  if (!gpsReady) {
    Serial.println("GPS init failed");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("GPS INIT FAIL");
    display.display();
    delay(1000);
  }
  if (!loraReady) {
    Serial.println("LoRa init failed");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("LORA INIT FAIL");
    display.display();
    delay(1000);
  }

  updateDisplay(true);
  Serial.println("Setup complete!");
}

void loop() {
  gps.update();

  if (millis() - lastGpsSample >= GPS_SAMPLE_INTERVAL) {
    lastGpsSample = millis();
    latestGpsData = gps.getData();
    if (currentScreen == SCREEN_GPS) {
      updateDisplay(true);
    }
  }

  if (loraReady) {
    handleLoRa();
  }

  if (buttonPressed) {
    buttonPressed = false;
    currentScreen = (currentScreen == SCREEN_GPS) ? SCREEN_RADIO : SCREEN_GPS;
    Serial.print("Switched to screen: ");
    Serial.println(currentScreen == SCREEN_GPS ? "GPS" : "RADIO");
    updateDisplay(true);
  }

  updateDisplay();
  delay(5);
}

void updateDisplay(bool force) {
  unsigned long now = millis();
  if (!force && (now - lastDisplayRefresh) < DISPLAY_REFRESH_INTERVAL) {
    return;
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  if (currentScreen == SCREEN_GPS) {
    drawGpsScreen();
  } else {
    drawRadioScreen();
  }

  display.display();
  lastDisplayRefresh = now;
}

void drawGpsScreen() {
  display.setCursor(0, 0);
  display.println("GPS INFO");
  display.print("Fix: ");
  display.println(latestGpsData.valid ? "YES" : "NO");
  display.print("Sat: ");
  display.println(latestGpsData.satellites);
  display.print("Lat: ");
  if (latestGpsData.valid) {
    display.println(latestGpsData.latitude, 5);
  } else {
    display.println("--");
  }
  display.print("Lon: ");
  if (latestGpsData.valid) {
    display.println(latestGpsData.longitude, 5);
  } else {
    display.println("--");
  }
  display.print("Alt: ");
  display.print(latestGpsData.altitude, 1);
  display.println("m");
  display.print("Spd: ");
  display.print(latestGpsData.speed, 1);
  display.println("kmh");
}

void drawRadioScreen() {
  display.setCursor(0, 0);
  display.println("RADIO INFO");
  display.print("Role: ");
  display.println(deviceRoleString());
  display.print("LoRa: ");
  display.println(loraReady ? "READY" : "ERROR");
  display.print("TX: ");
  display.println(txCount);
  display.print("RX: ");
  display.println(rxCount);
  display.print("RSSI: ");
  if (rxCount > 0) {
    display.println(lastRSSI);
  } else {
    display.println("--");
  }
  display.print("SNR: ");
  if (rxCount > 0) {
    display.println(lastSNR, 1);
  } else {
    display.println("--");
  }
  display.print("LastTX: ");
  display.println(lastSendSuccess ? "OK" : "--");
  display.print("Msg: ");
  String preview = lastLoRaMessage;
  if (preview.length() > 16) {
    preview = preview.substring(0, 16);
  }
  display.println(preview);
}

void handleLoRa() {
  unsigned long now = millis();

  if (isBeaconRole()) {
    if (now - lastHeartbeat >= HEARTBEAT_INTERVAL) {
      lastHeartbeat = now;
      String payload = String("BEACON|") + DEVICE_ID + "|" + String(now / 1000);
      lastSendSuccess = lora.sendMessage(payload);
      if (lastSendSuccess) {
        txCount++;
        Serial.println("LoRa TX: " + payload);
      } else {
        Serial.println("LoRa TX failed");
      }
      if (currentScreen == SCREEN_RADIO) {
        updateDisplay(true);
      }
    }
  }

  if (lora.available()) {
    String message = lora.receiveMessage();
    if (message.length() > 0) {
      rxCount++;
      lastRSSI = lora.getRSSI();
      lastSNR = lora.getSNR();
      lastLoRaMessage = message;
      Serial.println("LoRa RX: " + message);
      if (currentScreen == SCREEN_RADIO) {
        updateDisplay(true);
      }
    }
  }
}

const char* deviceRoleString() {
  switch (DEVICE_ROLE) {
    case ROLE_BEACON:
      return "Beacon";
    case ROLE_RELAY:
      return "Relay";
    case ROLE_BOTH:
    default:
      return "Beacon+Relay";
  }
}

bool isBeaconRole() {
  return DEVICE_ROLE == ROLE_BEACON || DEVICE_ROLE == ROLE_BOTH;
}

bool isRelayRole() {
  return DEVICE_ROLE == ROLE_RELAY || DEVICE_ROLE == ROLE_BOTH;
}
