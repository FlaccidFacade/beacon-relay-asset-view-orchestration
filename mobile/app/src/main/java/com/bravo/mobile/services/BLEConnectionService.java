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
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanResult;
import android.bluetooth.le.ScanSettings;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Binder;
import android.os.Build;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;

import java.util.Arrays;
import java.util.HashSet;
import java.util.Set;
import java.util.concurrent.atomic.AtomicBoolean;

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

    // Known Raspberry Pi Pico W device name prefixes (covers Pico WH and Pico 2WH)
    private static final Set<String> PICO_DEVICE_NAMES = new HashSet<>(Arrays.asList(
        "pico wh", "pico 2wh", "pico w"
    ));

    /** Scan timeout in milliseconds (30 seconds). */
    private static final long SCAN_TIMEOUT_MS = 30_000;

    /** Custom error code reported when Bluetooth is unavailable. */
    public static final int ERROR_BLUETOOTH_UNAVAILABLE = -1;
    /** Custom error code reported when no device was found within the scan timeout. */
    public static final int ERROR_NO_DEVICE_FOUND = -2;
    /** Custom error code reported when required Bluetooth permissions are not granted. */
    public static final int ERROR_PERMISSION_DENIED = -3;

    public static final String ACTION_DATA_RECEIVED = "com.bravo.mobile.ACTION_DATA_RECEIVED";
    public static final String EXTRA_TELEMETRY = "telemetry_data";
    
    private BluetoothAdapter bluetoothAdapter;
    private BluetoothLeScanner bluetoothLeScanner;
    private BluetoothGatt bluetoothGatt;
    private final AtomicBoolean isScanning = new AtomicBoolean(false);
    private final Handler scanTimeoutHandler = new Handler(Looper.getMainLooper());
    private LoRaReceiver loRaReceiver;
    private final IBinder binder = new LocalBinder();
    private ConnectionStateListener connectionStateListener;

    public interface ConnectionStateListener {
        void onConnectionStateChanged(boolean connected);
        void onTelemetryReceived(TelemetryData data);
        default void onScanStarted() {}
        default void onScanFailed(int errorCode) {}
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
        if (bluetoothAdapter != null) {
            bluetoothLeScanner = bluetoothAdapter.getBluetoothLeScanner();
        }
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
     * Scan for known Raspberry Pi Pico WH / Pico 2WH devices and auto-connect to the first one found.
     * No device selection prompt is shown — connection is automatic.
     * The scan stops automatically after {@value #SCAN_TIMEOUT_MS} ms if no device is found.
     */
    public void scanForPicoDevices() {
        if (bluetoothAdapter == null || bluetoothLeScanner == null) {
            if (connectionStateListener != null) {
                connectionStateListener.onScanFailed(ERROR_BLUETOOTH_UNAVAILABLE);
            }
            return;
        }

        if (isScanning.get()) {
            return;
        }

        // On Android 12+ BLUETOOTH_SCAN is a runtime permission
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S &&
                checkSelfPermission(android.Manifest.permission.BLUETOOTH_SCAN)
                        != PackageManager.PERMISSION_GRANTED) {
            if (connectionStateListener != null) {
                connectionStateListener.onScanFailed(ERROR_PERMISSION_DENIED);
            }
            return;
        }

        ScanSettings settings = new ScanSettings.Builder()
                .setScanMode(ScanSettings.SCAN_MODE_LOW_LATENCY)
                .build();

        // No device-name filters — case-insensitive matching is done in the scan callback.
        isScanning.set(true);
        bluetoothLeScanner.startScan(null, settings, leScanCallback);

        if (connectionStateListener != null) {
            connectionStateListener.onScanStarted();
        }

        // Stop scanning automatically after the timeout
        scanTimeoutHandler.postDelayed(() -> {
            if (isScanning.get()) {
                stopScan();
                if (connectionStateListener != null) {
                    connectionStateListener.onScanFailed(ERROR_NO_DEVICE_FOUND);
                }
            }
        }, SCAN_TIMEOUT_MS);
    }

    /**
     * Stop an ongoing BLE scan and cancel any pending scan timeout.
     */
    public void stopScan() {
        scanTimeoutHandler.removeCallbacksAndMessages(null);
        if (bluetoothLeScanner != null && isScanning.compareAndSet(true, false)) {
            bluetoothLeScanner.stopScan(leScanCallback);
        }
    }

    /**
     * Scan callback — auto-connects to the first matching Pico device found.
     */
    private final ScanCallback leScanCallback = new ScanCallback() {
        @Override
        public void onScanResult(int callbackType, ScanResult result) {
            BluetoothDevice device = result.getDevice();
            String name = device.getName();
            if (name != null && PICO_DEVICE_NAMES.contains(name.trim().toLowerCase())) {
                stopScan();
                connectToDevice(device.getAddress());
            }
        }

        @Override
        public void onScanFailed(int errorCode) {
            isScanning.set(false);
            if (connectionStateListener != null) {
                connectionStateListener.onScanFailed(errorCode);
            }
        }
    };

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
        stopScan();
        disconnect();
    }
}
