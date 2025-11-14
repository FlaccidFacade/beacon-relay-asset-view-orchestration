# B.R.A.V.O API Architecture

## Overview

The B.R.A.V.O API is a serverless IoT device management platform built on AWS Lambda and API Gateway, providing both REST and GraphQL interfaces for device registration, telemetry collection, and OTA firmware updates.

## Architecture Diagram

```
┌─────────────────────────────────────────────────────────────────────────┐
│                              Client Layer                                │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────────────────┐   │
│  │IoT Device│  │Mobile App│  │  Web App │  │  Backend Services    │   │
│  └────┬─────┘  └────┬─────┘  └────┬─────┘  └──────────┬───────────┘   │
└───────┼─────────────┼─────────────┼────────────────────┼───────────────┘
        │             │             │                    │
        └─────────────┴─────────────┴────────────────────┘
                            │
                            ▼
        ┌───────────────────────────────────────────────┐
        │         AWS API Gateway                       │
        │  ┌─────────────────┐  ┌──────────────────┐   │
        │  │  REST API       │  │  GraphQL API     │   │
        │  │  /devices       │  │  /graphql        │   │
        │  │  /telemetry     │  │                  │   │
        │  │  /ota-updates   │  │                  │   │
        │  └────────┬────────┘  └────────┬─────────┘   │
        └───────────┼──────────────────── ┼─────────────┘
                    │                     │
                    ▼                     ▼
        ┌───────────────────────────────────────────────┐
        │          AWS Lambda Functions                 │
        │  ┌─────────────────┐  ┌──────────────────┐   │
        │  │ bravo-api-rest  │  │bravo-api-graphql │   │
        │  │                 │  │                  │   │
        │  │  Rest Handler   │  │  Apollo Server   │   │
        │  └────────┬────────┘  └────────┬─────────┘   │
        └───────────┼──────────────────── ┼─────────────┘
                    │                     │
                    └──────────┬──────────┘
                               │
        ┌──────────────────────┴────────────────────────┐
        │           Business Logic Layer                 │
        │  ┌────────────────────────────────────────┐   │
        │  │  Handlers                              │   │
        │  │  ├── devices.ts (Device Management)    │   │
        │  │  ├── telemetry.ts (Data Collection)    │   │
        │  │  └── ota.ts (Firmware Updates)         │   │
        │  └────────────────────────────────────────┘   │
        │  ┌────────────────────────────────────────┐   │
        │  │  Utils                                 │   │
        │  │  ├── database.ts (Data Access)         │   │
        │  │  ├── validator.ts (Input Validation)   │   │
        │  │  └── response.ts (Response Formatting) │   │
        │  └────────────────────────────────────────┘   │
        └───────────────────┬────────────────────────────┘
                            │
                            ▼
        ┌───────────────────────────────────────────────┐
        │          Data Storage Layer                   │
        │  ┌─────────────────────────────────────────┐  │
        │  │  Current: In-Memory (Development)       │  │
        │  │  Production Options:                    │  │
        │  │  ├── Amazon DynamoDB (Recommended)      │  │
        │  │  ├── Amazon RDS (PostgreSQL/MySQL)      │  │
        │  │  └── Amazon Aurora Serverless           │  │
        │  └─────────────────────────────────────────┘  │
        └───────────────────────────────────────────────┘
                            │
        ┌───────────────────┴───────────────────────────┐
        │         Supporting AWS Services               │
        │  ┌──────────────┐  ┌──────────────────────┐  │
        │  │CloudWatch    │  │  S3 (Firmware)       │  │
        │  │Logs/Metrics  │  │  Storage             │  │
        │  └──────────────┘  └──────────────────────┘  │
        └───────────────────────────────────────────────┘
```

## Component Description

### API Gateway

**Purpose**: Entry point for all API requests

**Features**:
- Route management for REST endpoints
- Request/response transformation
- CORS configuration
- Rate limiting and throttling
- API key management (optional)
- Request validation

**Endpoints**:
- REST API: `/devices`, `/telemetry`, `/ota-updates`
- GraphQL API: `/graphql`

### Lambda Functions

#### bravo-api-rest
**Purpose**: Handles all REST API requests

