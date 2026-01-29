package com.bravo.mobile.services;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.Service;
import android.content.Intent;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.os.Binder;
import android.os.Build;
import android.os.IBinder;

import androidx.core.app.NotificationCompat;

import com.bravo.mobile.libs.LoRaReceiver;
import com.bravo.mobile.libs.TelemetryParser;
import com.bravo.mobile.models.LoRaPacket;
import com.bravo.mobile.models.TelemetryData;
import com.hoho.android.usbserial.driver.UsbSerialDriver;
import com.hoho.android.usbserial.driver.UsbSerialPort;
import com.hoho.android.usbserial.driver.UsbSerialProber;

import java.io.IOException;
import java.io.InputStream;
import java.util.List;

/**
 * Foreground service for receiving LoRa telemetry via USB connection to ESP32
 * Manages USB serial connection and data parsing
 */
public class LoRaReceiverService extends Service {
    private static final String CHANNEL_ID = "LoRaReceiverChannel";
    private static final int NOTIFICATION_ID = 2;
    private static final int BAUD_RATE = 115200;
    
    public static final String ACTION_DATA_RECEIVED = "com.bravo.mobile.ACTION_LORA_DATA_RECEIVED";
    public static final String EXTRA_TELEMETRY = "telemetry_data";
    
    private UsbManager usbManager;
    private UsbSerialPort serialPort;
    private LoRaReceiver loRaReceiver;
    private final IBinder binder = new LocalBinder();
    private DataReceivedListener dataReceivedListener;

    public interface DataReceivedListener {
        void onTelemetryReceived(TelemetryData data);
        void onConnectionStateChanged(boolean connected);
    }

    public class LocalBinder extends Binder {
        public LoRaReceiverService getService() {
            return LoRaReceiverService.this;
        }
    }

    @Override
    public void onCreate() {
        super.onCreate();
        usbManager = (UsbManager) getSystemService(USB_SERVICE);
        loRaReceiver = new LoRaReceiver();
        setupLoRaReceiver();
        createNotificationChannel();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Notification notification = createNotification("LoRa Receiver Service Running");
        startForeground(NOTIFICATION_ID, notification);
        return START_STICKY;
    }

    @Override
    public IBinder onBind(Intent intent) {
        return binder;
    }

    /**
     * Setup LoRa receiver callbacks
     */
    private void setupLoRaReceiver() {
        loRaReceiver.addListener(new LoRaReceiver.LoRaPacketListener() {
            @Override
            public void onPacketReceived(LoRaPacket packet) {
                TelemetryData telemetry = TelemetryParser.parse(packet);
                if (telemetry != null) {
                    if (dataReceivedListener != null) {
                        dataReceivedListener.onTelemetryReceived(telemetry);
                    }
                    broadcastTelemetryData(telemetry);
                }
            }

            @Override
            public void onError(String error) {
                // Log error or notify user
            }
        });
    }

    /**
     * Connect to USB device
     */
    public boolean connectToUSB() {
        List<UsbSerialDriver> availableDrivers = UsbSerialProber.getDefaultProber()
                .findAllDrivers(usbManager);
        
        if (availableDrivers.isEmpty()) {
            return false;
        }

        // Get the first available driver (ESP32)
        UsbSerialDriver driver = availableDrivers.get(0);
        UsbDeviceConnection connection = usbManager.openDevice(driver.getDevice());
        
        if (connection == null) {
            return false;
        }

        try {
            serialPort = driver.getPorts().get(0);
            serialPort.open(connection);
            serialPort.setParameters(BAUD_RATE, 8, UsbSerialPort.STOPBITS_1, 
                                    UsbSerialPort.PARITY_NONE);
            
            // Start receiving data
            startReceiving();
            
            if (dataReceivedListener != null) {
                dataReceivedListener.onConnectionStateChanged(true);
            }
            
            return true;
        } catch (IOException e) {
            e.printStackTrace();
            return false;
        }
    }

    /**
     * Start receiving data from USB serial port
     */
    private void startReceiving() {
        if (serialPort == null) {
            return;
        }

        new Thread(new Runnable() {
            @Override
            public void run() {
                byte[] buffer = new byte[256];
                while (serialPort != null && serialPort.isOpen()) {
                    try {
                        int bytesRead = serialPort.read(buffer, 1000);
                        if (bytesRead > 0) {
                            // Create LoRa packet and process
                            byte[] data = new byte[bytesRead];
                            System.arraycopy(buffer, 0, data, 0, bytesRead);
                            
                            LoRaPacket packet = new LoRaPacket(data, 0, 0);
                            TelemetryData telemetry = TelemetryParser.parse(packet);
                            
                            if (telemetry != null) {
                                if (dataReceivedListener != null) {
                                    dataReceivedListener.onTelemetryReceived(telemetry);
                                }
                                broadcastTelemetryData(telemetry);
                            }
                        }
                    } catch (IOException e) {
                        break;
                    }
                }
            }
        }).start();
    }

    /**
     * Disconnect from USB device
     */
    public void disconnect() {
        if (serialPort != null) {
            try {
                serialPort.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
            serialPort = null;
        }
        
        loRaReceiver.stopReceiving();
        
        if (dataReceivedListener != null) {
            dataReceivedListener.onConnectionStateChanged(false);
        }
    }

    /**
     * Check if connected to USB device
     */
    public boolean isConnected() {
        return serialPort != null && serialPort.isOpen();
    }

    /**
     * Set data received listener
     */
    public void setDataReceivedListener(DataReceivedListener listener) {
        this.dataReceivedListener = listener;
    }

    /**
     * Broadcast telemetry data to the app
     */
    private void broadcastTelemetryData(TelemetryData telemetry) {
        Intent intent = new Intent(ACTION_DATA_RECEIVED);
        intent.putExtra(EXTRA_TELEMETRY, telemetry.toString());
        sendBroadcast(intent);
    }

    /**
     * Create notification channel for foreground service
     */
    private void createNotificationChannel() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel channel = new NotificationChannel(
                CHANNEL_ID,
                "LoRa Receiver Service",
                NotificationManager.IMPORTANCE_LOW
            );
            NotificationManager manager = getSystemService(NotificationManager.class);
            manager.createNotificationChannel(channel);
        }
    }

    /**
     * Create notification for foreground service
     */
    private Notification createNotification(String text) {
        return new NotificationCompat.Builder(this, CHANNEL_ID)
                .setContentTitle("BRAVO Mobile")
                .setContentText(text)
                .setSmallIcon(android.R.drawable.ic_dialog_info)
                .build();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        disconnect();
    }
}
