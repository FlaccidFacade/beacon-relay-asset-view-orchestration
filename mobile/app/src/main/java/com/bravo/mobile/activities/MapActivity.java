package com.bravo.mobile.activities;

import android.Manifest;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import com.bravo.mobile.R;
import com.bravo.mobile.libs.MapVisualization;
import com.bravo.mobile.libs.TelemetryParser;
import com.bravo.mobile.models.LoRaPacket;
import com.bravo.mobile.models.TelemetryData;
import com.bravo.mobile.services.BLEConnectionService;
import com.bravo.mobile.services.LoRaReceiverService;

import org.osmdroid.views.MapView;

import java.text.SimpleDateFormat;
import java.util.Locale;

/**
 * Map activity for displaying GPS telemetry on an interactive map
 * Supports both online and offline map viewing
 */
public class MapActivity extends AppCompatActivity {
    private MapView mapView;
    private MapVisualization mapVisualization;
    private TextView textLatLng;
    private TextView textAltitude;
    private TextView textSpeed;
    private TextView textLastUpdate;
    
    private SimpleDateFormat dateFormat = new SimpleDateFormat("HH:mm:ss", Locale.getDefault());
    
    private static final int REQUEST_LOCATION_PERMISSION = 1;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_map);
        
        initializeViews();
        checkLocationPermission();
        registerTelemetryReceiver();
    }

    /**
     * Initialize UI components
     */
    private void initializeViews() {
        mapView = findViewById(R.id.mapView);
        textLatLng = findViewById(R.id.textLatLng);
        textAltitude = findViewById(R.id.textAltitude);
        textSpeed = findViewById(R.id.textSpeed);
        textLastUpdate = findViewById(R.id.textLastUpdate);
        
        // Initialize map visualization
        mapVisualization = new MapVisualization(this, mapView);
        
        // Enable offline maps by default
        mapVisualization.setOfflineMode(false);
    }

    /**
     * Check and request location permission if needed
     */
    private void checkLocationPermission() {
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION)
                != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this,
                new String[]{Manifest.permission.ACCESS_FINE_LOCATION},
                REQUEST_LOCATION_PERMISSION);
        }
    }

    /**
     * Register broadcast receiver for telemetry data
     */
    private void registerTelemetryReceiver() {
        IntentFilter filter = new IntentFilter();
        filter.addAction(BLEConnectionService.ACTION_DATA_RECEIVED);
        filter.addAction(LoRaReceiverService.ACTION_DATA_RECEIVED);
        registerReceiver(telemetryReceiver, filter);
    }

    /**
     * Broadcast receiver for telemetry data from services
     */
    private final BroadcastReceiver telemetryReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            
            if (BLEConnectionService.ACTION_DATA_RECEIVED.equals(action) ||
                LoRaReceiverService.ACTION_DATA_RECEIVED.equals(action)) {
                
                String telemetryString = intent.getStringExtra(BLEConnectionService.EXTRA_TELEMETRY);
                if (telemetryString != null) {
                    // Parse telemetry data from string
                    // For now, we'll create a mock telemetry object
                    // In production, you would deserialize the string
                    updateMapWithMockData();
                }
            }
        }
    };

    /**
     * Update map with telemetry data
     */
    private void updateMap(TelemetryData telemetry) {
        if (telemetry == null) {
            return;
        }
        
        runOnUiThread(() -> {
            // Update map visualization
            mapVisualization.updateLocation(telemetry);
            
            // Update telemetry info display
            textLatLng.setText(String.format("Location: %.6f, %.6f", 
                telemetry.getLatitude(), telemetry.getLongitude()));
            textAltitude.setText(String.format("Altitude: %.1f m", 
                telemetry.getAltitude()));
            textSpeed.setText(String.format("Speed: %.1f km/h", 
                telemetry.getSpeed()));
            textLastUpdate.setText(String.format("Last Update: %s", 
                dateFormat.format(telemetry.getTimestamp())));
        });
    }

    /**
     * Update map with mock data for demonstration
     */
    private void updateMapWithMockData() {
        // This is temporary for demonstration
        // In production, parse actual telemetry data
        TelemetryData mockData = new TelemetryData(37.7749, -122.4194, 50, 25.5f);
        mockData.setDeviceId("ESP32-001");
        mockData.setRssi(-75);
        mockData.setBatteryLevel(85);
        updateMap(mockData);
    }

    @Override
    public void onResume() {
        super.onResume();
        if (mapView != null) {
            mapView.onResume();
        }
    }

    @Override
    public void onPause() {
        super.onPause();
        if (mapView != null) {
            mapView.onPause();
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unregisterReceiver(telemetryReceiver);
        if (mapVisualization != null) {
            mapVisualization.onDestroy();
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == REQUEST_LOCATION_PERMISSION) {
            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                Toast.makeText(this, "Location permission granted", Toast.LENGTH_SHORT).show();
            } else {
                Toast.makeText(this, R.string.error_permission_denied, Toast.LENGTH_SHORT).show();
            }
        }
    }
}
