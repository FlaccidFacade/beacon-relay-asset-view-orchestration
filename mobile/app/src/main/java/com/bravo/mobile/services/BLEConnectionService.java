package com.bravo.mobile.services;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothProfile;
import android.content.Intent;
import android.os.Binder;
import android.os.Build;
import android.os.IBinder;

import androidx.core.app.NotificationCompat;

import com.bravo.mobile.libs.LoRaReceiver;
import com.bravo.mobile.models.LoRaPacket;
import com.bravo.mobile.models.TelemetryData;
import com.bravo.mobile.libs.TelemetryParser;

/**
 * Foreground service for managing BLE connection to ESP32 collar/dongle
 * Receives LoRa telemetry data via BLE and broadcasts it to the app
 */
public class BLEConnectionService extends Service {
    private static final String CHANNEL_ID = "BLEConnectionChannel";
    private static final int NOTIFICATION_ID = 1;
    
    // ESP32 Service and Characteristic UUIDs (placeholder - replace with actual UUIDs)
    private static final String SERVICE_UUID = "4fafc201-1fb5-459e-8fcc-c5c9c331914b";
    private static final String CHARACTERISTIC_UUID = "beb5483e-36e1-4688-b7f5-ea07361b26a8";
    
    public static final String ACTION_DATA_RECEIVED = "com.bravo.mobile.ACTION_DATA_RECEIVED";
    public static final String EXTRA_TELEMETRY = "telemetry_data";
    
    private BluetoothAdapter bluetoothAdapter;
    private BluetoothGatt bluetoothGatt;
    private LoRaReceiver loRaReceiver;
    private final IBinder binder = new LocalBinder();
    private ConnectionStateListener connectionStateListener;

    public interface ConnectionStateListener {
        void onConnectionStateChanged(boolean connected);
        void onTelemetryReceived(TelemetryData data);
    }

    public class LocalBinder extends Binder {
        public BLEConnectionService getService() {
            return BLEConnectionService.this;
        }
    }

    @Override
    public void onCreate() {
        super.onCreate();
        bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        loRaReceiver = new LoRaReceiver();
        createNotificationChannel();
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Notification notification = createNotification("BLE Connection Service Running");
        startForeground(NOTIFICATION_ID, notification);
        return START_STICKY;
    }

    @Override
    public IBinder onBind(Intent intent) {
        return binder;
    }

    /**
     * Connect to a BLE device by address
     */
    public boolean connectToDevice(String deviceAddress) {
        if (bluetoothAdapter == null || deviceAddress == null) {
            return false;
        }

        try {
            BluetoothDevice device = bluetoothAdapter.getRemoteDevice(deviceAddress);
            bluetoothGatt = device.connectGatt(this, false, gattCallback);
            return true;
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
    }

    /**
     * Disconnect from the BLE device
     */
    public void disconnect() {
        if (bluetoothGatt != null) {
            bluetoothGatt.disconnect();
            bluetoothGatt.close();
            bluetoothGatt = null;
        }
    }

    /**
     * Check if connected to a device
     */
    public boolean isConnected() {
        return bluetoothGatt != null;
    }

    /**
     * Set connection state listener
     */
    public void setConnectionStateListener(ConnectionStateListener listener) {
        this.connectionStateListener = listener;
    }

    /**
     * BLE GATT callback for handling connection events and data
     */
    private final BluetoothGattCallback gattCallback = new BluetoothGattCallback() {
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                bluetoothGatt.discoverServices();
                if (connectionStateListener != null) {
                    connectionStateListener.onConnectionStateChanged(true);
                }
            } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                if (connectionStateListener != null) {
                    connectionStateListener.onConnectionStateChanged(false);
                }
            }
        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                // Enable notifications for the characteristic
                // This is where you would set up to receive data from ESP32
            }
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt, 
                                           BluetoothGattCharacteristic characteristic) {
            // Data received from ESP32
            byte[] data = characteristic.getValue();
            if (data != null && data.length > 0) {
                processReceivedData(data);
            }
        }
    };

    /**
     * Process received data from BLE
     */
    private void processReceivedData(byte[] data) {
        try {
            // Create LoRa packet from received data
            LoRaPacket packet = new LoRaPacket(data, 0, 0);
            
            // Parse to telemetry data
            TelemetryData telemetry = TelemetryParser.parse(packet);
            
            if (telemetry != null) {
                // Notify listener
                if (connectionStateListener != null) {
                    connectionStateListener.onTelemetryReceived(telemetry);
                }
                
                // Broadcast to app
                broadcastTelemetryData(telemetry);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
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
                "BLE Connection Service",
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
