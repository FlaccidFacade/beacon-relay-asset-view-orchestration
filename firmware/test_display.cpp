/**
 * Simple OLED test for Heltec WiFi LoRa 32 V3
 * This will help us debug the display issue
 */

#include <Arduino.h>
#include <Wire.h>
#include "SSD1306Wire.h"

// Try different possible pin configurations for Heltec V3
#define OLED_SDA 17
#define OLED_SCL 18
#define OLED_RST 21

SSD1306Wire display(0x3c, OLED_SDA, OLED_SCL);

void scanI2C() {
  Serial.println("Scanning I2C bus...");
  Wire.begin(OLED_SDA, OLED_SCL);
  
  byte error, address;
  int nDevices = 0;
  
  for(address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      nDevices++;
    }
  }
  
  if (nDevices == 0)
    Serial.println("No I2C devices found");
  else
    Serial.println("I2C scan complete");
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n\n=== Heltec OLED Test ===");
  
  // Scan I2C bus first
  scanI2C();
  delay(1000);
  
  // Reset display
  Serial.println("Resetting display...");
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(50);
  digitalWrite(OLED_RST, HIGH);
  delay(50);
  
  // Initialize display
  Serial.println("Initializing display...");
  display.init();
  
  Serial.println("Setting up display...");
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  
  // Draw test pattern
  Serial.println("Drawing test...");
  display.clear();
  display.drawString(64, 10, "Hello!");
  display.drawString(64, 30, "OLED Test");
  display.display();
  
  Serial.println("Test complete - check display!");
}

void loop() {
  static int counter = 0;
  
  delay(2000);
  
  display.clear();
  display.drawString(64, 20, "Count: " + String(counter++));
  display.display();
  
  Serial.println("Updated display: " + String(counter));
}
