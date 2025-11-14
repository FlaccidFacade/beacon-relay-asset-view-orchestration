# B.R.A.V.O. Infrastructure Architecture

## Overview

This document describes the architecture and design decisions for the B.R.A.V.O. (Bluetooth Radio Advanced Visual Orchestration) AWS infrastructure.

## Architecture Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                         B.R.A.V.O. Platform                      │
└─────────────────────────────────────────────────────────────────┘

┌──────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   IoT Devices    │    │   Web Clients    │    │  Mobile Apps    │
│   (MQTT)         │    │   (HTTPS)        │    │  (REST API)     │
└────────┬─────────┘    └────────┬─────────┘    └────────┬────────┘
         │                       │                        │
         │                       │                        │
    ┌────▼───────────────────────▼────────────────────────▼────┐
    │                     AWS Cloud                             │
    │                                                            │
    │  ┌──────────────┐         ┌─────────────────────────┐   │
    │  │  IoT Core    │         │   CloudFront (CDN)      │   │
    │  │  - Devices   │         │         │               │   │
    │  │  - Policies  │         │    ┌────▼──────┐        │   │
    │  │  - MQTT      │         │    │ S3 Bucket │        │   │
    │  └──────┬───────┘         │    │ (Website) │        │   │
    │         │                 │    └───────────┘        │   │
    │         │                 └─────────────────────────┘   │
    │         │                                                │
    │  ┌──────▼────────────────────────────────────────┐      │
    │  │         API Gateway (REST API)                 │      │
    │  │  - /devices/*    - /telemetry/*               │      │
    │  └──────┬───────────────────┬─────────────────────┘      │
    │         │                   │                            │
    │  ┌──────▼──────┐     ┌──────▼──────────┐               │
    │  │   Lambda    │     │    Lambda       │               │
    │  │  (Devices)  │     │  (Telemetry)    │               │
    │  └──────┬──────┘     └──────┬──────────┘               │
    │         │                   │                            │
    │  ┌──────▼───────────────────▼─────────┐                 │
    │  │         DynamoDB Tables             │                 │
    │  │  - Devices Table                    │                 │
    │  │  - Telemetry Table                  │                 │
    │  └─────────────────────────────────────┘                 │
    │                                                            │
    └────────────────────────────────────────────────────────────┘
```

## Stack Components

### 1. IoT Core Stack

**Purpose**: Manages IoT device connectivity and security

**Components**:
- **Thing Type**: Defines the template for B.R.A.V.O. devices.
- **Device Policy**: Grants devices permissions to:
  - Connect to IoT Core
  - Publish/Subscribe to specific MQTT topics
  - Receive messages on device-specific topics

**MQTT Topic Structure**:
- Pattern: `bravo/devices/{thingName}/*`
- Ensures devices can only access their own topics

**Security**:
- Certificate-based authentication
- Least-privilege IAM policies
- Topic-level authorization

### 2. DynamoDB Stack

**Purpose**: Persistent data storage for devices and telemetry

#### Devices Table

**Schema**:
- Partition Key: `deviceId` (String)
- Sort Key: `timestamp` (Number)
- Attributes: status, name, metadata, configuration

**Indexes**:
- StatusIndex: Query devices by status (active, inactive, error)

**Features**:
- DynamoDB Streams for change tracking
- Point-in-time recovery
- AWS-managed encryption

**Use Cases**:
- Device registry
- Device configuration storage
- Device status tracking

#### Telemetry Table

**Schema**:
- Partition Key: `deviceId` (String)
- Sort Key: `timestamp` (Number)
- Attributes: metricType, value, unit, metadata

**Indexes**:
- MetricTypeIndex: Query telemetry by metric type

**Features**:
- TTL enabled (30 days retention)
- Point-in-time recovery
- AWS-managed encryption

**Use Cases**:
- Time-series telemetry data
- Device metrics and measurements
- Historical data analysis

### 3. S3/CloudFront Stack

**Purpose**: Static website hosting with global CDN

**Components**:

#### S3 Bucket
- Stores static website files (HTML, CSS, JS)
- Versioning enabled for rollback capability
- Server-side encryption (SSE-S3)
- Block all public access (secured via CloudFront)

#### CloudFront Distribution
- Global CDN for low-latency content delivery
- HTTPS-only (redirect HTTP to HTTPS)
- Custom error pages (SPA routing support)
- Gzip compression enabled
- Origin Access Identity for secure S3 access

**Features**:
- High availability (multi-region)
- Low latency (edge caching)
- Cost optimization (reduced S3 requests)
- Security (DDoS protection, HTTPS)

### 4. Lambda/API Gateway Stack

**Purpose**: REST API for device and telemetry management

#### API Endpoints

**Device Management**:
- `GET /devices` - List all devices
- `GET /devices/{deviceId}` - Get device details
- `POST /devices` - Register new device
- `DELETE /devices/{deviceId}` - Deregister device

**Telemetry Management**:
- `GET /telemetry/{deviceId}` - Get device telemetry (last 100 records)
- `POST /telemetry` - Submit telemetry data

#### Lambda Functions

**Device Lambda**:
- Runtime: Node.js 20.x
- Memory: 256 MB
- Timeout: 30 seconds
- Permissions: DynamoDB read/write, IoT describe

**Telemetry Lambda**:
- Runtime: Node.js 20.x
- Memory: 256 MB
- Timeout: 30 seconds
- Permissions: DynamoDB read/write

#### API Gateway Configuration
- Throttling: 1000 requests/sec, 2000 burst
- CORS: Enabled for all origins
- Logging: CloudWatch Logs
- Metrics: Enabled
- Stage: prod

## Data Flow

### Device Registration Flow
1. Client sends POST to `/devices` with device information
2. API Gateway routes to Device Lambda
3. Lambda validates input and creates device record
4. Device record stored in DynamoDB Devices table
5. Response returned with device details

### Telemetry Submission Flow
1. IoT device publishes telemetry to MQTT topic
2. IoT Rule (future) triggers Lambda or direct DynamoDB write
3. Alternative: Device calls POST `/telemetry` via REST API
4. Telemetry Lambda processes and validates data
5. Data stored in DynamoDB Telemetry table with TTL

### Website Access Flow
1. User requests website URL
2. Request hits CloudFront edge location
3. CloudFront checks cache
4. If cache miss, retrieves from S3
5. Content cached at edge and served to user

## Security Architecture

### Defense in Depth

**Layer 1: Network**
- CloudFront DDoS protection
- WAF integration (optional)

**Layer 2: Authentication**
- IoT device certificates
- API Gateway IAM authentication (configurable)

**Layer 3: Authorization**
- IoT device policies (least privilege)
- Lambda execution roles (scoped permissions)

**Layer 4: Encryption**
- Data in transit: TLS 1.2+
- Data at rest: AWS-managed encryption

**Layer 5: Monitoring**
- CloudWatch Logs
- CloudWatch Metrics
- CloudTrail (account-level)

## Scalability

### Horizontal Scaling
- **Lambda**: Automatic concurrent execution scaling
- **API Gateway**: Handles millions of requests
- **DynamoDB**: Auto-scaling (pay-per-request)
- **CloudFront**: Global edge network

### Performance Optimization
- CloudFront caching reduces origin load
- DynamoDB GSIs for efficient queries
- Lambda warm-up (future enhancement)
- API Gateway caching (optional)

## High Availability

### Redundancy
- Multi-AZ by default (Lambda, DynamoDB, API Gateway)
- S3 11 9's durability
- CloudFront multi-region

### Disaster Recovery
- DynamoDB point-in-time recovery
- S3 versioning
- CloudFormation for infrastructure recreation

## Cost Optimization

### Zero Fixed Monthly Costs

This infrastructure achieves **$0 fixed monthly costs** when idle through 100% serverless, pay-per-use services.

### Cost Analysis Tools

For detailed cost information, see:
- **[Complete Cost Analysis](COST_ANALYSIS.md)** - Comprehensive cost breakdown, scenarios, and ROI analysis
- **[Cost Tags Reference](COST_TAGS.md)** - Cost allocation tags for tracking and budgeting
- **Cost Analysis Script**: Run `./scripts/cost-analysis.sh` for interactive cost estimates

### Quick Cost Estimates

| Usage Level | Monthly Cost |
|-------------|--------------|
| Development/Testing | $1-5 |
| Small Production (100 devices) | $50-100 |
| Large Production (1,000 devices) | $500-1,000 |

### Implemented Optimizations

1. ✅ **DynamoDB**: Pay-per-request (no idle costs, no provisioned capacity waste)
2. ✅ **Lambda**: Pay per invocation (no idle costs, right-sized at 256 MB)
3. ✅ **S3**: Lifecycle policies enabled for versioning cleanup
4. ✅ **CloudFront**: Price class 100 (US, Europe, Israel) for cost efficiency
5. ✅ **TTL**: Automatic deletion of old telemetry data (30-day retention)
6. ✅ **API Throttling**: Prevents cost spikes from abuse
7. ✅ **Short Log Retention**: 1-week retention on Lambda logs

### Cost Allocation Tags

All resources are tagged for detailed cost tracking:
- `Project`: BRAVO (groups all project resources)
- `Stage`: dev/prod (separates environments)
- `CostCenter`: IoT, Storage, WebHosting, API (service categories)
- `Service`: Specific service identifiers (e.g., Lambda-Devices, DynamoDB-Telemetry)

### Monitoring
- AWS Cost Explorer (track costs by service and tag)
- Budget alerts (recommended: 50%, 80%, 100% thresholds)
- Resource tagging for cost allocation
- CloudWatch metrics for usage tracking

## Monitoring and Observability

### CloudWatch Integration
- Lambda execution logs
- API Gateway access logs
- DynamoDB metrics (read/write capacity, throttles)
- CloudFront access logs (optional)

### Key Metrics
- API latency (p50, p95, p99)
- Lambda errors and duration
- DynamoDB consumed capacity
- CloudFront cache hit ratio

## Deployment Strategy

### CI/CD Pipeline
1. Code pushed to main branch
2. GitHub Actions triggered
3. CDK synthesis (CloudFormation templates)
4. Automated deployment to AWS
5. Stack outputs captured

### Blue-Green Deployment
- Future: API Gateway stages
- Future: Lambda aliases and versions

### Rollback Strategy
- CloudFormation stack rollback
- Lambda version pinning
- S3 object versioning

## Future Enhancements

### Phase 2
- [ ] IoT Rules for real-time telemetry processing
- [ ] Cognito for user authentication
- [ ] Custom domain for CloudFront
- [ ] API Gateway custom domain
- [ ] WAF for API protection

### Phase 3
- [ ] Step Functions for complex workflows
- [ ] EventBridge for event-driven architecture
- [ ] QuickSight for analytics dashboard
- [ ] Lambda@Edge for edge compute
- [ ] Kinesis for streaming analytics

### Phase 4
- [ ] Multi-region deployment
- [ ] Global Accelerator
- [ ] Advanced monitoring with X-Ray
- [ ] Cost optimization with Savings Plans

## Compliance and Governance

### Best Practices
- All resources tagged (Project, Stage, ManagedBy)
- Encryption at rest and in transit
- Least privilege IAM policies
- Resource retention policies
- Audit logging with CloudTrail

### AWS Well-Architected Framework
- ✅ Operational Excellence: IaC, monitoring, CI/CD
- ✅ Security: Encryption, IAM, network isolation
- ✅ Reliability: Multi-AZ, backups, auto-scaling
- ✅ Performance Efficiency: Serverless, CDN, caching
- ✅ Cost Optimization: Pay-per-use, right-sizing

## Support and Maintenance

### Regular Tasks
- Review CloudWatch logs for errors
- Monitor API Gateway throttling
- Check DynamoDB consumed capacity
- Review AWS Cost Explorer

### Incident Response
1. Check CloudWatch alarms
2. Review Lambda errors in logs
3. Check API Gateway 5xx errors
4. Verify DynamoDB throttling
5. Check CloudFront error rate

## References

- [AWS CDK Documentation](https://docs.aws.amazon.com/cdk/)
- [AWS IoT Core Developer Guide](https://docs.aws.amazon.com/iot/)
- [DynamoDB Best Practices](https://docs.aws.amazon.com/amazondynamodb/latest/developerguide/best-practices.html)
- [CloudFront Best Practices](https://docs.aws.amazon.com/AmazonCloudFront/latest/DeveloperGuide/best-practices.html)
