/**
 * B.R.A.V.O. Multi-Mode Test
 * 
 * Press PRG button to cycle through modes:
 * 1) RELAY - Receive LoRa packets
 * 2) BEACON - Transmit LoRa packets  
 * 3) GPS - Display GPS coordinates
 */

#include "LoRaComm.h"
#include "GPS.h"

// Undefine conflicting macros from Heltec library before including Adafruit
#ifdef BLACK
#undef BLACK
#endif
#ifdef WHITE
#undef WHITE
#endif

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED Display pins
#define OLED_SDA 17
#define OLED_SCL 18
#define OLED_RST 21
#define Vext 36

// Button pin
#define BUTTON_PIN 0  // PRG button

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);
LoRaComm lora;
GPS gpsModule;

// Mode system
enum SystemMode {
  MODE_RELAY = 0,
  MODE_BEACON = 1,
  MODE_GPS = 2
};

SystemMode currentMode = MODE_RELAY;
int packetCount = 0;
int receivedCount = 0;
int lastRSSI = 0;
unsigned long lastButtonPress = 0;
bool buttonPressed = false;

// Function declarations
void switchMode();
void runRelayMode();
void runBeaconMode();
void runGPSMode();

void setup() {
  // Initialize Serial IMMEDIATELY
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  delay(2000);  // Longer delay to ensure Serial is ready
  
  Serial.println("\n\n\n");
  Serial.println("======================");
  Serial.println("  B.R.A.V.O. LoRa Test");
  Serial.println("======================");
  Serial.flush();
  
  delay(500);
  
  Serial.println("Step 1: Enabling Vext...");
  Serial.flush();
  
  // Enable Vext power for OLED and LoRa
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);  // LOW = ON
  delay(100);
  Serial.println("Step 1: Vext enabled OK");
  Serial.flush();
  
  // Also enable LoRa power if separate pin exists
  // Some V3 boards need this
  pinMode(12, OUTPUT);  // RST pin - try enabling
  digitalWrite(12, HIGH);
  delay(10);
  digitalWrite(12, LOW);
  delay(10);
  digitalWrite(12, HIGH);
  delay(100);
  Serial.println("Step 1a: LoRa reset toggled");
  Serial.flush();
  
  // Setup button
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.println("Step 1b: Button configured");
  Serial.flush();
  
  Serial.println("Step 2: Initializing I2C and display...");
  Serial.flush();
  
  // Initialize I2C and display
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("Step 2: Display init failed!");
    Serial.flush();
    while (1); // Halt execution if display init fails
  } else {
    Serial.println("Step 2: Display initialized OK");
  }
  Serial.flush();
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("B.R.A.V.O. LoRa");
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
  display.setCursor(0, 15);
  display.println("Initializing...");
  display.display();
  
  // Initialize LoRa
  Serial.println("Initializing LoRa...");
  
  // Add delay before LoRa init
  delay(500);
  
  if (!lora.begin()) {
    Serial.println("LoRa init failed!");
    display.setCursor(0, 25);
    display.println("LoRa FAILED!");
    display.setCursor(0, 35);
    display.println("Check antenna!");
    display.display();
    while(1) {
      delay(1000);
    }
  }
  
  Serial.println("LoRa initialized!");
  
  // Initialize GPS
  Serial.println("Initializing GPS...");
  if (gpsModule.begin()) {
    Serial.println("GPS initialized!");
  } else {
    Serial.println("GPS init failed!");
  }
  
  // Show startup screen
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("B.R.A.V.O. System");
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
  display.setCursor(0, 20);
  display.println("Multi-Mode Ready");
  display.setCursor(0, 35);
  display.println("Press PRG button");
  display.setCursor(0, 45);
  display.println("to change mode");
  display.display();
  delay(3000);
  
  Serial.println("System ready! Press button to change mode.");
}

void switchMode() {
  currentMode = (SystemMode)((currentMode + 1) % 3);
  packetCount = 0;
  receivedCount = 0;
  
  Serial.print("Switching to mode: ");
  Serial.println(currentMode);
  
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 10);
  
  switch(currentMode) {
    case MODE_RELAY:
      display.println("RELAY");
      display.setTextSize(1);
      display.setCursor(30, 40);
      display.println("Receive Mode");
      Serial.println(">>> RELAY MODE <<<");
      break;
    case MODE_BEACON:
      display.println("BEACON");
      display.setTextSize(1);
      display.setCursor(25, 40);
      display.println("Transmit Mode");
      Serial.println(">>> BEACON MODE <<<");
      break;
    case MODE_GPS:
      display.println(" GPS");
      display.setTextSize(1);
      display.setCursor(25, 40);
      display.println("Location Mode");
      Serial.println(">>> GPS MODE <<<");
      break;
  }
  display.display();
  delay(1500);
  
  // Force immediate screen update for new mode
  display.clearDisplay();
  display.display();
}