**Features**:
- Request routing based on path and HTTP method
- Input validation
- Business logic execution
- Response formatting

**Handler**: `lambda/rest.handler`

#### bravo-api-graphql
**Purpose**: Handles GraphQL API requests

**Features**:
- GraphQL schema enforcement
- Query and mutation resolution
- GraphQL introspection
- Interactive GraphQL Playground

**Handler**: `lambda/graphql.handler`

### Business Logic Layer

#### Handlers
- **devices.ts**: Device registration, retrieval, and deletion
- **telemetry.ts**: Telemetry data submission and retrieval
- **ota.ts**: OTA update creation, status tracking, and retrieval

#### Utils
- **database.ts**: Data access layer (abstraction for storage operations)
- **validator.ts**: Input validation functions
- **response.ts**: Standardized API response formatting

### Data Models

#### Device
```typescript
{
  deviceId: string;        // Unique identifier
  name: string;            // Device name
  type: string;            // Device type (sensor, actuator, etc.)
  firmwareVersion: string; // Current firmware version
  registeredAt: string;    // ISO 8601 timestamp
  lastSeen?: string;       // ISO 8601 timestamp
  status: DeviceStatus;    // ACTIVE | INACTIVE | UPDATING | ERROR
}
```

#### Telemetry
```typescript
{
  telemetryId: string;     // Unique identifier
  deviceId: string;        // Associated device ID
  timestamp: string;       // ISO 8601 timestamp
  data: {                  // Flexible telemetry data
    temperature?: number;
    humidity?: number;
    signal_strength?: number;
    battery_level?: number;
    [key: string]: any;
  }
}
```

#### OTA Update
```typescript
{
  updateId: string;        // Unique identifier
  deviceId: string;        // Target device ID
  fromVersion: string;     // Current firmware version
  toVersion: string;       // Target firmware version
  status: OTAUpdateStatus; // PENDING | IN_PROGRESS | COMPLETED | FAILED | CANCELLED
  downloadUrl: string;     // Firmware download URL
  createdAt: string;       // ISO 8601 timestamp
  startedAt?: string;      // ISO 8601 timestamp
  completedAt?: string;    // ISO 8601 timestamp
}
```

## Data Flow

### Device Registration Flow
```
1. Client → API Gateway → Lambda (REST/GraphQL)
2. Lambda → Validate input
3. Lambda → Generate device ID
4. Lambda → Save to database
5. Lambda → Return device object
6. Lambda ← Database
7. Client ← API Gateway ← Lambda
```

### Telemetry Submission Flow
```
1. Device → API Gateway → Lambda
2. Lambda → Validate device exists
3. Lambda → Generate telemetry ID
4. Lambda → Save telemetry data
5. Lambda → Update device lastSeen
6. Lambda → Return telemetry object
7. Device ← API Gateway ← Lambda
```

### OTA Update Flow
```
1. Admin → API Gateway → Lambda (Create OTA)
2. Lambda → Validate device exists
3. Lambda → Create OTA update record (status: PENDING)
4. Lambda → Update device status to UPDATING
5. Lambda → Return OTA update object
6. Admin ← API Gateway ← Lambda

7. Device → API Gateway → Lambda (Poll for updates)
8. Lambda → Get OTA updates for device
9. Device ← Return pending updates

10. Device → Download firmware from S3
11. Device → Install firmware
12. Device → API Gateway → Lambda (Update status)
13. Lambda → Update OTA status to COMPLETED
14. Lambda → Update device firmware version
15. Lambda → Update device status to ACTIVE
16. Device ← API Gateway ← Lambda
```

## Scaling Considerations

### Lambda Auto-Scaling
- Lambda functions automatically scale based on request volume
- Concurrent execution limit: 1000 (default, can be increased)
- Cold start mitigation: Provisioned concurrency (optional)

### API Gateway Limits
- Default: 10,000 requests per second
- Burst: 5,000 requests
- Can be increased via AWS support

### Database Scaling

#### DynamoDB (Recommended for Serverless)
- On-demand capacity: Automatically scales
- Provisioned capacity: Manual scaling or auto-scaling
- Single-table design for optimal performance

