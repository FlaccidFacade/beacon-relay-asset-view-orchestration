package com.bravo.mobile.libs;

import android.content.Context;
import android.graphics.drawable.Drawable;

import androidx.core.content.ContextCompat;

import com.bravo.mobile.models.TelemetryData;

import org.osmdroid.api.IMapController;
import org.osmdroid.config.Configuration;
import org.osmdroid.tileprovider.tilesource.TileSourceFactory;
import org.osmdroid.util.GeoPoint;
import org.osmdroid.views.MapView;
import org.osmdroid.views.overlay.Marker;
import org.osmdroid.views.overlay.Polyline;

import java.util.ArrayList;
import java.util.List;

/**
 * Utility class for visualizing GPS telemetry data on a map
 * Uses OSMDroid for offline map support
 */
public class MapVisualization {
    private MapView mapView;
    private Context context;
    private Marker currentLocationMarker;
    private Polyline trackingPath;
    private List<GeoPoint> pathPoints;

    public MapVisualization(Context context, MapView mapView) {
        this.context = context;
        this.mapView = mapView;
        this.pathPoints = new ArrayList<>();
        initializeMap();
    }

    /**
     * Initialize the map with default settings
     */
    private void initializeMap() {
        // Configure OSMDroid
        Configuration.getInstance().setUserAgentValue(context.getPackageName());
        
        // Set up the map
        mapView.setTileSource(TileSourceFactory.MAPNIK);
        mapView.setBuiltInZoomControls(true);
        mapView.setMultiTouchControls(true);
        
        // Set default zoom and center
        IMapController mapController = mapView.getController();
        mapController.setZoom(15.0);
        
        // Initialize tracking path
        trackingPath = new Polyline();
        trackingPath.setWidth(5f);
        trackingPath.setColor(0xFF0000FF); // Blue color
        mapView.getOverlays().add(trackingPath);
    }

    /**
     * Update map with new telemetry data
     * @param telemetry Latest telemetry data with GPS coordinates
     */
    public void updateLocation(TelemetryData telemetry) {
        if (telemetry == null || !TelemetryParser.isValidGPS(telemetry)) {
            return;
        }

        GeoPoint newPoint = new GeoPoint(telemetry.getLatitude(), telemetry.getLongitude());
        
        // Update current location marker
        updateCurrentLocationMarker(newPoint, telemetry);
        
        // Add to tracking path
        addToTrackingPath(newPoint);
        
        // Center map on current location
        IMapController mapController = mapView.getController();
        mapController.animateTo(newPoint);
        
        mapView.invalidate();
    }

    /**
     * Update or create the current location marker
     */
    private void updateCurrentLocationMarker(GeoPoint point, TelemetryData telemetry) {
        if (currentLocationMarker == null) {
            currentLocationMarker = new Marker(mapView);
            currentLocationMarker.setAnchor(Marker.ANCHOR_CENTER, Marker.ANCHOR_BOTTOM);
            mapView.getOverlays().add(currentLocationMarker);
        }
        
        currentLocationMarker.setPosition(point);
        currentLocationMarker.setTitle("Current Location");
        
        String snippet = String.format(
            "Lat: %.6f\nLon: %.6f\nAlt: %.1fm\nSpeed: %.1f km/h",
            telemetry.getLatitude(),
            telemetry.getLongitude(),
            telemetry.getAltitude(),
            telemetry.getSpeed()
        );
        currentLocationMarker.setSnippet(snippet);
    }

    /**
     * Add point to the tracking path
     */
    private void addToTrackingPath(GeoPoint point) {
        pathPoints.add(point);
        trackingPath.setPoints(pathPoints);
    }

    /**
     * Clear the tracking path
     */
    public void clearTrackingPath() {
        pathPoints.clear();
        trackingPath.setPoints(pathPoints);
        mapView.invalidate();
    }

    /**
     * Enable or disable offline mode
     * In offline mode, the map will only use cached tiles
     */
    public void setOfflineMode(boolean offline) {
        if (offline) {
            mapView.setUseDataConnection(false);
        } else {
            mapView.setUseDataConnection(true);
        }
    }

    /**
     * Set map zoom level
     */
    public void setZoomLevel(double zoom) {
        IMapController mapController = mapView.getController();
        mapController.setZoom(zoom);
    }

    /**
     * Center map on specific coordinates
     */
    public void centerOn(double latitude, double longitude) {
        GeoPoint point = new GeoPoint(latitude, longitude);
        IMapController mapController = mapView.getController();
        mapController.setCenter(point);
    }

    /**
     * Clean up resources
     */
    public void onDestroy() {
        if (mapView != null) {
            mapView.onDetach();
        }
    }
}
