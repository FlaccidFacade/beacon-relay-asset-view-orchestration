# B.R.A.V.O API

**B**eacon **R**elay **A**sset **V**iew **O**rchestration API

A Node.js/TypeScript serverless API providing REST and GraphQL endpoints for IoT device management, telemetry data collection, and Over-The-Air (OTA) firmware updates.

## Features

- **Device Registration**: Register and manage IoT devices
- **Telemetry Collection**: Submit and retrieve device telemetry data
- **OTA Updates**: Manage firmware updates for devices
- **Dual API Support**: Both REST and GraphQL endpoints
- **Serverless Architecture**: Deployed on AWS Lambda + API Gateway
- **TypeScript**: Full type safety and modern development experience
- **CI/CD**: Automated deployment via GitHub Actions

## Project Structure

```
api/
├── src/
│   ├── lambda/           # Lambda function handlers
│   │   ├── rest.ts       # REST API Lambda handler
│   │   └── graphql.ts    # GraphQL API Lambda handler
│   ├── handlers/         # Business logic handlers
│   │   ├── devices.ts    # Device management handlers
│   │   ├── telemetry.ts  # Telemetry handlers
│   │   └── ota.ts        # OTA update handlers
│   ├── utils/            # Utility functions
│   │   ├── response.ts   # API response helpers
│   │   ├── validator.ts  # Input validation
│   │   └── database.ts   # Database operations (mock)
│   ├── types/            # TypeScript type definitions
│   │   └── index.ts      # Shared types
│   └── graphql/          # GraphQL schema and resolvers
│       ├── schema.ts     # GraphQL type definitions
│       └── resolvers.ts  # GraphQL resolvers
├── .github/
│   └── workflows/
│       └── deploy.yml    # CI/CD workflow
├── dist/                 # Compiled JavaScript (generated)
├── package.json
├── tsconfig.json
└── README.md
```

## Setup

### Prerequisites

- Node.js 20.x or later
- npm 10.x or later
- AWS Account (for deployment)
- AWS CLI configured (for deployment)

### Installation

1. Clone the repository:
```bash
git clone https://github.com/beacon-relay-asset-view-orchestration/api.git
cd api
```

2. Install dependencies:
```bash
npm install
```

3. Build the project:
```bash
npm run build
```

### Development

```bash
# Run TypeScript compiler in watch mode
npm run build -- --watch

# Run linter
npm run lint

# Fix linting issues
npm run lint:fix

# Type check without building
npm run typecheck
```

## REST API Endpoints

### Device Management

#### Register a Device
```http
POST /devices
Content-Type: application/json

{
  "name": "Temperature Sensor 1",
  "type": "sensor",
  "firmwareVersion": "1.0.0"
}
```

**Response:**
```json
{
  "success": true,
  "data": {
    "deviceId": "uuid-here",
    "name": "Temperature Sensor 1",
    "type": "sensor",
    "firmwareVersion": "1.0.0",
    "registeredAt": "2025-10-21T12:00:00.000Z",
    "status": "ACTIVE"
  }
}
```

#### Get Device
```http
GET /devices/{deviceId}
```

#### List All Devices
```http
GET /devices
```

#### Delete Device
```http
DELETE /devices/{deviceId}
```

### Telemetry

#### Submit Telemetry Data
```http
POST /telemetry
Content-Type: application/json

{
  "deviceId": "device-uuid",
  "data": {
    "temperature": 23.5,
    "humidity": 65.2,
    "signal_strength": -45,
    "battery_level": 87
  }
}
```

#### Get Telemetry by ID
```http
GET /telemetry/{telemetryId}
```

#### Get Device Telemetry
```http
GET /devices/{deviceId}/telemetry
```

### OTA Updates

#### Create OTA Update
```http
POST /ota-updates
Content-Type: application/json

{
  "deviceId": "device-uuid",
  "toVersion": "1.1.0",
  "downloadUrl": "https://example.com/firmware/v1.1.0.bin"
}
```

#### Get OTA Update
```http
GET /ota-updates/{updateId}
```

#### Update OTA Status
```http
PATCH /ota-updates/{updateId}/status
Content-Type: application/json

{
  "status": "COMPLETED"
}
```

**Valid status values:** `PENDING`, `IN_PROGRESS`, `COMPLETED`, `FAILED`, `CANCELLED`

#### Get Device OTA Updates
```http
GET /devices/{deviceId}/ota-updates
```

## GraphQL API

### GraphQL Endpoint
```
POST /graphql
```

### Example Queries

#### Register a Device
```graphql
mutation RegisterDevice {
  registerDevice(input: {
    name: "Temperature Sensor 1"
    type: "sensor"
    firmwareVersion: "1.0.0"
  }) {
    deviceId
    name
    type
    firmwareVersion
    registeredAt
    status
  }
}
```

#### Get All Devices
```graphql
query GetDevices {
  devices {
    deviceId
    name
    type
    firmwareVersion
    status
    lastSeen
  }
}
```

#### Submit Telemetry
```graphql
mutation SubmitTelemetry {
  submitTelemetry(input: {
    deviceId: "device-uuid"
    data: {
      temperature: 23.5
      humidity: 65.2
      signal_strength: -45
      battery_level: 87
    }
  }) {
    telemetryId
    deviceId
    timestamp
    data {
      temperature
      humidity
      signal_strength
      battery_level
    }
  }
}
```

#### Create OTA Update
```graphql
mutation CreateOTAUpdate {
  createOTAUpdate(input: {
    deviceId: "device-uuid"
    toVersion: "1.1.0"
    downloadUrl: "https://example.com/firmware/v1.1.0.bin"
  }) {
    updateId
    deviceId
    fromVersion
    toVersion
    status
    downloadUrl
    createdAt
  }
}
```

