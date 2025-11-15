/**
 * B.R.A.V.O. Firmware - GPS Display Test
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "GPS.h"

// Heltec WiFi LoRa 32 V3 OLED pins
#define OLED_SDA 17
#define OLED_SCL 18
#define OLED_RST 21
#define Vext 36  // Power enable pin for OLED

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);
GPS gpsModule;

void scanI2C() {
  Serial.println("Scanning I2C bus...");
  byte count = 0;
  
  for (byte i = 1; i < 127; i++) {
    Wire.beginTransmission(i);
    if (Wire.endTransmission() == 0) {
      Serial.print("Found device at 0x");
      if (i < 16) Serial.print("0");
      Serial.println(i, HEX);
      count++;
    }
  }
  
  if (count == 0) {
    Serial.println("No I2C devices found!");
  } else {
    Serial.print("Found ");
    Serial.print(count);
    Serial.println(" device(s)");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=== Heltec V3 Display Test ===");
  
  // Enable Vext power for OLED (critical for Heltec V3!)
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);  // LOW = ON for Vext
  delay(100);  // Give power time to stabilize
  Serial.println("Vext power enabled");
  
  // Initialize I2C with custom pins
  Wire.begin(OLED_SDA, OLED_SCL);
  
  // Scan for I2C devices
  scanI2C();
  
  // Initialize display
  Serial.println("Initializing display...");
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("SSD1306 allocation failed!");
    Serial.println("Trying alternate address 0x3D...");
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) {
      Serial.println("Display init failed at both addresses!");
      for(;;); // Don't proceed, loop forever
    }
  }
  
  Serial.println("Display initialized!");
  
  // Initialize GPS
  Serial.println("Initializing GPS...");
  if (gpsModule.begin()) {
    Serial.println("GPS initialized!");
  } else {
    Serial.println("GPS initialization failed!");
  }
  
  // Show startup screen
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("B.R.A.V.O. System");
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
  display.setCursor(0, 20);
  display.println("GPS: Initializing");
  display.setCursor(0, 30);
  display.println("Searching for");
  display.setCursor(0, 40);
  display.println("satellites...");
  display.display();
  
  Serial.println("System ready!");
}

void loop() {
  static unsigned long lastUpdate = 0;
  static unsigned long lastDebug = 0;
  static unsigned long lastRawGPS = 0;
  
  // Update GPS data continuously
  gpsModule.update();
  
  // Debug: Show raw GPS data (every 3 seconds - capture a full NMEA sentence)
  if (millis() - lastRawGPS >= 3000) {
    lastRawGPS = millis();
    Serial.println("\n--- Raw GPS Data (next 200 chars) ---");
    int count = 0;
    unsigned long startWait = millis();
    while (count < 200 && (millis() - startWait) < 500) {
      if (Serial2.available()) {
        char c = Serial2.read();
        Serial.print(c);
        count++;
      }
    }
    if (count == 0) {
      Serial.println("NO DATA FROM GPS MODULE!");
      Serial.println("Check wiring: GPS TX -> ESP32 Pin 33");
    }
    Serial.println("\n--- End Raw Data ---");
  }
  
  // Debug: Check if GPS is sending any data (every 5 seconds)
  if (millis() - lastDebug >= 5000) {
    lastDebug = millis();
    Serial.print("GPS Debug - Characters processed: ");
    Serial.print(gpsModule.getCharsProcessed());
    Serial.print(" | Failed checksums: ");
    Serial.println(gpsModule.getFailedChecksums());
  }
  
  // Update display every 1 second
  if (millis() - lastUpdate >= 1000) {
    lastUpdate = millis();
    
    double lat, lon;
    bool hasLocation = gpsModule.getLocation(lat, lon);
    uint8_t sats = gpsModule.getSatellites();
    
    // Update display
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    
    // Header
    display.setCursor(0, 0);
    display.println("B.R.A.V.O. GPS");
    display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
    
    // GPS Status
    display.setCursor(0, 13);
    display.print("Sats: ");
    display.print(sats);
    display.print(" | ");
    display.print(hasLocation ? "LOCK" : "SEARCH");
    
    // Always display coordinates (even if invalid, show 0.000000)
    display.setCursor(0, 23);
    display.print("Lat:");
    if (hasLocation) {
      display.print(lat, 6);
    } else {
      display.print(" No Fix");
    }
    
    display.setCursor(0, 33);
    display.print("Lon:");
    if (hasLocation) {
      display.print(lon, 6);
    } else {
      display.print(" No Fix");
    }
    
    // Show altitude or data status
    display.setCursor(0, 43);
    if (hasLocation) {
      double alt = gpsModule.getAltitude();
      display.print("Alt: ");
      display.print(alt, 1);
      display.print("m");
    } else {
      display.print("Chars: ");
      display.print(gpsModule.getCharsProcessed());
    }
    
    // Show speed or time
    display.setCursor(0, 53);
    if (hasLocation) {
      float speed = gpsModule.getSpeed();
      display.print("Spd: ");
      display.print(speed, 1);
      display.print("km/h");
    } else {
      display.print("Time: ");
      display.print(millis() / 1000);
      display.print("s");
    }
    
    if (hasLocation) {
      Serial.printf("GPS LOCK: %.6f, %.6f | Alt: %.1fm | Sats: %d\n", 
                    lat, lon, gpsModule.getAltitude(), sats);
    } else {
      Serial.printf("GPS SEARCH: Sats: %d | Chars: %d | Time: %ds\n", 
                    sats, gpsModule.getCharsProcessed(), millis()/1000);
    }
    
    display.display();
  }
  
  delay(10);  // Small delay to prevent overwhelming the serial port
}
