package com.bravo.mobile.utils;

/**
 * Application-wide constants
 */
public class Constants {
    // BLE Constants
    public static final String ESP32_DEVICE_NAME_PREFIX = "ESP32";
    public static final int BLE_SCAN_TIMEOUT = 10000; // 10 seconds
    
    // LoRa Constants
    public static final int LORA_FREQUENCY = 915000000; // 915 MHz (US)
    public static final int LORA_SPREADING_FACTOR = 7;
    public static final int LORA_BANDWIDTH = 125000; // 125 kHz
    
    // USB Constants
    public static final int USB_BAUD_RATE = 115200;
    public static final int USB_READ_TIMEOUT = 1000; // 1 second
    
    // Map Constants
    public static final double DEFAULT_LATITUDE = 37.7749;
    public static final double DEFAULT_LONGITUDE = -122.4194;
    public static final double DEFAULT_ZOOM = 15.0;
    
    // Telemetry Constants
    public static final int MAX_TELEMETRY_HISTORY = 1000; // Keep last 1000 points
    public static final int TELEMETRY_UPDATE_INTERVAL = 1000; // 1 second
}