#### Get Device Telemetry
```graphql
query GetDeviceTelemetry {
  deviceTelemetry(deviceId: "device-uuid") {
    telemetryId
    timestamp
    data {
      temperature
      humidity
    }
  }
}
```

## Deployment

### GitHub Actions (Automated)

The project includes a GitHub Actions workflow that automatically deploys to AWS Lambda when changes are pushed to the `main` branch.

#### Required GitHub Secrets

Configure these secrets in your GitHub repository settings:

- `AWS_ACCESS_KEY_ID`: AWS access key with Lambda and API Gateway permissions
- `AWS_SECRET_ACCESS_KEY`: AWS secret access key
- `AWS_LAMBDA_ROLE_ARN`: ARN of the IAM role for Lambda execution

#### Deployment Process

1. Push to `main` branch:
```bash
git push origin main
```

2. GitHub Actions will:
   - Install dependencies
   - Run linter and type checks
   - Build the TypeScript project
   - Package the Lambda functions
   - Deploy to AWS Lambda
   - Update API Gateway integrations

### Manual Deployment

1. Build the project:
```bash
npm run build
```

2. Package for Lambda:
```bash
cd dist
npm install --production --no-package-lock
zip -r ../lambda.zip .
cd ..
```

3. Deploy REST API Lambda:
```bash
aws lambda create-function \
  --function-name bravo-api-rest \
  --runtime nodejs20.x \
  --role arn:aws:iam::ACCOUNT_ID:role/lambda-execution-role \
  --handler lambda/rest.handler \
  --zip-file fileb://lambda.zip \
  --timeout 30 \
  --memory-size 512 \
  --region us-east-1
```

4. Deploy GraphQL API Lambda:
```bash
aws lambda create-function \
  --function-name bravo-api-graphql \
  --runtime nodejs20.x \
  --role arn:aws:iam::ACCOUNT_ID:role/lambda-execution-role \
  --handler lambda/graphql.handler \
  --zip-file fileb://lambda.zip \
  --timeout 30 \
  --memory-size 512 \
  --region us-east-1
```

5. Set up API Gateway:
   - Create a new REST API in API Gateway
   - Create resources and methods for REST endpoints
   - Integrate methods with the Lambda functions
   - Deploy the API to a stage (e.g., `prod`)

### Update Existing Functions

```bash
aws lambda update-function-code \
  --function-name bravo-api-rest \
  --zip-file fileb://lambda.zip \
  --region us-east-1

aws lambda update-function-code \
  --function-name bravo-api-graphql \
  --zip-file fileb://lambda.zip \
  --region us-east-1
```

## AWS Infrastructure

### Required AWS Resources

1. **IAM Role for Lambda**: Role with permissions for:
   - CloudWatch Logs
   - (Optional) DynamoDB access
   - (Optional) S3 access for firmware files

2. **Lambda Functions**:
   - `bravo-api-rest`: Handles REST API requests
   - `bravo-api-graphql`: Handles GraphQL requests

3. **API Gateway**:
   - REST API with Lambda proxy integration
   - Configure CORS if needed
   - Set up custom domain (optional)

### Example IAM Policy

```json
{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Effect": "Allow",
      "Action": [
        "logs:CreateLogGroup",
        "logs:CreateLogStream",
        "logs:PutLogEvents"
      ],
      "Resource": "arn:aws:logs:*:*:*"
    }
  ]
}
```

## Database

The current implementation uses an in-memory mock database for demonstration purposes. For production use, replace the mock database in `src/utils/database.ts` with a real database solution:

### Recommended Options:

- **Amazon DynamoDB**: Serverless NoSQL database (recommended for Lambda)
- **Amazon RDS**: Relational database (PostgreSQL, MySQL)
- **Amazon Aurora Serverless**: Auto-scaling relational database

### Migration Steps:

1. Install database client (e.g., `@aws-sdk/client-dynamodb`)
2. Update `src/utils/database.ts` with real implementation
3. Add database credentials to AWS Secrets Manager
4. Update Lambda IAM role with database permissions

## Environment Variables

Configure these environment variables in your Lambda function:

- `AWS_REGION`: AWS region (default: us-east-1)
- `DATABASE_TABLE_NAME`: DynamoDB table name (if using DynamoDB)
- `LOG_LEVEL`: Logging level (debug, info, warn, error)

## Monitoring and Logging

- CloudWatch Logs: All Lambda executions are logged to CloudWatch
- CloudWatch Metrics: Monitor Lambda invocations, errors, and duration
- X-Ray: Enable AWS X-Ray for distributed tracing (optional)

## Testing

The API can be tested using:

- **cURL**: Command-line HTTP client
- **Postman**: API testing tool
- **GraphQL Playground**: Built-in GraphQL IDE (available at GraphQL endpoint)
- **AWS Console**: Test Lambda functions directly

### Example cURL Commands

```bash
# Register a device
curl -X POST https://api-gateway-url/devices \
  -H "Content-Type: application/json" \
  -d '{"name":"Sensor 1","type":"temperature","firmwareVersion":"1.0.0"}'

# Get all devices
curl https://api-gateway-url/devices

# Submit telemetry
curl -X POST https://api-gateway-url/telemetry \
  -H "Content-Type: application/json" \
  -d '{"deviceId":"device-id","data":{"temperature":25.5}}'
```

## License

ISC

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Submit a pull request

## Support

For issues and questions, please open a GitHub issue in the repository.
