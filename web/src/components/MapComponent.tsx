'use client';

import { useEffect, useRef } from 'react';
import L from 'leaflet';

interface Device {
  id: string;
  name: string;
  lat: number;
  lng: number;
  status: 'online' | 'offline' | 'warning';
}

interface MapComponentProps {
  devices: Device[];
}

export default function MapComponent({ devices }: MapComponentProps) {
  const mapRef = useRef<L.Map | null>(null);
  const mapContainerRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    if (!mapContainerRef.current || mapRef.current) return;

    // Initialize map
    const map = L.map(mapContainerRef.current).setView([40.7128, -74.0060], 10);
    mapRef.current = map;

    // Add tile layer
    L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
      attribution: '&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors',
      maxZoom: 19,
    }).addTo(map);

    // Fix for default marker icon
    // eslint-disable-next-line @typescript-eslint/no-explicit-any
    delete (L.Icon.Default.prototype as any)._getIconUrl;
    L.Icon.Default.mergeOptions({
      iconRetinaUrl: 'https://cdnjs.cloudflare.com/ajax/libs/leaflet/1.7.1/images/marker-icon-2x.png',
      iconUrl: 'https://cdnjs.cloudflare.com/ajax/libs/leaflet/1.7.1/images/marker-icon.png',
      shadowUrl: 'https://cdnjs.cloudflare.com/ajax/libs/leaflet/1.7.1/images/marker-shadow.png',
    });

    return () => {
      if (mapRef.current) {
        mapRef.current.remove();
        mapRef.current = null;
      }
    };
  }, []);

  useEffect(() => {
    if (!mapRef.current) return;

    // Clear existing markers
    mapRef.current.eachLayer((layer) => {
      if (layer instanceof L.Marker) {
        mapRef.current?.removeLayer(layer);
      }
    });

    // Add markers for devices
    const markers: L.Marker[] = [];
    devices.forEach((device) => {
      const getMarkerColor = (status: string) => {
        switch (status) {
          case 'online':
            return 'green';
          case 'warning':
            return 'orange';
          case 'offline':
            return 'red';
          default:
            return 'gray';
        }
      };

      const marker = L.marker([device.lat, device.lng]).addTo(mapRef.current!);
      marker.bindPopup(`
        <div style="min-width: 150px;">
          <strong>${device.name}</strong><br/>
          Status: <span style="color: ${getMarkerColor(device.status)}">${device.status}</span><br/>
          Location: ${device.lat.toFixed(4)}, ${device.lng.toFixed(4)}
        </div>
      `);
      markers.push(marker);
    });

    // Fit bounds to show all markers
    if (markers.length > 0) {
      const group = L.featureGroup(markers);
      mapRef.current.fitBounds(group.getBounds().pad(0.1));
    }
  }, [devices]);

  return (
    <div 
      ref={mapContainerRef} 
      className="w-full h-[600px] rounded-lg shadow-md"
      style={{ zIndex: 0 }}
    />
  );
}
