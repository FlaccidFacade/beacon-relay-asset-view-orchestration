# B.R.A.V.O. Dashboard

**BT Radio Advanced Visual Orchestration** - A comprehensive web dashboard for monitoring and managing radio devices.

This Next.js application provides real-time device telemetry, interactive map visualization, and offline map caching capabilities.

## Features

- 📊 **Device Telemetry**: Monitor real-time device metrics including battery, signal strength, temperature, and status
- 🗺️ **Map Visualization**: Interactive map showing device locations with status indicators
- 💾 **Offline Map Caching**: Download and manage offline map tiles for areas without internet connectivity
- 📱 **Responsive Design**: Works seamlessly on desktop and mobile devices
- ⚡ **Real-time Updates**: Live data updates for device monitoring

## Technology Stack

- **Framework**: Next.js 15 with React 19
- **Language**: TypeScript
- **Styling**: Tailwind CSS 4
- **Maps**: Leaflet with React-Leaflet
- **Charts**: Recharts
- **Deployment**: AWS Amplify or S3 + CloudFront

## Project Structure

```
web/
├── .github/
│   └── workflows/
│       └── deploy.yml         # CI/CD workflow for AWS deployment
├── src/
│   ├── app/                   # Next.js app directory
│   │   ├── layout.tsx         # Root layout with navigation
│   │   ├── page.tsx           # Home page
│   │   ├── telemetry/         # Device telemetry page
│   │   ├── map/               # Map visualization page
│   │   └── offline-cache/     # Offline cache management page
│   ├── components/            # Reusable React components
│   │   ├── Navigation.tsx     # Main navigation component
│   │   └── MapComponent.tsx   # Leaflet map wrapper
│   ├── hooks/                 # Custom React hooks
│   │   └── useDevices.ts      # Device data management hook
│   └── lib/                   # Utility functions and libraries
│       └── utils.ts           # Common utility functions
├── public/                    # Static assets
├── next.config.ts             # Next.js configuration
├── package.json               # Project dependencies
└── README.md                  # This file
```

## Getting Started

### Prerequisites

- Node.js 20.x or higher
- npm 10.x or higher

### Installation

1. Clone the repository:
```bash
git clone https://github.com/BT-Radio-Advanced-Visual-Orchestration/web.git
cd web
```

2. Install dependencies:
```bash
npm install
```

3. Run the development server:
```bash
npm run dev
```

4. Open your browser and navigate to:
```
http://localhost:3000/bravo
```

### Building for Production

Build the application for production:

```bash
npm run build
```

The static files will be exported to the `out/` directory.

### Running Production Build Locally

```bash
npm run start
```

## Development

### Available Scripts

- `npm run dev` - Start development server with Turbopack
- `npm run build` - Build the application for production
- `npm run start` - Start production server
- `npm run lint` - Run ESLint to check code quality

### Code Structure

- **Pages**: Located in `src/app/`, each page is a React Server Component
- **Components**: Reusable UI components in `src/components/`
- **Hooks**: Custom React hooks for data fetching and state management
- **Lib**: Utility functions and shared logic

## Deployment

The application is configured to deploy automatically to AWS when a release is published.

### AWS Configuration

#### Option 1: AWS Amplify

1. Create an Amplify app in the AWS Console
2. Connect it to your GitHub repository
3. The deployment will happen automatically on push

#### Option 2: S3 + CloudFront

1. Create an S3 bucket for hosting static files
2. Create a CloudFront distribution pointing to the S3 bucket
3. Set up the following GitHub secrets:
   - `AWS_ACCESS_KEY_ID`: Your AWS access key
   - `AWS_SECRET_ACCESS_KEY`: Your AWS secret key
   - `AWS_REGION`: AWS region (e.g., `us-east-1`)
   - `S3_BUCKET`: Your S3 bucket name
   - `CLOUDFRONT_DISTRIBUTION_ID`: Your CloudFront distribution ID
   - `DOMAIN`: Your domain name (e.g., `flaccidfacade.dev`)

### GitHub Actions Workflow

The `.github/workflows/deploy.yml` file contains the CI/CD pipeline that:

1. Checks out the code
2. Installs dependencies
3. Builds the application
4. Deploys to AWS S3
5. Invalidates CloudFront cache

The workflow runs automatically on:
- Release published
- Manual trigger via GitHub Actions UI

## Environment Variables

Create a `.env.local` file for local development:

```env
# API endpoints (if using real backend)
NEXT_PUBLIC_API_URL=https://api.example.com
NEXT_PUBLIC_WS_URL=wss://api.example.com

# Map configuration
NEXT_PUBLIC_MAP_TILE_URL=https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png
```

## Features in Detail

### Device Telemetry

- Real-time monitoring of device metrics
- Interactive charts showing historical data
- Device status indicators (online, warning, offline)
- Click on devices to view detailed telemetry graphs

### Map Visualization

- Interactive Leaflet map with device markers
- Color-coded markers based on device status
- Click markers to view device details
- Automatic map bounds adjustment

### Offline Map Caching

- Define geographic regions to cache
- Configure zoom levels for caching
- Download map tiles for offline use
- Manage and update cached regions

## Browser Support

- Chrome (latest)
- Firefox (latest)
- Safari (latest)
- Edge (latest)

## Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is part of the B.R.A.V.O. system.

## Support

For issues and questions, please open an issue on GitHub.

## Acknowledgments

- Built with [Next.js](https://nextjs.org/)
- Maps powered by [Leaflet](https://leafletjs.com/)
- Charts by [Recharts](https://recharts.org/)
- Styled with [Tailwind CSS](https://tailwindcss.com/)
