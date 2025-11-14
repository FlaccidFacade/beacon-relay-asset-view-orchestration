'use client';

import { useState } from 'react';

interface CacheRegion {
  id: string;
  name: string;
  bounds: {
    north: number;
    south: number;
    east: number;
    west: number;
  };
  zoomLevels: number[];
  status: 'cached' | 'downloading' | 'pending';
  size: string;
  lastUpdate: string;
}

export default function OfflineCachePage() {
  const [cacheRegions, setCacheRegions] = useState<CacheRegion[]>([
    {
      id: '1',
      name: 'New York City',
      bounds: { north: 40.8, south: 40.7, east: -73.9, west: -74.1 },
      zoomLevels: [10, 11, 12, 13, 14],
      status: 'cached',
      size: '124 MB',
      lastUpdate: '2025-10-20',
    },
    {
      id: '2',
      name: 'Brooklyn Area',
      bounds: { north: 40.75, south: 40.65, east: -73.85, west: -74.05 },
      zoomLevels: [12, 13, 14, 15],
      status: 'cached',
      size: '98 MB',
      lastUpdate: '2025-10-19',
    },
  ]);

  const [newRegion, setNewRegion] = useState({
    name: '',
    north: '',
    south: '',
    east: '',
    west: '',
    minZoom: '10',
    maxZoom: '14',
  });

  const handleAddRegion = () => {
    if (!newRegion.name || !newRegion.north || !newRegion.south || !newRegion.east || !newRegion.west) {
      alert('Please fill in all fields');
      return;
    }

    const zoomLevels = [];
    for (let i = parseInt(newRegion.minZoom); i <= parseInt(newRegion.maxZoom); i++) {
      zoomLevels.push(i);
    }

    const region: CacheRegion = {
      id: Date.now().toString(),
      name: newRegion.name,
      bounds: {
        north: parseFloat(newRegion.north),
        south: parseFloat(newRegion.south),
        east: parseFloat(newRegion.east),
        west: parseFloat(newRegion.west),
      },
      zoomLevels,
      status: 'pending',
      size: '0 MB',
      lastUpdate: new Date().toISOString().split('T')[0],
    };

    setCacheRegions([...cacheRegions, region]);
    setNewRegion({
      name: '',
      north: '',
      south: '',
      east: '',
      west: '',
      minZoom: '10',
      maxZoom: '14',
    });
  };

  const handleDownload = (id: string) => {
    setCacheRegions(
      cacheRegions.map((region) =>
        region.id === id ? { ...region, status: 'downloading' as const } : region
      )
    );

    // Simulate download
    setTimeout(() => {
      setCacheRegions((regions) =>
        regions.map((region) =>
          region.id === id
            ? {
                ...region,
                status: 'cached' as const,
                size: `${Math.floor(Math.random() * 200) + 50} MB`,
              }
            : region
        )
      );
    }, 2000);
  };

  const handleDelete = (id: string) => {
    if (confirm('Are you sure you want to delete this cached region?')) {
      setCacheRegions(cacheRegions.filter((region) => region.id !== id));
    }
  };

  const getStatusColor = (status: string) => {
    switch (status) {
      case 'cached':
        return 'bg-green-100 text-green-800';
      case 'downloading':
        return 'bg-blue-100 text-blue-800';
      case 'pending':
        return 'bg-yellow-100 text-yellow-800';
      default:
        return 'bg-gray-100 text-gray-800';
    }
  };

  return (
    <div className="container mx-auto px-4 py-8">
      <h1 className="text-3xl font-bold mb-6 text-gray-900">Offline Map Cache</h1>

      <div className="bg-white rounded-lg shadow-md p-6 mb-6">
        <h2 className="text-xl font-semibold mb-4 text-gray-900">Add New Cache Region</h2>
        <div className="grid grid-cols-1 md:grid-cols-2 gap-4 mb-4">
          <div>
            <label className="block text-sm font-medium text-gray-700 mb-1">Region Name</label>
            <input
              type="text"
              className="w-full px-3 py-2 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-blue-500 text-gray-900"
              value={newRegion.name}
              onChange={(e) => setNewRegion({ ...newRegion, name: e.target.value })}
              placeholder="e.g., Manhattan District"
            />
          </div>
          <div className="grid grid-cols-2 gap-2">
            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">Min Zoom</label>
              <input
                type="number"
                className="w-full px-3 py-2 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-blue-500 text-gray-900"
                value={newRegion.minZoom}
                onChange={(e) => setNewRegion({ ...newRegion, minZoom: e.target.value })}
                min="1"
                max="19"
              />
            </div>
            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">Max Zoom</label>
              <input
                type="number"
                className="w-full px-3 py-2 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-blue-500 text-gray-900"
                value={newRegion.maxZoom}
                onChange={(e) => setNewRegion({ ...newRegion, maxZoom: e.target.value })}
                min="1"
                max="19"
              />
            </div>
          </div>
        </div>
        <div className="grid grid-cols-2 md:grid-cols-4 gap-4 mb-4">
          <div>
            <label className="block text-sm font-medium text-gray-700 mb-1">North</label>
            <input
              type="number"
              step="0.0001"
              className="w-full px-3 py-2 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-blue-500 text-gray-900"
              value={newRegion.north}
              onChange={(e) => setNewRegion({ ...newRegion, north: e.target.value })}
              placeholder="40.8"
            />
          </div>
          <div>
            <label className="block text-sm font-medium text-gray-700 mb-1">South</label>
            <input
              type="number"
              step="0.0001"
              className="w-full px-3 py-2 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-blue-500 text-gray-900"
              value={newRegion.south}
              onChange={(e) => setNewRegion({ ...newRegion, south: e.target.value })}
              placeholder="40.7"
            />
          </div>
          <div>
            <label className="block text-sm font-medium text-gray-700 mb-1">East</label>
            <input
              type="number"
              step="0.0001"
              className="w-full px-3 py-2 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-blue-500 text-gray-900"
              value={newRegion.east}
              onChange={(e) => setNewRegion({ ...newRegion, east: e.target.value })}
              placeholder="-73.9"
            />
          </div>
          <div>
            <label className="block text-sm font-medium text-gray-700 mb-1">West</label>
            <input
              type="number"
              step="0.0001"
              className="w-full px-3 py-2 border border-gray-300 rounded-md focus:outline-none focus:ring-2 focus:ring-blue-500 text-gray-900"
              value={newRegion.west}
              onChange={(e) => setNewRegion({ ...newRegion, west: e.target.value })}
              placeholder="-74.1"
            />
          </div>
        </div>
        <button
          onClick={handleAddRegion}
          className="px-4 py-2 bg-blue-600 text-white rounded-md hover:bg-blue-700 transition-colors"
        >
          Add Region
        </button>
      </div>

      <div className="bg-white rounded-lg shadow-md p-6">
        <h2 className="text-xl font-semibold mb-4 text-gray-900">Cached Regions</h2>
        <div className="space-y-4">
          {cacheRegions.length === 0 ? (
            <p className="text-gray-500 text-center py-8">No cached regions yet. Add one above!</p>
          ) : (
            cacheRegions.map((region) => (
              <div key={region.id} className="border border-gray-200 rounded-lg p-4">
                <div className="flex items-center justify-between mb-2">
                  <h3 className="text-lg font-semibold text-gray-900">{region.name}</h3>
                  <span className={`px-3 py-1 rounded-full text-sm font-medium ${getStatusColor(region.status)}`}>
                    {region.status}
                  </span>
                </div>
                <div className="grid grid-cols-2 md:grid-cols-4 gap-4 mb-3 text-sm">
                  <div>
                    <span className="text-gray-600">Bounds:</span>
                    <p className="text-gray-900">
                      N: {region.bounds.north}, S: {region.bounds.south}
                    </p>
                    <p className="text-gray-900">
                      E: {region.bounds.east}, W: {region.bounds.west}
                    </p>
                  </div>
                  <div>
                    <span className="text-gray-600">Zoom Levels:</span>
                    <p className="text-gray-900">{region.zoomLevels.join(', ')}</p>
                  </div>
                  <div>
                    <span className="text-gray-600">Size:</span>
                    <p className="text-gray-900">{region.size}</p>
                  </div>
                  <div>
                    <span className="text-gray-600">Last Update:</span>
                    <p className="text-gray-900">{region.lastUpdate}</p>
                  </div>
                </div>
                <div className="flex gap-2">
                  {region.status === 'pending' && (
                    <button
                      onClick={() => handleDownload(region.id)}
                      className="px-3 py-1 bg-blue-600 text-white text-sm rounded hover:bg-blue-700 transition-colors"
                    >
                      Download
                    </button>
                  )}
                  {region.status === 'downloading' && (
                    <button
                      disabled
                      className="px-3 py-1 bg-gray-400 text-white text-sm rounded cursor-not-allowed"
                    >
                      Downloading...
                    </button>
                  )}
                  {region.status === 'cached' && (
                    <button
                      onClick={() => handleDownload(region.id)}
                      className="px-3 py-1 bg-green-600 text-white text-sm rounded hover:bg-green-700 transition-colors"
                    >
                      Update
                    </button>
                  )}
                  <button
                    onClick={() => handleDelete(region.id)}
                    className="px-3 py-1 bg-red-600 text-white text-sm rounded hover:bg-red-700 transition-colors"
                  >
                    Delete
                  </button>
                </div>
              </div>
            ))
          )}
        </div>
      </div>

      <div className="bg-blue-50 border border-blue-200 rounded-lg p-4 mt-6">
        <h3 className="font-semibold mb-2 text-gray-900">About Offline Caching</h3>
        <p className="text-sm text-gray-700 mb-2">
          Offline map caching allows you to download map tiles for specific regions and zoom levels.
          This enables the application to work without an internet connection.
        </p>
        <ul className="list-disc list-inside space-y-1 text-sm text-gray-700">
          <li>Higher zoom levels result in more detailed maps but larger cache sizes</li>
          <li>Cache sizes are approximate and depend on the map content</li>
          <li>Downloaded regions can be updated to get the latest map data</li>
        </ul>
      </div>
    </div>
  );
}
