import { useState, useEffect } from 'react';

export interface Device {
  id: string;
  name: string;
  status: 'online' | 'offline' | 'warning';
  battery: number;
  signal: number;
  temperature: number;
  lat: number;
  lng: number;
  lastUpdate: Date;
}

/**
 * Custom hook to manage device data
 * In a real application, this would fetch from an API
 */
export function useDevices() {
  const [devices, setDevices] = useState<Device[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<Error | null>(null);

  useEffect(() => {
    // Simulate API call
    const fetchDevices = async () => {
      try {
        setLoading(true);
        // In a real app, this would be an API call
        await new Promise((resolve) => setTimeout(resolve, 500));
        
        const mockDevices: Device[] = [
          {
            id: '1',
            name: 'Device Alpha',
            status: 'online',
            battery: 85,
            signal: 92,
            temperature: 23,
            lat: 40.7128,
            lng: -74.0060,
            lastUpdate: new Date(),
          },
          {
            id: '2',
            name: 'Device Beta',
            status: 'online',
            battery: 67,
            signal: 78,
            temperature: 25,
            lat: 40.7589,
            lng: -73.9851,
            lastUpdate: new Date(),
          },
          {
            id: '3',
            name: 'Device Gamma',
            status: 'warning',
            battery: 45,
            signal: 55,
            temperature: 28,
            lat: 40.6782,
            lng: -73.9442,
            lastUpdate: new Date(),
          },
          {
            id: '4',
            name: 'Device Delta',
            status: 'offline',
            battery: 12,
            signal: 0,
            temperature: 0,
            lat: 40.7489,
            lng: -73.9680,
            lastUpdate: new Date(Date.now() - 3600000),
          },
        ];
        
        setDevices(mockDevices);
        setError(null);
      } catch (err) {
        setError(err instanceof Error ? err : new Error('Failed to fetch devices'));
      } finally {
        setLoading(false);
      }
    };

    fetchDevices();

    // Set up polling for real-time updates
    const interval = setInterval(fetchDevices, 30000); // Update every 30 seconds

    return () => clearInterval(interval);
  }, []);

  return { devices, loading, error };
}
