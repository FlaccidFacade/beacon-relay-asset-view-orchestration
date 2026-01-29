/**
 * B.R.A.V.O. Device Firmware
 * 
 * Single firmware instance that can be loaded to multiple devices.
 * Records GPS info on screen and communicates with other devices via LoRa.
 * 
 * Press PRG button to cycle through display pages:
 * 1) GPS Location - Current GPS coordinates and satellite info
 * 2) Communication - LoRa packet transmission/reception status
 * 3) Device Info - Device ID and system information
 * 4) Combined View - GPS + Communication status together
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

// Device Configuration - Set unique ID for each device
#define DEVICE_ID "BRAVO_001"  // Change to BRAVO_002, etc. for other devices

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);
LoRaComm lora;
GPS gpsModule;

// Multi-page display system
enum DisplayPage {
  PAGE_GPS = 0,
  PAGE_COMMUNICATION = 1,
  PAGE_DEVICE_INFO = 2,
  PAGE_COMBINED = 3,
  PAGE_COUNT = 4  // Total number of pages
};

DisplayPage currentPage = PAGE_GPS;
int packetCount = 0;
int receivedCount = 0;
int lastRSSI = 0;
float lastSNR = 0;
String lastReceivedMessage = "";
unsigned long lastButtonPress = 0;
bool buttonPressed = false;
unsigned long lastTxTime = 0;
unsigned long lastRxTime = 0;

// Function declarations
void switchPage();
void updateDisplay();
void showGPSPage();
void showCommunicationPage();
void showDeviceInfoPage();
void showCombinedPage();
void sendLoRaPacket();
void receiveLoRaPacket();

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
  display.println("B.R.A.V.O. Device");
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
  display.setCursor(0, 20);
  display.print("ID: ");
  display.println(DEVICE_ID);
  display.setCursor(0, 35);
  display.println("GPS + LoRa Ready");
  display.setCursor(0, 45);
  display.println("Press PRG: Pages");
  display.display();
  delay(3000);
  
  Serial.print("Device ");
  Serial.print(DEVICE_ID);
  Serial.println(" ready! Press button to change page.");
}

void switchPage() {
  currentPage = (DisplayPage)((currentPage + 1) % PAGE_COUNT);
  
  Serial.print("Switching to page: ");
  Serial.println(currentPage);
  
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  
  switch(currentPage) {
    case PAGE_GPS:
      display.println("GPS");
      display.setTextSize(1);
      display.setCursor(20, 40);
      display.println("Location Data");
      Serial.println(">>> GPS PAGE <<<");
      break;
    case PAGE_COMMUNICATION:
      display.println("COMM");
      display.setTextSize(1);
      display.setCursor(15, 40);
      display.println("LoRa Status");
      Serial.println(">>> COMMUNICATION PAGE <<<");
      break;
    case PAGE_DEVICE_INFO:
      display.println("INFO");
      display.setTextSize(1);
      display.setCursor(15, 40);
      display.println("Device Info");
      Serial.println(">>> DEVICE INFO PAGE <<<");
      break;
    case PAGE_COMBINED:
      display.println("ALL");
      display.setTextSize(1);
      display.setCursor(10, 40);
      display.println("Combined View");
      Serial.println(">>> COMBINED PAGE <<<");
      break;
    default:
      break;
  }
  display.display();
  delay(1000);
  
  // Force immediate screen update for new page
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
    switchPage();
    delay(200); // Give time to release button
  }
  if (buttonState == HIGH && buttonPressed) {
    buttonPressed = false;
  }
  
  // Update GPS continuously
  gpsModule.update();
  
  // Handle LoRa communication (both send and receive)
  sendLoRaPacket();
  receiveLoRaPacket();
  
  // Update display based on current page
  updateDisplay();
  
  delay(10);
}

void updateDisplay() {
  static unsigned long lastDisplayUpdate = 0;
  
  // Update display every 500ms
  if (millis() - lastDisplayUpdate < 500) {
    return;
  }
  lastDisplayUpdate = millis();
  
  switch(currentPage) {
    case PAGE_GPS:
      showGPSPage();
      break;
    case PAGE_COMMUNICATION:
      showCommunicationPage();
      break;
    case PAGE_DEVICE_INFO:
      showDeviceInfoPage();
      break;
    case PAGE_COMBINED:
      showCombinedPage();
      break;
  }
}

void showGPSPage() {
  double lat, lon;
  bool hasLocation = gpsModule.getLocation(lat, lon);
  uint8_t sats = gpsModule.getSatellites();
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  // Header
  display.setCursor(0, 0);
  display.print("GPS | Sats:");
  display.print(sats);
  display.print(" ");
  display.print(hasLocation ? "LOCK" : "SRCH");
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
  
  // Coordinates
  display.setCursor(0, 13);
  display.print("Lat:");
  if (hasLocation) {
    display.print(lat, 6);
  } else {
    display.print(" Searching...");
  }
  
  display.setCursor(0, 23);
  display.print("Lon:");
  if (hasLocation) {
    display.print(lon, 6);
  } else {
    display.print(" Searching...");
  }
  
  // Additional info
  display.setCursor(0, 33);
  if (hasLocation) {
    double alt = gpsModule.getAltitude();
    float speed = gpsModule.getSpeed();
    display.print("Alt:");
    display.print(alt, 1);
    display.print("m");
  } else {
    display.print("Chars:");
    display.print(gpsModule.getCharsProcessed());
  }
  
  display.setCursor(0, 43);
  if (hasLocation) {
    float speed = gpsModule.getSpeed();
    display.print("Spd:");
    display.print(speed, 1);
    display.print("km/h");
  }
  
  display.setCursor(0, 55);
  display.print("BTN=Next Page");
  
  display.display();
}

void showCommunicationPage() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  // Header
  display.setCursor(0, 0);
  display.println("LoRa Communication");
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
  
  // Transmission stats
  display.setCursor(0, 13);
  display.print("Sent: ");
  display.print(packetCount);
  display.print(" pkts");
  
  display.setCursor(0, 23);
  display.print("Rcvd: ");
  display.print(receivedCount);
  display.print(" pkts");
  
  // Last received info
  if (receivedCount > 0) {
    display.setCursor(0, 33);
    display.print("RSSI: ");
    display.print(lastRSSI);
    display.print("dBm");
    
    display.setCursor(0, 43);
    display.print("SNR: ");
    display.print(lastSNR, 1);
    display.print("dB");
  } else {
    display.setCursor(0, 33);
    display.print("Listening...");
  }
  
  display.setCursor(0, 55);
  display.print("BTN=Next Page");
  
  display.display();
}

void showDeviceInfoPage() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  // Header
  display.setCursor(0, 0);
  display.println("Device Information");
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE);
  
  // Device ID
  display.setCursor(0, 13);
  display.print("ID: ");
  display.println(DEVICE_ID);
  
  // Uptime
  display.setCursor(0, 23);
  display.print("Uptime: ");
  unsigned long uptime = millis() / 1000;
  display.print(uptime);
  display.print("s");
  
  // Module status
  display.setCursor(0, 33);
  display.print("GPS: ");
  display.print(gpsModule.hasFix() ? "Active" : "Search");
  
  display.setCursor(0, 43);
  display.print("LoRa: Active");
  
  display.setCursor(0, 55);
  display.print("BTN=Next Page");
  
  display.display();
}

void showCombinedPage() {
  double lat, lon;
  bool hasLocation = gpsModule.getLocation(lat, lon);
  uint8_t sats = gpsModule.getSatellites();
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  
  // Header with device ID
  display.setCursor(0, 0);
  display.print(DEVICE_ID);
  display.drawLine(0, 8, 128, 8, SSD1306_WHITE);
  
  // GPS Status (compact)
  display.setCursor(0, 10);
  display.print("GPS:");
  if (hasLocation) {
    display.print(lat, 4);
    display.print(",");
    display.print(lon, 4);
  } else {
    display.print("Searching(");
    display.print(sats);
    display.print(")");
  }
  
  // LoRa TX Status
  display.setCursor(0, 20);
  display.print("TX: ");
  display.print(packetCount);
  display.print(" pkts");
  
  // LoRa RX Status
  display.setCursor(0, 30);
  display.print("RX: ");
  display.print(receivedCount);
  if (receivedCount > 0) {
    display.print(" (");
    display.print(lastRSSI);
    display.print("dBm)");
  } else {
    display.print(" pkts");
  }
  
  // Last activity
  display.setCursor(0, 40);
  unsigned long now = millis();
  if (lastTxTime > 0) {
    display.print("LstTx:");
    display.print((now - lastTxTime) / 1000);
    display.print("s");
  }
  
  display.setCursor(0, 50);
  if (lastRxTime > 0) {
    display.print("LstRx:");
    display.print((now - lastRxTime) / 1000);
    display.print("s");
  }
  
  display.display();
}

void sendLoRaPacket() {
  static unsigned long lastTx = 0;
  
  // Send packet every 3 seconds
  if (millis() - lastTx >= 3000) {
    lastTx = millis();
    packetCount++;
    
    // Create packet with device ID and GPS data
    String message = String(DEVICE_ID) + "_PKT_" + String(packetCount);
    
    double lat, lon;
    if (gpsModule.getLocation(lat, lon)) {
      message += "_GPS:" + String(lat, 6) + "," + String(lon, 6);
    }
    
    Serial.print("Sending: ");
    Serial.println(message);
    
    bool success = lora.sendMessage(message);
    
    if (success) {
      lastTxTime = millis();
      Serial.println("Packet sent successfully");
    } else {
      Serial.println("Packet send failed");
    }
  }
}

void receiveLoRaPacket() {
  // Check for incoming LoRa packets
  if (lora.available()) {
    String message = lora.receiveMessage();
    lastRSSI = lora.getLastPacketRSSI();
    lastSNR = lora.getLastPacketSNR();
    lastReceivedMessage = message;
    receivedCount++;
    lastRxTime = millis();
    
    Serial.print("Received [RSSI: ");
    Serial.print(lastRSSI);
    Serial.print(" dBm, SNR: ");
    Serial.print(lastSNR);
    Serial.print(" dB]: ");
    Serial.println(message);
  }
}
