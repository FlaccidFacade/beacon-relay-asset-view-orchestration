/**
 * @file PinConfig.h
 * @brief Raspberry Pi Pico W pin assignments for B.R.A.V.O. firmware
 *
 * POWER SUPPLY (external, NOT from Pico):
 *   - Connect 5V to VSYS (pin 39 or 40 — either works, same rail)
 *   - Connect GND to any GND pin (3, 8, 13, 18, 23, 28, 33, or 38)
 *   - Pico's onboard regulator converts 5V to 3.3V
 *   - Pin 36 outputs max ~150 mA @ 3.3V (enough for GPS + OLED + LoRa)
 *   - Options: USB 5V adapter, 4x AA batteries (6V), or LiPo + regulator
 *
 * PERIPHERAL CONNECTIONS (all use 3.3V from pin 36 and GND):
 *   UART0 (GP0 TX, GP1 RX)   -> Debug serial monitor  (115200 baud, Serial1)
 *                               GND reference: physical pin 3
 *   I2C0  (GP4 SDA, GP5 SCL) -> SSD1306 OLED          (128x64, I2C addr 0x3C)
 *   SerialPIO (GP8 TX, GP9 RX)   -> RYLR896 RXD/TXD   (115200 baud AT commands)
 *   SerialPIO (GP12 TX, GP13 RX) -> GPS NEO-7m RXD/TXD (9600 baud NMEA)
 *   GP14                      -> RYLR896 NRESET        (active LOW reset)
 *   GP15                      -> GPS PPS               (1 Hz rising edge)
 *   GP22                      -> Push-button           (active LOW, INPUT_PULLUP)
 *
 * Wi-Fi / Bluetooth (CYW43439) is onboard the Pico W module itself — it
 * talks to the RP2040 over an internal SPI bus, not the GPIO header, so it
 * needs no external wiring or pin assignment here.
 */

#pragma once

// ── Debug UART (UART0 / Serial1) ──────────────────────────────────────────
#define PIN_DEBUG_TX    0    // GP0  → debug monitor TXD  (UART0 TX)
#define PIN_DEBUG_RX    1    // GP1  ← debug monitor RXD  (UART0 RX)

// ── LoRa RYLR896 (SerialPIO — GP8/GP9, physical pins 11/12; ─────────────
//    SerialPIO is used so UART0 remains free for debug on GP0/GP1)  ─────────
#define PIN_LORA_TX     8    // GP8  (physical pin 11) → RYLR896 RXD   (SerialPIO TX)
#define PIN_LORA_RX     9    // GP9  (physical pin 12) ← RYLR896 TXD   (SerialPIO RX)
#define PIN_LORA_RESET  14   // GP14 → RYLR896 NRESET (active LOW pulse)

// ── OLED SSD1306 128×64 (I2C0) ────────────────────────────────────────────
#define PIN_OLED_SDA    4    // GP4  ↔ OLED SDA
#define PIN_OLED_SCL    5    // GP5  → OLED SCL
#define OLED_I2C_ADDR   0x3C
#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   64
// No hardware reset pin on this OLED; pass -1 to Adafruit_SSD1306 constructor

// ── GPS NEO-7m (SerialPIO — GP12/GP13, physical pins 16/17; ──────────────
//    hardware UART1 isn't wired here, so GPS also uses software SerialPIO)
#define PIN_GPS_TX      12   // GP12 (physical pin 16) → NEO-7m RXD  (SerialPIO TX)
#define PIN_GPS_RX      13   // GP13 (physical pin 17) ← NEO-7m TXD  (SerialPIO RX)
#define PIN_GPS_PPS     15   // GP15 ← NEO-7m PPS    (1 Hz rising edge)
#define GPS_BAUD        9600

// ── User Input ────────────────────────────────────────────────────────────
#define PIN_BUTTON      22   // GP22 — momentary push-button, active LOW
                             // Wire: GP22 → button → GND  (INPUT_PULLUP)

// ── LoRa RF settings ──────────────────────────────────────────────────────
#define LORA_BAUD       115200
#define LORA_FREQ_HZ    915000000   // 915 MHz (North America)
#define LORA_NETWORK_ID 6
// AT+PARAMETER=<SF>,<BW>,<CR>,<PP>
//   SF=9, BW=7 (125 kHz), CR=1 (4/5), Preamble=12
#define LORA_PARAM_SF   9
#define LORA_PARAM_BW   7
#define LORA_PARAM_CR   1
#define LORA_PARAM_PP   12

// ── Pico W Power Rails ─────────────────────────────────────────────────────
// NOTE: Pico W receives 5V externally via VSYS pins (39, 40).
// Internal regulator outputs 3.3V on pin 36 for peripherals.
// DO NOT attempt to power the Pico from pin 36 — it is an OUTPUT only.
#define VSYS_PIN_1      39   // Pin 39: VSYS input (5V from external supply)
#define VSYS_PIN_2      40   // Pin 40: VSYS input (5V from external supply)
#define V33_OUT_PIN     36   // Pin 36: 3.3V regulated output (max ~150 mA)
                             // Use this to power: GPS, OLED, LoRa module
// GND pins available: 3, 8, 13, 18, 23, 28, 33, 38 (any one works)
