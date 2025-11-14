import Link from 'next/link';

export default function Home() {
  return (
    <div className="container mx-auto px-4 py-8">
      <div className="max-w-4xl mx-auto">
        <h1 className="text-4xl font-bold mb-6 text-gray-900">
          B.R.A.V.O. Dashboard
        </h1>
        <p className="text-xl text-gray-600 mb-8">
          BT Radio Advanced Visual Orchestration
        </p>

        <div className="grid grid-cols-1 md:grid-cols-3 gap-6 mb-8">
          <Link
            href="/telemetry"
            className="block p-6 bg-white rounded-lg shadow-md hover:shadow-lg transition-shadow border border-gray-200"
          >
            <h2 className="text-2xl font-semibold mb-3 text-gray-900">
              Device Telemetry
            </h2>
            <p className="text-gray-600">
              Monitor real-time device metrics, status, and performance data.
            </p>
          </Link>

          <Link
            href="/map"
            className="block p-6 bg-white rounded-lg shadow-md hover:shadow-lg transition-shadow border border-gray-200"
          >
            <h2 className="text-2xl font-semibold mb-3 text-gray-900">
              Map Visualization
            </h2>
            <p className="text-gray-600">
              View device locations and coverage areas on an interactive map.
            </p>
          </Link>

          <Link
            href="/offline-cache"
            className="block p-6 bg-white rounded-lg shadow-md hover:shadow-lg transition-shadow border border-gray-200"
          >
            <h2 className="text-2xl font-semibold mb-3 text-gray-900">
              Offline Cache
            </h2>
            <p className="text-gray-600">
              Manage offline map tiles and cached data for offline operation.
            </p>
          </Link>
        </div>

        <div className="bg-blue-50 border border-blue-200 rounded-lg p-6">
          <h3 className="text-lg font-semibold mb-2 text-gray-900">
            Getting Started
          </h3>
          <p className="text-gray-700">
            Select a section from the navigation above or click on one of the cards
            to explore the B.R.A.V.O. dashboard features.
          </p>
        </div>
      </div>
    </div>
  );
}
