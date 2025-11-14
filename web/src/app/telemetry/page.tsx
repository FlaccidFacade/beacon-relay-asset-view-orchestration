'use client';

import { useState, useEffect } from 'react';
import { LineChart, Line, XAxis, YAxis, CartesianGrid, Tooltip, Legend, ResponsiveContainer } from 'recharts';
import { format } from 'date-fns';

interface DeviceData {
  id: string;
  name: string;
  status: 'online' | 'offline' | 'warning';
  battery: number;
  signal: number;
  temperature: number;
  lastUpdate: Date;
}

export default function TelemetryPage() {
  const [devices, setDevices] = useState<DeviceData[]>([]);
  const [selectedDevice, setSelectedDevice] = useState<string | null>(null);
  const [historicalData, setHistoricalData] = useState<Array<{
    time: string;
    battery: number;
    signal: number;
    temperature: number;
  }>>([]);

  useEffect(() => {
    // Mock device data
    const mockDevices: DeviceData[] = [
      {
        id: '1',
        name: 'Device Alpha',
        status: 'online',
        battery: 85,
        signal: 92,
        temperature: 23,
        lastUpdate: new Date(),
      },
      {
        id: '2',
        name: 'Device Beta',
        status: 'online',
        battery: 67,
        signal: 78,
        temperature: 25,
        lastUpdate: new Date(),
      },
      {
        id: '3',
        name: 'Device Gamma',
        status: 'warning',
        battery: 45,
        signal: 55,
        temperature: 28,
        lastUpdate: new Date(),
      },
      {
        id: '4',
        name: 'Device Delta',
        status: 'offline',
        battery: 12,
        signal: 0,
        temperature: 0,
        lastUpdate: new Date(Date.now() - 3600000),
      },
    ];
    setDevices(mockDevices);

    // Mock historical data for charts
    const mockHistory = Array.from({ length: 24 }, (_, i) => ({
      time: format(new Date(Date.now() - (23 - i) * 3600000), 'HH:mm'),
      battery: 85 - Math.random() * 20,
      signal: 92 - Math.random() * 15,
      temperature: 23 + Math.random() * 5,
    }));
    setHistoricalData(mockHistory);
  }, []);

  const getStatusColor = (status: string) => {
    switch (status) {
      case 'online':
        return 'bg-green-500';
      case 'warning':
        return 'bg-yellow-500';
      case 'offline':
        return 'bg-red-500';
      default:
        return 'bg-gray-500';
    }
  };

  const getStatusTextColor = (status: string) => {
    switch (status) {
      case 'online':
        return 'text-green-700';
      case 'warning':
        return 'text-yellow-700';
      case 'offline':
        return 'text-red-700';
      default:
        return 'text-gray-700';
    }
  };

  return (
    <div className="container mx-auto px-4 py-8">
      <h1 className="text-3xl font-bold mb-6 text-gray-900">Device Telemetry</h1>

      <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-4 mb-8">
        {devices.map((device) => (
          <div
            key={device.id}
            className={`bg-white rounded-lg shadow-md p-4 cursor-pointer transition-all ${
              selectedDevice === device.id ? 'ring-2 ring-blue-500' : ''
            }`}
            onClick={() => setSelectedDevice(device.id)}
          >
            <div className="flex items-center justify-between mb-3">
              <h3 className="font-semibold text-gray-900">{device.name}</h3>
              <span className={`w-3 h-3 rounded-full ${getStatusColor(device.status)}`} />
            </div>
            <div className="space-y-2 text-sm">
              <div className="flex justify-between">
                <span className="text-gray-600">Status:</span>
                <span className={`font-medium ${getStatusTextColor(device.status)}`}>
                  {device.status}
                </span>
              </div>
              <div className="flex justify-between">
                <span className="text-gray-600">Battery:</span>
                <span className="text-gray-900">{device.battery}%</span>
              </div>
              <div className="flex justify-between">
                <span className="text-gray-600">Signal:</span>
                <span className="text-gray-900">{device.signal}%</span>
              </div>
              <div className="flex justify-between">
                <span className="text-gray-600">Temp:</span>
                <span className="text-gray-900">{device.temperature}°C</span>
              </div>
              <div className="text-xs text-gray-500 pt-2">
                Last update: {format(device.lastUpdate, 'HH:mm:ss')}
              </div>
            </div>
          </div>
        ))}
      </div>

      {selectedDevice && (
        <div className="bg-white rounded-lg shadow-md p-6">
          <h2 className="text-xl font-semibold mb-4 text-gray-900">
            Historical Data - {devices.find((d) => d.id === selectedDevice)?.name}
          </h2>
          <div className="space-y-8">
            <div>
              <h3 className="text-lg font-medium mb-2 text-gray-700">Battery Level</h3>
              <ResponsiveContainer width="100%" height={200}>
                <LineChart data={historicalData}>
                  <CartesianGrid strokeDasharray="3 3" />
                  <XAxis dataKey="time" />
                  <YAxis domain={[0, 100]} />
                  <Tooltip />
                  <Legend />
                  <Line type="monotone" dataKey="battery" stroke="#10b981" name="Battery %" />
                </LineChart>
              </ResponsiveContainer>
            </div>
            <div>
              <h3 className="text-lg font-medium mb-2 text-gray-700">Signal Strength</h3>
              <ResponsiveContainer width="100%" height={200}>
                <LineChart data={historicalData}>
                  <CartesianGrid strokeDasharray="3 3" />
                  <XAxis dataKey="time" />
                  <YAxis domain={[0, 100]} />
                  <Tooltip />
                  <Legend />
                  <Line type="monotone" dataKey="signal" stroke="#3b82f6" name="Signal %" />
                </LineChart>
              </ResponsiveContainer>
            </div>
            <div>
              <h3 className="text-lg font-medium mb-2 text-gray-700">Temperature</h3>
              <ResponsiveContainer width="100%" height={200}>
                <LineChart data={historicalData}>
                  <CartesianGrid strokeDasharray="3 3" />
                  <XAxis dataKey="time" />
                  <YAxis domain={[15, 35]} />
                  <Tooltip />
                  <Legend />
                  <Line type="monotone" dataKey="temperature" stroke="#ef4444" name="Temp °C" />
                </LineChart>
              </ResponsiveContainer>
            </div>
          </div>
        </div>
      )}

      {!selectedDevice && (
        <div className="bg-blue-50 border border-blue-200 rounded-lg p-6 text-center">
          <p className="text-gray-700">Select a device card above to view historical telemetry data</p>
        </div>
      )}
    </div>
  );
}
