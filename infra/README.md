# B.R.A.V.O. Infrastructure

AWS Infrastructure-as-Code project for B.R.A.V.O. (Bluetooth Radio Advanced Visual Orchestration) using AWS CDK (TypeScript).

## Overview

This project defines and manages the complete AWS infrastructure for the B.R.A.V.O. platform using AWS Cloud Development Kit (CDK). The infrastructure is organized into four main stacks:

### Stack Architecture

1. **IoT Core Stack** - Manages IoT devices, thing types, and security policies
2. **DynamoDB Stack** - Stores device data and telemetry information
3. **S3/CloudFront Stack** - Hosts the static website with CDN distribution
4. **Lambda/API Gateway Stack** - Provides REST API endpoints for device and telemetry management

### 💰 Cost-Efficient Serverless Design

This infrastructure uses **100% serverless services** with **$0 fixed monthly costs** when idle:
- Development/Testing: **$1-5/month**
- Small Production: **$50-100/month**
- Large Production: **$500-1,000/month**

[Run Cost Analysis](scripts/cost-analysis.sh) | [View Cost Documentation](COST_ANALYSIS.md) | [Cost Tags Reference](COST_TAGS.md)

## Prerequisites

Before deploying this infrastructure, ensure you have:

- **Node.js** (v18 or later) and npm installed
- **AWS CLI** configured with appropriate credentials
- **AWS Account** with sufficient permissions to create resources
- **AWS CDK** installed globally (optional): `npm install -g aws-cdk`

### AWS Permissions Required

Your AWS credentials need permissions for:
- CloudFormation (full access)
- IAM (create roles and policies)
- IoT Core (full access)
- DynamoDB (full access)
- S3 (full access)
- CloudFront (full access)
- Lambda (full access)
- API Gateway (full access)
- CloudWatch Logs (create log groups)

## Project Structure

```
.
├── bin/
│   └── app.ts                 # CDK app entry point
├── cdk/
│   └── stacks/                # CDK stack definitions
│       ├── iot-core-stack.ts
│       ├── dynamodb-stack.ts
│       ├── s3-cloudfront-stack.ts
│       └── lambda-api-gateway-stack.ts
├── config/
│   └── stack-config.json      # Configuration parameters
├── scripts/
│   ├── deploy.sh              # Deployment script
│   └── destroy.sh             # Cleanup script
├── .github/
│   └── workflows/
│       └── deploy.yml         # GitHub Actions deployment workflow
├── cdk.json                   # CDK configuration
├── tsconfig.json              # TypeScript configuration
└── package.json               # Node.js dependencies
```

## Configuration

Edit `config/stack-config.json` to customize your deployment:

```json
{
  "projectName": "BRAVO",
  "stage": "dev",
  "aws": {
    "account": "YOUR_AWS_ACCOUNT_ID",
    "region": "us-east-1"
  }
}
```

**Important**: Replace `YOUR_AWS_ACCOUNT_ID` with your actual AWS account ID before deploying.

## Setup Instructions

### 1. Clone and Install

```bash
# Clone the repository
git clone https://github.com/beacon-relay-asset-view-orchestration/infra
cd infra

# Install dependencies
npm install

# Build the TypeScript project
npm run build
```

### 2. Configure AWS Credentials

```bash
# Configure AWS CLI with your credentials
aws configure

# Verify configuration
aws sts get-caller-identity
```

### 3. Bootstrap CDK (First-time only)

```bash
# Bootstrap CDK in your AWS account and region
npx cdk bootstrap
```

## Deployment

### Option 1: Manual Deployment

#### Deploy all stacks:
```bash
npm run deploy
```

#### Or use the deployment script:
```bash
./scripts/deploy.sh
```

#### Deploy specific stack:
```bash
npx cdk deploy BRAVO-IoTCore
npx cdk deploy BRAVO-DynamoDB
npx cdk deploy BRAVO-S3CloudFront
npx cdk deploy BRAVO-LambdaAPI
```

### Option 2: Automated Deployment via GitHub Actions

The project includes a GitHub Actions workflow that automatically deploys infrastructure on push to the `main` branch.

#### Setup GitHub Actions:

1. **Configure AWS OIDC Provider** in your AWS account
2. **Add GitHub Secrets**:
   - `AWS_ROLE_ARN`: ARN of the IAM role for GitHub Actions
   - `AWS_REGION`: AWS region for deployment (optional, defaults to us-east-1)

3. **Push to main branch** to trigger automatic deployment

```bash
git add .
git commit -m "Deploy infrastructure"
git push origin main
```

The workflow will:
- Build the CDK project
- Synthesize CloudFormation templates
- Deploy all stacks to AWS (on push to main only)
- Output deployment results

## Stack Details

### 1. IoT Core Stack

Creates and manages IoT infrastructure:
- **Thing Type**: Template for B.R.A.V.O. devices.
- **Device Policy**: Security policy for device connections and MQTT operations
- **IoT Endpoint**: Connection endpoint for devices

**Outputs**:
- Thing Type Name
- Device Policy Name
- IoT Endpoint URL

### 2. DynamoDB Stack

Creates two DynamoDB tables:

#### Device Table
- **Purpose**: Store device information and metadata
- **Partition Key**: `deviceId` (String)
- **Sort Key**: `timestamp` (Number)
- **GSI**: StatusIndex for querying by device status
- **Features**: Point-in-time recovery, encryption, DynamoDB Streams

#### Telemetry Table
- **Purpose**: Store device telemetry data
- **Partition Key**: `deviceId` (String)
- **Sort Key**: `timestamp` (Number)
- **GSI**: MetricTypeIndex for querying by metric type
- **Features**: TTL enabled (30 days), point-in-time recovery, encryption

