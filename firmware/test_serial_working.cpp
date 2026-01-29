/**
 * Simple test to verify Serial and Display work
 */

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_SDA 17
#define OLED_SCL 18
#define OLED_RST 21
#define Vext 36
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

void setup() {
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n\n=== SERIAL TEST ===");
  Serial.println("Serial is working!");
  
  // Enable Vext
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
  delay(100);
  
  Serial.println("Vext enabled");
  
  // Init display
  Wire.begin(OLED_SDA, OLED_SCL);
  if (display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("Display OK");
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Serial Test");
    display.setCursor(0, 15);
    display.println("Check Serial");
    display.setCursor(0, 25);
    display.println("Monitor!");
    display.display();
  } else {
    Serial.println("Display FAILED");
  }
  
  Serial.println("Setup complete!");
}

void loop() {
  static int count = 0;
  Serial.print("Loop ");
  Serial.println(count++);
  
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Count: ");
  display.print(count);
  display.display();
  
  delay(1000);
}
