package com.bravo.mobile.libs;

import com.bravo.mobile.models.LoRaPacket;

import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;

/**
 * Handles receiving and parsing LoRa packets from USB or BLE connection
 * This is a placeholder implementation that will be connected to actual hardware
 */
public class LoRaReceiver {
    private InputStream inputStream;
    private List<LoRaPacketListener> listeners;
    private boolean isRunning;
    private Thread receiverThread;

    public interface LoRaPacketListener {
        void onPacketReceived(LoRaPacket packet);
        void onError(String error);
    }

    public LoRaReceiver() {
        this.listeners = new ArrayList<>();
        this.isRunning = false;
    }

    /**
     * Start receiving LoRa packets from the input stream
     * @param inputStream Stream from USB or BLE connection
     */
    public void startReceiving(InputStream inputStream) {
        this.inputStream = inputStream;
        this.isRunning = true;

        receiverThread = new Thread(new Runnable() {
            @Override
            public void run() {
                receiveLoop();
            }
        });
        receiverThread.start();
    }

    /**
     * Stop receiving packets
     */
    public void stopReceiving() {
        isRunning = false;
        if (receiverThread != null) {
            receiverThread.interrupt();
        }
    }

    /**
     * Main receive loop - reads from input stream and parses packets
     */
    private void receiveLoop() {
        byte[] buffer = new byte[256];
        
        while (isRunning) {
            try {
                if (inputStream != null && inputStream.available() > 0) {
                    int bytesRead = inputStream.read(buffer);
                    if (bytesRead > 0) {
                        // Parse the received data
                        LoRaPacket packet = parsePacket(buffer, bytesRead);
                        if (packet != null) {
                            notifyListeners(packet);
                        }
                    }
                }
                Thread.sleep(100); // Prevent busy waiting
            } catch (Exception e) {
                if (isRunning) {
                    notifyError(e.getMessage());
                }
            }
        }
    }

    /**
     * Parse raw bytes into a LoRa packet
     * Format: [HEADER][RSSI][SNR][DATA_LENGTH][DATA]
     */
    private LoRaPacket parsePacket(byte[] buffer, int length) {
        try {
            // Simple packet format parsing
            // Header (0xAA 0x55) - 2 bytes
            // RSSI - 1 byte (signed)
            // SNR - 1 byte (signed)
            // Data length - 1 byte
            // Data - variable length
            
            if (length < 5) {
                return null; // Insufficient data
            }

            // Check header
            if (buffer[0] != (byte)0xAA || buffer[1] != (byte)0x55) {
                return null; // Invalid header
            }

            int rssi = buffer[2];
            int snr = buffer[3];
            int dataLength = buffer[4] & 0xFF;

            if (length < 5 + dataLength) {
                return null; // Incomplete packet
            }

            byte[] data = new byte[dataLength];
            System.arraycopy(buffer, 5, data, 0, dataLength);

            LoRaPacket packet = new LoRaPacket(data, rssi, snr);
            return packet;
        } catch (Exception e) {
            return null;
        }
    }

    /**
     * Add a listener for packet events
     */
    public void addListener(LoRaPacketListener listener) {
        if (!listeners.contains(listener)) {
            listeners.add(listener);
        }
    }

    /**
     * Remove a listener
     */
    public void removeListener(LoRaPacketListener listener) {
        listeners.remove(listener);
    }

    /**
     * Notify all listeners of a received packet
     */
    private void notifyListeners(LoRaPacket packet) {
        for (LoRaPacketListener listener : listeners) {
            listener.onPacketReceived(packet);
        }
    }

    /**
     * Notify all listeners of an error
     */
    private void notifyError(String error) {
        for (LoRaPacketListener listener : listeners) {
            listener.onError(error);
        }
    }
}