void loop() {
  // Check for button press (with debounce)
  bool buttonState = digitalRead(BUTTON_PIN);
  
  if (buttonState == LOW && !buttonPressed && (millis() - lastButtonPress > 300)) {
    buttonPressed = true;
    lastButtonPress = millis();
    Serial.println("BUTTON PRESSED!");
    switchMode();
    delay(200); // Give time to release button
  }
  if (buttonState == HIGH && buttonPressed) {
    buttonPressed = false;
  }
  
  // Update GPS continuously
  gpsModule.update();
  
  // Run current mode
  switch(currentMode) {
    case MODE_RELAY:
      runRelayMode();
      break;
    case MODE_BEACON:
      runBeaconMode();
      break;
    case MODE_GPS:
      runGPSMode();
      break;
  }
  
  delay(10);
}

void runRelayMode() {
  // RELAY MODE - Receive LoRa packets
  if (lora.available()) {
    String message = lora.receiveMessage();
    lastRSSI = lora.getLastPacketRSSI();
    receivedCount++;
    
    Serial.print("Received [RSSI: ");
    Serial.print(lastRSSI);
    Serial.print(" dBm]: ");
    Serial.println(message);
    
    // Update display
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("RELAY");
    display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
    
    display.setCursor(0, 15);
    display.print("Rcvd: ");
    display.print(receivedCount);
    
    display.setCursor(0, 25);
    display.print("Last: ");
    if (message.length() > 11) {
      display.print(message.substring(0, 11));
    } else {
      display.print(message);
    }
    
    display.setCursor(0, 35);
    display.print("RSSI: ");
    display.print(lastRSSI);
    display.print(" dBm");
    
    display.setCursor(0, 45);
    display.print("SNR: ");
    display.print(lora.getLastPacketSNR());
    display.print(" dB");
    
    display.setCursor(0, 55);
    display.print("BTN=Switch");
    
    display.display();
  }
  
  // Update display every second even without packets
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate >= 1000) {
    lastUpdate = millis();
    
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("RELAY");
    display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
    
    display.setCursor(0, 15);
    display.print("Waiting...");
    
    display.setCursor(0, 25);
    display.print("Received: ");
    display.print(receivedCount);
    
    if (receivedCount > 0) {
      display.setCursor(0, 35);
      display.print("Last RSSI: ");
      display.print(lastRSSI);
      display.print("dBm");
    }
    
    display.setCursor(0, 55);
    display.print("BTN=Switch");
    
    display.display();
  }
}

void runBeaconMode() {
  // BEACON MODE - Transmit LoRa packets
  static unsigned long lastTx = 0;
  
  if (millis() - lastTx >= 2000) {  // Send every 2 seconds
    lastTx = millis();
    packetCount++;
    
    String message = "BRAVO_PKT_" + String(packetCount);
    
    Serial.print("Sending: ");
    Serial.println(message);
    
    bool success = lora.sendMessage(message);
    
    // Update display
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("BEACON");
    display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
    
    display.setCursor(0, 15);
    display.print("Sent: ");
    display.print(packetCount);
    
    display.setCursor(0, 25);
    display.print("Last: PKT_");
    display.print(packetCount);
    
    display.setCursor(0, 35);
    display.print("Status: ");
    display.print(success ? "OK" : "FAIL");
    
    display.setCursor(0, 45);
    display.print("Time: ");
    display.print(millis() / 1000);
    display.print("s");
    
    display.setCursor(0, 55);
    display.print("BTN=Switch");
    
    display.display();
  }
}

void runGPSMode() {
  // GPS MODE - Display GPS coordinates
  static unsigned long lastUpdate = 0;
  
  if (millis() - lastUpdate >= 1000) {
    lastUpdate = millis();
    
    double lat, lon;
    bool hasLocation = gpsModule.getLocation(lat, lon);
    uint8_t sats = gpsModule.getSatellites();
    
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    
    // Header
    display.setCursor(0, 0);
    display.println("GPS MODE");
    display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
    
    // GPS Status
    display.setCursor(0, 13);
    display.print("Sats: ");
    display.print(sats);
    display.print(" | ");
    display.print(hasLocation ? "LOCK" : "SEARCH");
    
    // Coordinates
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
    
    // Status info
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
    
    display.setCursor(0, 55);
    display.print("BTN=Switch");
    
    display.display();
    
    if (hasLocation) {
      Serial.printf("GPS: %.6f, %.6f | Sats: %d\n", lat, lon, sats);
    } else {
      Serial.printf("GPS: Searching | Sats: %d\n", sats);
    }
  }
}
