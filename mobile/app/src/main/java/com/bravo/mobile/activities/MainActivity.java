package com.bravo.mobile.activities;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.IBinder;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;

import com.bravo.mobile.R;
import com.bravo.mobile.models.TelemetryData;
import com.bravo.mobile.services.BLEConnectionService;
import com.bravo.mobile.services.LoRaReceiverService;

import java.text.SimpleDateFormat;
import java.util.Locale;

/**
 * Main activity for BRAVO Mobile app
 * Provides UI for connecting to ESP32 via BLE or USB and viewing telemetry data
 */
public class MainActivity extends AppCompatActivity {
    private TextView textConnectionStatus;
    private TextView textTelemetryDetails;
    private Button buttonConnectBLE;
    private Button buttonConnectUSB;
    private Button buttonViewMap;
    
    private BLEConnectionService bleService;
    private LoRaReceiverService loRaService;
    private boolean bleBound = false;
    private boolean loRaBound = false;
    
    private SimpleDateFormat dateFormat = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss", Locale.getDefault());

    // Permission launcher
    private final ActivityResultLauncher<String[]> permissionLauncher = 
        registerForActivityResult(new ActivityResultContracts.RequestMultiplePermissions(), result -> {
            boolean allGranted = true;
            for (Boolean granted : result.values()) {
                if (!granted) {
                    allGranted = false;
                    break;
                }
            }
            
            if (!allGranted) {
                Toast.makeText(this, R.string.error_permission_denied, Toast.LENGTH_SHORT).show();
            }
        });

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        initializeViews();
        requestPermissions();
        setupButtons();
    }

    /**
     * Initialize UI components
     */
    private void initializeViews() {
        textConnectionStatus = findViewById(R.id.textConnectionStatus);
        textTelemetryDetails = findViewById(R.id.textTelemetryDetails);
        buttonConnectBLE = findViewById(R.id.buttonConnectBLE);
        buttonConnectUSB = findViewById(R.id.buttonConnectUSB);
        buttonViewMap = findViewById(R.id.buttonViewMap);
    }

    /**
     * Request necessary permissions
     */
    private void requestPermissions() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            String[] permissions = {
                Manifest.permission.BLUETOOTH_SCAN,
                Manifest.permission.BLUETOOTH_CONNECT,
                Manifest.permission.ACCESS_FINE_LOCATION
            };
            
            boolean needsPermission = false;
            for (String permission : permissions) {
                if (ContextCompat.checkSelfPermission(this, permission) 
                        != PackageManager.PERMISSION_GRANTED) {
                    needsPermission = true;
                    break;
                }
            }
            
            if (needsPermission) {
                permissionLauncher.launch(permissions);
            }
        }
    }

    /**
     * Setup button click listeners
     */
    private void setupButtons() {
        buttonConnectBLE.setOnClickListener(v -> connectViaBLE());
        buttonConnectUSB.setOnClickListener(v -> connectViaUSB());
        buttonViewMap.setOnClickListener(v -> openMapActivity());
    }

    /**
     * Connect to ESP32 via Bluetooth Low Energy
     */
    private void connectViaBLE() {
        BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        
        if (bluetoothAdapter == null) {
            Toast.makeText(this, R.string.error_bluetooth_not_supported, Toast.LENGTH_SHORT).show();
            return;
        }
        
        if (!bluetoothAdapter.isEnabled()) {
            Toast.makeText(this, R.string.error_bluetooth_not_enabled, Toast.LENGTH_SHORT).show();
            return;
        }
        
        // Start BLE service
        Intent intent = new Intent(this, BLEConnectionService.class);
        startService(intent);
        bindService(intent, bleConnection, Context.BIND_AUTO_CREATE);
        
        textConnectionStatus.setText(R.string.status_connecting);
        
        // TODO: Show device selection dialog
        Toast.makeText(this, "BLE connection initiated", Toast.LENGTH_SHORT).show();
    }

    /**
     * Connect to ESP32 via USB
     */
    private void connectViaUSB() {
        // Start LoRa receiver service
        Intent intent = new Intent(this, LoRaReceiverService.class);
        startService(intent);
        bindService(intent, loRaConnection, Context.BIND_AUTO_CREATE);
        
        textConnectionStatus.setText(R.string.status_connecting);
    }

    /**
     * Open map activity
     */
    private void openMapActivity() {
        Intent intent = new Intent(this, MapActivity.class);
        startActivity(intent);
    }

    /**
     * Update telemetry display
     */
    private void updateTelemetryDisplay(TelemetryData data) {
        if (data == null) {
            return;
        }
        
        runOnUiThread(() -> {
            StringBuilder sb = new StringBuilder();
            sb.append("Device: ").append(data.getDeviceId() != null ? data.getDeviceId() : "Unknown").append("\n");
            sb.append(String.format("Latitude: %.6f\n", data.getLatitude()));
            sb.append(String.format("Longitude: %.6f\n", data.getLongitude()));
            sb.append(String.format("Altitude: %.1f m\n", data.getAltitude()));
            sb.append(String.format("Speed: %.1f km/h\n", data.getSpeed()));
            sb.append(String.format("Signal: %d dBm\n", data.getRssi()));
            sb.append(String.format("Battery: %d%%\n", data.getBatteryLevel()));
            sb.append("Last Update: ").append(dateFormat.format(data.getTimestamp()));
            
            textTelemetryDetails.setText(sb.toString());
            textConnectionStatus.setText(R.string.status_receiving);
        });
    }

    /**
     * Service connection for BLE service
     */
    private final ServiceConnection bleConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            BLEConnectionService.LocalBinder binder = (BLEConnectionService.LocalBinder) service;
            bleService = binder.getService();
            bleBound = true;
            
            bleService.setConnectionStateListener(new BLEConnectionService.ConnectionStateListener() {
                @Override
                public void onConnectionStateChanged(boolean connected) {
                    runOnUiThread(() -> {
                        textConnectionStatus.setText(connected ? 
                            R.string.status_connected : R.string.status_disconnected);
                    });
                }
                
                @Override
                public void onTelemetryReceived(TelemetryData data) {
                    updateTelemetryDisplay(data);
                }
            });
            
            // TODO: Connect to specific device
        }
        
        @Override
        public void onServiceDisconnected(ComponentName name) {
            bleBound = false;
        }
    };

    /**
     * Service connection for LoRa receiver service
     */
    private final ServiceConnection loRaConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            LoRaReceiverService.LocalBinder binder = (LoRaReceiverService.LocalBinder) service;
            loRaService = binder.getService();
            loRaBound = true;
            
            loRaService.setDataReceivedListener(new LoRaReceiverService.DataReceivedListener() {
                @Override
                public void onTelemetryReceived(TelemetryData data) {
                    updateTelemetryDisplay(data);
                }
                
                @Override
                public void onConnectionStateChanged(boolean connected) {
                    runOnUiThread(() -> {
                        textConnectionStatus.setText(connected ? 
                            R.string.status_connected : R.string.status_disconnected);
                    });
                }
            });
            
            // Attempt to connect to USB device
            boolean connected = loRaService.connectToUSB();
            if (!connected) {
                Toast.makeText(MainActivity.this, R.string.error_no_device_found, 
                              Toast.LENGTH_SHORT).show();
            }
        }
        
        @Override
        public void onServiceDisconnected(ComponentName name) {
            loRaBound = false;
        }
    };

    @Override
    protected void onDestroy() {
        super.onDestroy();
        
        if (bleBound) {
            unbindService(bleConnection);
            bleBound = false;
        }
        
        if (loRaBound) {
            unbindService(loRaConnection);
            loRaBound = false;
        }
    }
}
