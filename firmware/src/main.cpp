/**
 * Button-driven status screens for GPS info and LoRa TX/RX stats.
 */

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#ifdef BLACK
#undef BLACK
#endif
#ifdef WHITE
#undef WHITE
#endif
#ifdef INVERSE
#undef INVERSE
#endif

#include "GPS.h"
#include "LoRaComm.h"
#include "heltec.h"

#define OLED_SDA 17
#define OLED_SCL 18
#define OLED_RST 21
#define Vext 36
#define BUTTON_PIN 0  // PRG button

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

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