**Outputs**:
- Device Table Name and ARN
- Telemetry Table Name and ARN

### 3. S3/CloudFront Stack

Creates static website hosting infrastructure:
- **S3 Bucket**: Stores website files (versioned, encrypted)
- **CloudFront Distribution**: CDN for global content delivery
- **Origin Access Identity**: Secure access from CloudFront to S3
- **Features**: HTTPS redirect, custom error pages, gzip compression

**Outputs**:
- S3 Bucket Name and ARN
- CloudFront Distribution ID
- Website URL

### 4. Lambda/API Gateway Stack

Creates REST API with Lambda functions:

#### API Endpoints:

**Device Management**:
- `GET /devices` - List all devices
- `GET /devices/{deviceId}` - Get specific device
- `POST /devices` - Create new device
- `DELETE /devices/{deviceId}` - Delete device

**Telemetry Management**:
- `GET /telemetry/{deviceId}` - Get device telemetry
- `POST /telemetry` - Submit telemetry data

**Features**:
- CORS enabled
- Request throttling (1000 req/sec, 2000 burst)
- CloudWatch logging and metrics
- IAM authentication
- DynamoDB and IoT integration

**Outputs**:
- API Gateway Endpoint URL
- Lambda Function Names

## Useful Commands

```bash
# Install dependencies
npm install

# Build TypeScript code
npm run build

# Watch for changes and rebuild
npm run watch

# Synthesize CloudFormation templates
npm run synth

# Deploy all stacks
npm run deploy

# Destroy all stacks
npm run destroy

# List all stacks
npx cdk list

# Show differences between deployed and local
npx cdk diff

# View CloudFormation template
npx cdk synth BRAVO-IoTCore

# Run cost analysis
./scripts/cost-analysis.sh
```

## Testing the API

After deployment, test the API endpoints:

```bash
# Get API endpoint from stack outputs
API_ENDPOINT=$(aws cloudformation describe-stacks \
  --stack-name BRAVO-LambdaAPI \
  --query 'Stacks[0].Outputs[?OutputKey==`ApiEndpoint`].OutputValue' \
  --output text)

# Create a device
curl -X POST $API_ENDPOINT/devices \
  -H "Content-Type: application/json" \
  -d '{"deviceId":"device-001","status":"active","name":"Test Device"}'

# Get all devices
curl $API_ENDPOINT/devices

# Get specific device
curl $API_ENDPOINT/devices/device-001

# Submit telemetry
curl -X POST $API_ENDPOINT/telemetry \
  -H "Content-Type: application/json" \
  -d '{"deviceId":"device-001","metricType":"temperature","value":25.5}'

# Get telemetry
curl $API_ENDPOINT/telemetry/device-001
```

## Cleanup

To remove all infrastructure:

```bash
# Using npm script
npm run destroy

# Using destroy script (with confirmation)
./scripts/destroy.sh

# Using CDK directly
npx cdk destroy --all
```

**Note**: Some resources (S3 buckets, DynamoDB tables) have retention policies to prevent accidental data loss. You may need to manually delete these after running destroy.

## Cost Analysis

This infrastructure uses **serverless, pay-per-use services** with **zero fixed monthly costs** when idle. All costs scale with actual usage.

### Quick Cost Estimates

- **Development/Testing**: $1-5/month
- **Small Production (100 devices)**: $50-100/month
- **Large Production (1,000 devices)**: $500-1,000/month

### Services Used

- **DynamoDB**: Pay-per-request billing mode ($0 when idle)
- **Lambda**: Pay per request and compute time (generous free tier)
- **API Gateway**: Pay per request ($0 when idle)
- **CloudFront**: Pay per request and data transfer ($0 when idle)
- **S3**: Pay for storage and requests (~$0.023/GB/month)
- **IoT Core**: Pay per message and connection ($0 when idle)

### Cost Analysis Tools

Run the cost analysis script to see detailed cost breakdowns and estimates:

```bash
./scripts/cost-analysis.sh
```

For comprehensive cost information including:
- Detailed service-by-service cost breakdown
- Cost optimization strategies
- Monitoring and alerting setup
- ROI analysis vs. self-hosted infrastructure

See the **[Complete Cost Analysis Documentation](COST_ANALYSIS.md)**

### Cost Monitoring

Set up AWS Cost Explorer and budget alerts to monitor costs:

```bash
# Example: Set up a budget alert
aws budgets create-budget \
  --account-id YOUR_ACCOUNT_ID \
  --budget file://budget.json \
  --notifications-with-subscribers file://notifications.json
```

## Security Best Practices

- All data is encrypted at rest using AWS-managed keys
- S3 bucket blocks all public access
- CloudFront enforces HTTPS
- IoT policies follow least-privilege principle
- API throttling prevents abuse
- Lambda functions have minimal IAM permissions

## Troubleshooting

### CDK Bootstrap Error
If you see bootstrap errors, run:
```bash
npx cdk bootstrap aws://ACCOUNT-ID/REGION
```

### Permission Denied
Ensure your AWS credentials have sufficient permissions for all services.

### Stack Update Failed
Check CloudFormation events:
```bash
aws cloudformation describe-stack-events --stack-name BRAVO-IoTCore
```

### API Gateway 403 Errors
Verify CORS configuration and API Gateway deployment.

## Contributing

1. Create a feature branch
2. Make your changes
3. Test locally with `npm run synth`
4. Submit a pull request

## License

ISC

## Support

For issues and questions, please open an issue in the repository.
