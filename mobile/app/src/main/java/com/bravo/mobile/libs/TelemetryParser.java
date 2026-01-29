package com.bravo.mobile.libs;

import com.bravo.mobile.models.LoRaPacket;
import com.bravo.mobile.models.TelemetryData;
import com.google.gson.Gson;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Date;

/**
 * Parser for converting LoRa packets into TelemetryData
 * Supports both JSON and binary packet formats
 */
public class TelemetryParser {
    private static final Gson gson = new Gson();

    /**
     * Parse a LoRa packet into telemetry data
     * @param packet The raw LoRa packet
     * @return TelemetryData object or null if parsing fails
     */
    public static TelemetryData parse(LoRaPacket packet) {
        if (packet == null || packet.getRawData() == null) {
            return null;
        }

        try {
            // Try parsing as JSON first
            String jsonString = new String(packet.getRawData());
            if (jsonString.startsWith("{")) {
                return parseJson(jsonString, packet.getRssi(), packet.getSnr());
            } else {
                // Parse as binary format
                return parseBinary(packet.getRawData(), packet.getRssi(), packet.getSnr());
            }
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }

    /**
     * Parse JSON formatted telemetry
     * Example: {"lat":37.7749,"lon":-122.4194,"alt":50,"spd":10.5,"dev":"ESP32-001"}
     */
    private static TelemetryData parseJson(String json, int rssi, int snr) {
        try {
            JsonObject jsonObject = JsonParser.parseString(json).getAsJsonObject();
            TelemetryData data = new TelemetryData();
            
            if (jsonObject.has("lat")) {
                data.setLatitude(jsonObject.get("lat").getAsDouble());
            }
            if (jsonObject.has("lon")) {
                data.setLongitude(jsonObject.get("lon").getAsDouble());
            }
            if (jsonObject.has("alt")) {
                data.setAltitude(jsonObject.get("alt").getAsDouble());
            }
            if (jsonObject.has("spd")) {
                data.setSpeed(jsonObject.get("spd").getAsFloat());
            }
            if (jsonObject.has("dev")) {
                data.setDeviceId(jsonObject.get("dev").getAsString());
            }
            if (jsonObject.has("bat")) {
                data.setBatteryLevel(jsonObject.get("bat").getAsInt());
            }
            
            data.setRssi(rssi);
            data.setSnr(snr);
            data.setTimestamp(new Date());
            
            return data;
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }

    /**
     * Parse binary formatted telemetry
     * Binary format: [4 bytes lat][4 bytes lon][4 bytes alt][4 bytes spd][1 byte bat]
     */
    private static TelemetryData parseBinary(byte[] data, int rssi, int snr) {
        if (data.length < 17) {
            return null; // Insufficient data
        }

        try {
            ByteBuffer buffer = ByteBuffer.wrap(data);
            buffer.order(ByteOrder.LITTLE_ENDIAN);
            
            TelemetryData telemetry = new TelemetryData();
            telemetry.setLatitude(buffer.getFloat());
            telemetry.setLongitude(buffer.getFloat());
            telemetry.setAltitude(buffer.getFloat());
            telemetry.setSpeed(buffer.getFloat());
            telemetry.setBatteryLevel(buffer.get() & 0xFF);
            telemetry.setRssi(rssi);
            telemetry.setSnr(snr);
            telemetry.setTimestamp(new Date());
            
            return telemetry;
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }

    /**
     * Validate if telemetry data has valid GPS coordinates
     */
    public static boolean isValidGPS(TelemetryData data) {
        if (data == null) {
            return false;
        }
        return data.getLatitude() >= -90 && data.getLatitude() <= 90 &&
               data.getLongitude() >= -180 && data.getLongitude() <= 180;
    }
}
