/**
 * @file Display.cpp
 * @brief OLED Display module implementation using Adafruit SSD1306
 */

#include "Display.h"

Display::Display() : display(nullptr), initialized(false), lastUpdate(0) {
}

bool Display::begin() {
    Serial.println("Display: Initializing with Adafruit SSD1306...");
    
    // Enable Vext power for OLED (critical for Heltec V3!)
    pinMode(Vext, OUTPUT);
    digitalWrite(Vext, LOW);  // LOW = ON for Vext
    delay(100);  // Give power time to stabilize
    Serial.println("Display: Vext power enabled");
    
    // Initialize I2C with Heltec V3 pins
    Wire.begin(OLED_SDA, OLED_SCL);
    
    // Create display object
    display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);
    
    // Try primary address
    if(!display->begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        Serial.println("Display: Failed at 0x3C, trying 0x3D...");
        // Try alternate address
        if(!display->begin(SSD1306_SWITCHCAPVCC, 0x3D)) {
            Serial.println("Display: Initialization failed!");
            initialized = false;
            return false;
        }
    }
    
    initialized = true;
    Serial.println("Display: Initialized successfully");
    
    // Clear and show startup message
    display->clearDisplay();
    display->setTextSize(1);
    display->setTextColor(SSD1306_WHITE);
    display->setCursor(0, 0);
    display->println("B.R.A.V.O.");
    display->println("Display Ready");
    display->display();
    
    return true;
}

void Display::clear() {
    if (!initialized || !display) return;
    display->clearDisplay();
}

void Display::updateStatus(const GPSData& gpsData, const char* deviceId, 
                          const char* deviceType, uint8_t battery, bool bleConnected) {
    if (!initialized || !display || !shouldUpdate()) return;
    
    display->clearDisplay();
    display->setTextSize(1);
    display->setTextColor(SSD1306_WHITE);
    
    // Line 1: Device info
    display->setCursor(0, 0);
    display->print(deviceId);
    
    // Line 2: Device type and battery
    display->setCursor(0, 10);
    display->print(deviceType);
    display->print(" | Bat:");
    display->print(battery);
    display->print("%");
    
    // Line 3: GPS Status
    display->setCursor(0, 20);
    if (gpsData.valid) {
        display->print("GPS: FIX");
        
        // Line 4: Latitude
        display->setCursor(0, 30);
        display->print("Lat:");
        display->print(gpsData.latitude, 6);
        
        // Line 5: Longitude
        display->setCursor(0, 40);
        display->print("Lon:");
        display->print(gpsData.longitude, 6);
        
        // Line 6: Satellites
        display->setCursor(0, 50);
        display->print("Sats:");
        display->print(gpsData.satellites);
    } else {
        display->print("GPS: Searching...");
        display->setCursor(0, 30);
        display->print("Sats: ");
        display->print(gpsData.satellites);
    }
    
    display->display();
    lastUpdate = millis();
}

void Display::showGPS(const GPSData& gpsData) {
    if (!initialized || !display) return;
    
    display->clearDisplay();
    display->setTextSize(1);
    display->setTextColor(SSD1306_WHITE);
    
    if (gpsData.valid) {
        display->setCursor(0, 0);
        display->println("GPS Location:");
        display->print("Lat:");
        display->println(gpsData.latitude, 6);
        display->print("Lon:");
        display->println(gpsData.longitude, 6);
        display->print("Alt:");
        display->print(gpsData.altitude, 1);
        display->println("m");
        display->print("Sats:");
        display->print(gpsData.satellites);
    } else {
        display->setCursor(0, 0);
        display->println("GPS: No Fix");
        display->print("Satellites: ");
        display->println(gpsData.satellites);
        display->println("Searching...");
    }
    
    display->display();
}

void Display::showDeviceInfo(const char* deviceId, const char* deviceType) {
    if (!initialized || !display) return;
    
    display->clearDisplay();
    display->setTextSize(2);
    display->setTextColor(SSD1306_WHITE);
    
    display->setCursor(10, 5);
    display->println("B.R.A.V.O.");
    display->setTextSize(1);
    display->setCursor(0, 30);
    display->println(deviceType);
    display->println(deviceId);
    display->display();
}

void Display::showInitStatus(const char* module, bool success) {
    if (!initialized || !display) return;
    
    display->clearDisplay();
    display->setTextSize(1);
    display->setTextColor(SSD1306_WHITE);
    display->setCursor(0, 20);
    display->print(module);
    display->print(": ");
    display->println(success ? "OK" : "FAIL");
    display->display();
    delay(500);
}

void Display::showMessage(const char* message) {
    if (!initialized || !display) return;
    
    display->clearDisplay();
    display->setTextSize(1);
    display->setTextColor(SSD1306_WHITE);
    display->setCursor(0, 20);
    display->println(message);
    display->display();
}

bool Display::shouldUpdate() {
    return (millis() - lastUpdate >= DISPLAY_UPDATE_INTERVAL);
}
