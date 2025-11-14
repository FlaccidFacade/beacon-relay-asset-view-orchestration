'use client';

import { useState, useEffect } from 'react';
import dynamic from 'next/dynamic';

// Dynamically import the map component to avoid SSR issues
const MapComponent = dynamic(() => import('@/components/MapComponent'), {
  ssr: false,
  loading: () => (
    <div className="w-full h-[600px] bg-gray-200 rounded-lg shadow-md flex items-center justify-center">
      <p className="text-gray-600">Loading map...</p>
    </div>
  ),
});

interface Device {
  id: string;
  name: string;
  lat: number;
  lng: number;
  status: 'online' | 'offline' | 'warning';
}

export default function MapPage() {
  const [devices, setDevices] = useState<Device[]>([]);

  useEffect(() => {
    // Mock device data with coordinates around New York City
    const mockDevices: Device[] = [
      {
        id: '1',
        name: 'Device Alpha',
        lat: 40.7128,
        lng: -74.0060,
        status: 'online',
      },
      {
        id: '2',
        name: 'Device Beta',
        lat: 40.7589,
        lng: -73.9851,
        status: 'online',
      },
      {
        id: '3',
        name: 'Device Gamma',
        lat: 40.6782,
        lng: -73.9442,
        status: 'warning',
      },
      {
        id: '4',
        name: 'Device Delta',
        lat: 40.7489,
        lng: -73.9680,
        status: 'offline',
      },
      {
        id: '5',
        name: 'Device Epsilon',
        lat: 40.7282,
        lng: -74.0776,
        status: 'online',
      },
    ];
    setDevices(mockDevices);
  }, []);

  return (
    <div className="container mx-auto px-4 py-8">
      <h1 className="text-3xl font-bold mb-6 text-gray-900">Map Visualization</h1>

      <div className="mb-6 bg-white rounded-lg shadow-md p-4">
        <h2 className="text-lg font-semibold mb-3 text-gray-900">Device Locations</h2>
        <div className="grid grid-cols-2 md:grid-cols-5 gap-4">
          {devices.map((device) => (
            <div key={device.id} className="flex items-center space-x-2">
              <span
                className={`w-3 h-3 rounded-full ${
                  device.status === 'online'
                    ? 'bg-green-500'
                    : device.status === 'warning'
                    ? 'bg-yellow-500'
                    : 'bg-red-500'
                }`}
              />
              <span className="text-sm text-gray-700">{device.name}</span>
            </div>
          ))}
        </div>
      </div>

      <div className="mb-6">
        <MapComponent devices={devices} />
      </div>

      <div className="bg-blue-50 border border-blue-200 rounded-lg p-4">
        <h3 className="font-semibold mb-2 text-gray-900">Map Features</h3>
        <ul className="list-disc list-inside space-y-1 text-sm text-gray-700">
          <li>Click on markers to view device details</li>
          <li>Zoom in/out using the controls or mouse wheel</li>
          <li>Pan by clicking and dragging the map</li>
          <li>Marker colors indicate device status (green: online, yellow: warning, red: offline)</li>
        </ul>
      </div>
    </div>
  );
}