#### RDS/Aurora
- Read replicas for read scaling
- Aurora Auto Scaling for write scaling
- Connection pooling required (use RDS Proxy)

## Security

### Authentication & Authorization
**Current**: None (development)

**Production Options**:
- AWS Cognito
- API Gateway API Keys
- IAM authorization
- Custom authorizers (Lambda)
- OAuth 2.0 / OpenID Connect

### Data Protection
- HTTPS/TLS for data in transit
- Encryption at rest (DynamoDB, S3)
- VPC for Lambda functions (optional)
- Secrets Manager for sensitive data

### IAM Permissions
**Lambda Execution Role**:
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
    },
    {
      "Effect": "Allow",
      "Action": [
        "dynamodb:GetItem",
        "dynamodb:PutItem",
        "dynamodb:UpdateItem",
        "dynamodb:DeleteItem",
        "dynamodb:Query",
        "dynamodb:Scan"
      ],
      "Resource": "arn:aws:dynamodb:*:*:table/bravo-*"
    },
    {
      "Effect": "Allow",
      "Action": [
        "s3:GetObject"
      ],
      "Resource": "arn:aws:s3:::bravo-firmware/*"
    }
  ]
}
```

## Monitoring & Observability

### CloudWatch Metrics
- Lambda invocations
- Lambda duration
- Lambda errors
- API Gateway 4XX/5XX errors
- API Gateway latency
- DynamoDB read/write capacity

### CloudWatch Logs
- Lambda function logs
- API Gateway access logs
- Structured logging for easier querying

### Alarms
- High error rate
- High latency (p99 > threshold)
- Throttling events
- DynamoDB capacity exceeded

### X-Ray (Optional)
- End-to-end request tracing
- Service map visualization
- Performance bottleneck identification

## Cost Optimization

### Lambda
- Right-size memory allocation
- Use ARM64 (Graviton2) for 20% cost savings
- Optimize cold starts
- Consider Lambda reserved concurrency

### API Gateway
- Use regional endpoints (cheaper than edge-optimized)
- Enable caching for read-heavy workloads
- Consider HTTP API vs REST API (cheaper, simpler)

### Database
- DynamoDB: Use on-demand for unpredictable workloads
- DynamoDB: Use provisioned for predictable workloads
- Enable DynamoDB auto-scaling
- Use TTL for automatic data expiration

### S3 (Firmware Storage)
- Use S3 Intelligent-Tiering
- Enable lifecycle policies
- Use CloudFront for global firmware distribution

## Disaster Recovery

### Backup Strategy
- DynamoDB: Point-in-time recovery (PITR)
- DynamoDB: On-demand backups
- Lambda: Code in Git (version controlled)
- Infrastructure as Code (Terraform/CloudFormation)

### Multi-Region Considerations
- Route 53 for DNS failover
- DynamoDB Global Tables
- Lambda deployment in multiple regions
- S3 Cross-Region Replication for firmware

## Future Enhancements

1. **Real-time Updates**: Add WebSocket support for real-time device state changes
2. **Advanced Analytics**: Integrate with AWS IoT Analytics or Kinesis
3. **Device Shadows**: Implement device state caching and synchronization
4. **Batch Operations**: Support bulk device operations
5. **Device Groups**: Add device grouping and fleet management
6. **Rule Engine**: Add conditional logic for automated actions
7. **Notifications**: Add SNS/SES for alerts and notifications
8. **Data Export**: Add S3 data lake integration for long-term storage
9. **API Versioning**: Implement API versioning strategy
10. **GraphQL Subscriptions**: Add real-time GraphQL subscriptions

## Technology Stack

- **Runtime**: Node.js 20.x
- **Language**: TypeScript 5.x
- **API Framework**: Express-like routing (REST), Apollo Server (GraphQL)
- **Cloud Provider**: AWS
- **Compute**: Lambda
- **API Gateway**: Amazon API Gateway
- **Database**: In-memory (dev), DynamoDB/RDS (prod)
- **Storage**: S3 (firmware files)
- **Monitoring**: CloudWatch
- **CI/CD**: GitHub Actions
- **IaC**: CloudFormation/Terraform (recommended for production)
