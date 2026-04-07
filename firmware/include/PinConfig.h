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
 *   UART0 (GP0 TX, GP1 RX)  -> RYLR896 RXD/TXD     (115200 baud AT commands)
 *   I2C0  (GP4 SDA, GP5 SCL) -> SSD1306 OLED        (128x64, I2C addr 0x3C)
 *   UART1 (GP8 TX, GP9 RX)  -> GPS NEO-7m          (9600 baud NMEA)
 *   GP14                     -> RYLR896 NRESET      (active LOW reset)
 *   GP15                     -> GPS PPS              (1 Hz rising edge)
 */

#pragma once

// ── LoRa RYLR896 (UART0) ───────────────────────────────────────────────────
#define PIN_LORA_TX     0    // GP0  → RYLR896 RXD   (UART0 TX)
#define PIN_LORA_RX     1    // GP1  ← RYLR896 TXD   (UART0 RX)
#define PIN_LORA_RESET  14   // GP14 → RYLR896 NRESET (active LOW pulse)

// ── OLED SSD1306 128×64 (I2C0) ────────────────────────────────────────────
#define PIN_OLED_SDA    4    // GP4  ↔ OLED SDA
#define PIN_OLED_SCL    5    // GP5  → OLED SCL
#define OLED_I2C_ADDR   0x3C
#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   64
// No hardware reset pin on this OLED; pass -1 to Adafruit_SSD1306 constructor

// ── GPS NEO-7m (UART1) ────────────────────────────────────────────────────
#define PIN_GPS_TX      8    // GP8  → NEO-7m RXD    (UART1 TX)
#define PIN_GPS_RX      9    // GP9  ← NEO-7m TXD    (UART1 RX)
#define PIN_GPS_PPS     15   // GP15 ← NEO-7m PPS    (1 Hz rising edge)
#define GPS_BAUD        9600

// ── User Input ────────────────────────────────────────────────────────────
#define PIN_BUTTON      16   // GP16 — momentary push-button, active LOW
                             // Wire: GP16 → button → GND  (INPUT_PULLUP)

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
