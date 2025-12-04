---
title: "B.R.A.V.O. MVP Hardware BOM"
sidebar_label: "Hardware BOM"
---

# B.R.A.V.O. MVP Hardware BOM

This document lists the **Bill of Materials** for the B.R.A.V.O. prototype, including the **collar** and **dongle** for breadboard testing and development.

For more details, see the [firmware](../firmware/) and [web dashboard](../web/) directories in this monorepo.

---

## Core Components

| Component     | Example Part                  | Purpose                | Notes                         |
| ------------- | ----------------------------- | ---------------------- | ----------------------------- |
| ESP32 LoRa V3 | YELUFT ESP32 LoRa V3          | MCU + LoRa transceiver | Already purchased             |
| GPS Module    | GT-U7 GPS (NEO-6M compatible) | Geolocation            | Included with ESP32 kit       |
| LoRa Antenna  | 915 MHz SMA                   | Long-range comms       | Must match regional frequency |

---

## Power & Regulation

| Component           | Example Part            | Purpose              | Notes                            |
| ------------------- | ----------------------- | -------------------- | -------------------------------- |
| Lithium-Ion Battery | 3.7V 1200–2000 mAh LiPo | Main power           | JST connector preferred          |
| Battery Charger     | TP4056 Li-Ion module    | Charge via USB       | Micro-USB or Type-C              |
| Step-up Converter   | MT3608 boost converter  | Raise voltage to 5 V | For 5V peripherals               |
| Slide Switch        | SPDT mini toggle        | Power control        | Inline between battery and board |

---

## Breadboard & Wiring

| Component             | Example Part  | Purpose               | Notes                                |
| --------------------- | ------------- | --------------------- | ------------------------------------ |
| Solderless Breadboard | 830-point     | Prototyping           | Full-size recommended                |
| Jumper Wires          | M-M, F-F, M-F | Connections           | 20–30 pack                           |
| Pin Headers           | 2.54 mm       | For ESP32/GPS modules | Breakaway headers for breadboard use |

---

## Dongle & Connectivity

| Component           | Example Part                 | Purpose            | Notes                          |
| ------------------- | ---------------------------- | ------------------ | ------------------------------ |
| USB-to-UART Adapter | CP2102 / CH340G              | Direct flash/debug | Use if ESP32 USB unstable      |
| LoRa USB Dongle     | Ebyte E32-915T30D or Dragino | Gateway for PC/RPi | Acts as temporary LoRa gateway |
| USB Hub (optional)  | Powered 4-port USB           | Expand peripherals | Useful for multiple dongles    |

---

## Optional Modules

| Component           | Example Part       | Purpose                | Notes                    |
| ------------------- | ------------------ | ---------------------- | ------------------------ |
| OLED Display        | 0.96" I2C SSD1306  | Telemetry / status     | Connect to ESP32 I2C     |
| Buzzer / LED        | 5V piezo / RGB LED | Alerts / status        | Optional                 |
| IMU / Temp Sensor   | MPU6050 / BME280   | Motion & environmental | I2C                      |
| MicroSD Card Module | SPI reader         | Data logging           | Optional offline storage |

---

## Debugging & Testing

| Component          | Purpose                     | Notes                               |
| ------------------ | --------------------------- | ----------------------------------- |
| Multimeter         | Voltage & continuity checks | Essential                           |
| USB Logic Analyzer | Serial & I2C debugging      | Optional, helpful for timing issues |

---

## Estimated Cost

| Category              | Estimated Cost |
| --------------------- | -------------- |
| Core (ESP32 + GPS)    | $25–35         |
| Power System          | $10–15         |
| Breadboard & Wiring   | $10            |
| Dongle & Connectivity | $20–30         |
| Optional Modules      | $10–20         |
| **Total**             | **$65–100**    |

---

### Notes

- **Power Design:** ESP32 + GPS peaks at ~250–350 mA; LoRa peaks up to ~500 mA. LTE (for future expansion) may require up to ~800 mA.
- **Breadboard:** Keep jumper wires neat and label connections for firmware testing.
- **Dongle:** Use a powered USB hub if testing multiple units simultaneously.
- **Expansion:** The BOM supports adding LTE/5G modules and additional sensors for v1.0.0.
