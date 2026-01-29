/**
 * @file OTA.cpp
 * @brief OTA update module implementation
 */

#include "OTA.h"

OTA::OTA() : initialized(false), enabled(false) {
}

bool OTA::begin(const char* hostname, const char* password) {
    this->hostname = String(hostname);

    // Set hostname
    ArduinoOTA.setHostname(hostname);

    // Set password if provided
    if (password && strlen(password) > 0) {
        ArduinoOTA.setPassword(password);
    }

    // Configure OTA callbacks
    ArduinoOTA.onStart(onStart);
    ArduinoOTA.onEnd(onEnd);
    ArduinoOTA.onProgress(onProgress);
    ArduinoOTA.onError(onError);

    // Start OTA service
    ArduinoOTA.begin();

    initialized = true;
    enabled = true;
    Serial.println("OTA initialized successfully");
    return true;
}

void OTA::handle() {
    if (!initialized || !enabled) {
        return;
    }

    ArduinoOTA.handle();
}

bool OTA::connectWiFi(const char* ssid, const char* password, unsigned long timeout) {
    Serial.print("Connecting to WiFi: ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - startTime > timeout) {
            Serial.println("\nWiFi connection timeout");
            return false;
        }
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    return true;
}

bool OTA::isWiFiConnected() {
    return WiFi.status() == WL_CONNECTED;
}

void OTA::disconnectWiFi() {
    WiFi.disconnect();
    Serial.println("WiFi disconnected");
}

void OTA::enable() {
    enabled = true;
}

void OTA::disable() {
    enabled = false;
}

bool OTA::isEnabled() {
    return enabled;
}

// Static callback implementations
void OTA::onStart() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
    } else {
        type = "filesystem";
    }
    Serial.println("OTA: Start updating " + type);
}

void OTA::onEnd() {
    Serial.println("\nOTA: Update complete");
}

void OTA::onProgress(unsigned int progress, unsigned int total) {
    static unsigned int lastPercent = 0;
    unsigned int percent = (progress / (total / 100));
    
    if (percent != lastPercent && percent % 10 == 0) {
        Serial.printf("OTA Progress: %u%%\n", percent);
        lastPercent = percent;
    }
}

void OTA::onError(ota_error_t error) {
    Serial.printf("OTA Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
        Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
        Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
        Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
        Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
        Serial.println("End Failed");
    }
}
