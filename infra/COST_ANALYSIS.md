# B.R.A.V.O. Infrastructure Cost Analysis

## Overview

This document provides a comprehensive cost analysis for the B.R.A.V.O. (Bluetooth Radio Advanced Visual Orchestration) AWS infrastructure. The infrastructure is designed using serverless, pay-per-use services to minimize fixed costs and scale efficiently with demand.

## Cost Analysis Summary

### Key Finding: Zero Fixed Monthly Costs

The B.R.A.V.O. infrastructure has **no fixed monthly costs** when idle. All services use pay-per-use pricing models, making it highly cost-efficient for development, testing, and variable production workloads.

## Service-by-Service Cost Breakdown

### 1. AWS IoT Core

**Pricing Model**: Pay-per-use

| Component | Pricing | Notes |
|-----------|---------|-------|
| Device Connectivity | $0.08 per million connection-minutes | Billed only when devices are connected |
| Messaging | $1.00 per million messages | For MQTT publish/subscribe operations |
| Device Shadow Operations | $1.25 per million operations | If using device shadows (not currently implemented) |
| Rules Engine Actions | $0.15 per million actions | For IoT rules (future enhancement) |

**Cost Optimization**:
- ✅ Devices only connect when needed
- ✅ Efficient MQTT topic design minimizes message count
- ✅ No device shadows currently (can add if needed)

**Estimated Monthly Cost**:
- 10 devices, 24/7 connected, 1,000 messages/day: **~$3.50**
- 100 devices, 24/7 connected, 10,000 messages/day: **~$35**
- 1,000 devices, 24/7 connected, 100,000 messages/day: **~$350**

### 2. Amazon DynamoDB

**Pricing Model**: Pay-per-request (On-Demand)

| Component | Pricing | Notes |
|-----------|---------|-------|
| Write Request Units | $1.25 per million writes | Up to 1 KB per write |
| Read Request Units | $0.25 per million reads | Up to 4 KB per read |
| Data Storage | $0.25 per GB-month | All data stored |
| Point-in-Time Recovery | $0.20 per GB-month | Enabled for both tables |
| DynamoDB Streams | $0.02 per 100,000 read requests | Enabled on Device table |

**Tables**:
1. **Device Table**: Stores device registry and configuration
2. **Telemetry Table**: Stores time-series telemetry data (30-day TTL)

**Cost Optimization**:
- ✅ Pay-per-request mode (no provisioned capacity waste)
- ✅ TTL enabled on Telemetry table (auto-deletes old data)
- ✅ Efficient data model minimizes read/write operations
- ✅ GSIs use projection to minimize storage

**Estimated Monthly Cost**:
- Light usage (10K reads, 5K writes/day, 100 MB storage): **~$1.50**
- Moderate usage (100K reads, 50K writes/day, 1 GB storage): **~$15**
- Heavy usage (1M reads, 500K writes/day, 10 GB storage): **~$150**

### 3. AWS Lambda

**Pricing Model**: Pay-per-invocation + compute time

| Component | Pricing | Free Tier |
|-----------|---------|-----------|
| Requests | $0.20 per 1M requests | 1M requests/month |
| Compute (GB-seconds) | $0.0000166667 per GB-second | 400,000 GB-seconds/month |

**Functions**:
1. **Device Function**: Handles device CRUD operations (256 MB, ~100ms avg)
2. **Telemetry Function**: Handles telemetry data (256 MB, ~100ms avg)

**Cost Optimization**:
- ✅ Right-sized memory allocation (256 MB)
- ✅ Fast execution times (~100ms) minimize compute costs
- ✅ Efficient code reduces cold starts
- ✅ Free tier covers light to moderate usage

**Estimated Monthly Cost**:
- Within free tier for most development workloads: **$0.00**
- 100K invocations/month (beyond free tier): **~$0.40**
- 1M invocations/month: **~$4**

### 4. Amazon API Gateway

**Pricing Model**: Pay-per-request

| Component | Pricing | Free Tier |
|-----------|---------|-----------|
| REST API Requests | $3.50 per million requests | 1M requests/month (12 months) |
| Data Transfer Out | $0.09 per GB (first 10 TB) | Included with AWS free tier |

**API Endpoints**: 6 endpoints (devices CRUD + telemetry)

**Cost Optimization**:
- ✅ REST API (cheaper than WebSocket for this use case)
- ✅ Request throttling prevents abuse
- ✅ Efficient endpoint design minimizes unnecessary calls
- 💡 Consider enabling caching for frequently accessed data

**Estimated Monthly Cost**:
- Light usage (3K requests/day): **$0.32/month**
- Moderate usage (30K requests/day): **$3.15/month**
- Heavy usage (300K requests/day): **$31.50/month**

### 5. Amazon S3

**Pricing Model**: Pay-per-use (storage + requests)

| Component | Pricing | Notes |
|-----------|---------|-------|
| Standard Storage | $0.023 per GB-month | First 50 TB |
| PUT/POST Requests | $0.005 per 1,000 requests | For uploads |
| GET Requests | $0.0004 per 1,000 requests | For downloads |
| Data Transfer Out | $0.09 per GB | To internet (first 10 TB) |

**Usage**: Static website hosting

**Cost Optimization**:
- ✅ Block all public access (CloudFront only)
- ✅ Versioning for rollback capability
- ✅ Server-side encryption (no extra cost)
- 💡 Consider S3 Lifecycle policies for old versions
- 💡 Consider S3 Intelligent-Tiering for infrequently accessed content

**Estimated Monthly Cost**:
- Small website (100 MB, 1K requests/month): **~$0.03**
- Medium website (1 GB, 10K requests/month): **~$0.25**
- Large website (10 GB, 100K requests/month): **~$2.50**

Note: Data transfer is typically served by CloudFront, not S3 directly.

### 6. Amazon CloudFront

**Pricing Model**: Pay-per-use (data transfer + requests)

| Component | Pricing | Free Tier |
|-----------|---------|-----------|
| Data Transfer Out (US/EU) | $0.085 per GB (first 10 TB) | 1 TB/month (12 months) |
| HTTPS Requests | $0.0100 per 10,000 requests | 10M requests/month (12 months) |
| HTTP Requests | $0.0075 per 10,000 requests | N/A (we use HTTPS only) |

**Price Class**: PRICE_CLASS_100 (US, Europe, Israel) - Most cost-effective for these regions

**Cost Optimization**:
- ✅ Price Class 100 (limited regions for lower cost)
- ✅ Gzip compression reduces data transfer
- ✅ Efficient caching reduces origin requests
- ✅ Origin Access Identity (no S3 transfer costs to CloudFront)
- 💡 Monitor cache hit ratio to optimize caching

**Estimated Monthly Cost**:
- Light traffic (1 GB transfer, 100K requests): **~$0.18**
- Moderate traffic (50 GB transfer, 5M requests): **~$4.75**
- Heavy traffic (500 GB transfer, 50M requests): **~$47.50**

### 7. Amazon CloudWatch

**Pricing Model**: Pay-per-use

| Component | Pricing | Free Tier |
|-----------|---------|-----------|
| Logs Ingestion | $0.50 per GB | 5 GB/month |
| Logs Storage | $0.03 per GB-month | N/A |
| Metrics | $0.30 per custom metric | 10 custom metrics |
| Dashboards | $3.00 per dashboard/month | 3 dashboards |

**Usage**: Lambda logs, API Gateway logs, metrics

**Cost Optimization**:
- ✅ Log retention set to 1 week for Lambda functions
- ✅ Structured logging for efficient parsing
- 💡 Consider using log retention policies
- 💡 Monitor log volume to optimize verbosity

**Estimated Monthly Cost**:
- Light usage (500 MB logs/month): **~$0.50**
- Moderate usage (5 GB logs/month): **~$2.50**
- Heavy usage (50 GB logs/month): **~$25**

## Total Cost Estimates by Scenario

### Scenario 1: Development/Testing

**Characteristics**:
- 10 IoT devices connected intermittently
- 100 API requests per day
- 1,000 telemetry messages per day
- 100 MB website storage
- 1 GB website data transfer per month
- Minimal logging

**Monthly Cost Breakdown**:
| Service | Cost |
|---------|------|
| IoT Core | $0.50 |
| DynamoDB | $0.50 |
| Lambda | $0.00 (free tier) |
| API Gateway | $0.00 (free tier) |
| S3 | $0.03 |
| CloudFront | $0.00 (free tier) |
| CloudWatch | $0.00 (free tier) |
| **TOTAL** | **~$1-3/month** |

### Scenario 2: Small Production

**Characteristics**:
- 100 IoT devices connected 24/7
- 10,000 API requests per day
- 50,000 telemetry messages per day
- 500 MB website storage
- 50 GB website data transfer per month
- Moderate logging

**Monthly Cost Breakdown**:
| Service | Cost |
|---------|------|
| IoT Core | $35 |
| DynamoDB | $15 |
| Lambda | $2 |
| API Gateway | $3 |
| S3 | $0.15 |
| CloudFront | $4.50 |
| CloudWatch | $1.50 |
| **TOTAL** | **~$60/month** |

### Scenario 3: Large Production

**Characteristics**:
- 1,000 IoT devices connected 24/7
- 100,000 API requests per day
- 1 million telemetry messages per day
- 2 GB website storage
- 500 GB website data transfer per month
- Heavy logging

**Monthly Cost Breakdown**:
| Service | Cost |
|---------|------|
| IoT Core | $350 |
| DynamoDB | $150 |
| Lambda | $20 |
| API Gateway | $31 |
| S3 | $0.50 |
| CloudFront | $47 |
| CloudWatch | $10 |
| **TOTAL** | **~$600/month** |

## Cost Optimization Strategies

### Immediate Optimizations (Already Implemented)

1. ✅ **Serverless Architecture**: No idle costs, pay only for actual usage
2. ✅ **DynamoDB On-Demand**: No wasted provisioned capacity
3. ✅ **TTL on Telemetry Data**: Automatically deletes data after 30 days
4. ✅ **CloudFront Price Class 100**: Lower cost for US/Europe regions
5. ✅ **Lambda Right-Sizing**: 256 MB memory is optimal for these functions
6. ✅ **S3 Block Public Access**: Prevents unwanted data transfer costs
7. ✅ **API Gateway Throttling**: Prevents cost spikes from abuse
8. ✅ **Short Log Retention**: 1-week retention on Lambda logs

### Future Optimizations (Recommended)

1. 💡 **API Gateway Caching**: Reduce Lambda invocations by caching responses
   - Potential savings: 20-40% on API Gateway + Lambda costs
   - Implementation: Add caching to frequently accessed endpoints

2. 💡 **CloudFront Cache Optimization**: Increase cache TTL where appropriate
   - Potential savings: 10-20% on origin requests
   - Implementation: Review and optimize cache behaviors

3. 💡 **DynamoDB Reserved Capacity**: For consistent high workloads
   - Potential savings: 40-60% on DynamoDB costs for predictable traffic
   - Implementation: Switch from on-demand to provisioned with reserved capacity

4. 💡 **S3 Intelligent-Tiering**: Automatically moves objects to cheaper tiers
   - Potential savings: 20-40% on S3 storage costs
   - Implementation: Enable on S3 bucket for website assets

5. 💡 **Savings Plans**: Commit to 1-3 year Lambda/Fargate usage
   - Potential savings: Up to 17% on Lambda costs
   - Implementation: Use AWS Savings Plans calculator

6. 💡 **CloudWatch Logs Optimization**: Reduce log verbosity, use sampling
   - Potential savings: 30-50% on CloudWatch costs
   - Implementation: Review log statements, implement log sampling

7. 💡 **IoT Device Grouping**: Batch messages to reduce message count
   - Potential savings: 20-30% on IoT Core messaging costs
   - Implementation: Batch telemetry data on device before publishing

## Cost Monitoring and Alerting

### Setting Up AWS Budgets

Create a budget alert to monitor costs:

```bash
# Create a budget.json file
cat > budget.json << EOF
{
  "BudgetName": "BRAVO-Monthly-Budget",
  "BudgetLimit": {
    "Amount": "100",
    "Unit": "USD"
  },
  "TimeUnit": "MONTHLY",
  "BudgetType": "COST",
  "CostFilters": {
    "TagKeyValue": ["user:Project$BRAVO"]
  }
}
EOF

# Create notifications.json
cat > notifications.json << EOF
[
  {
    "Notification": {
      "NotificationType": "ACTUAL",
      "ComparisonOperator": "GREATER_THAN",
      "Threshold": 80,
      "ThresholdType": "PERCENTAGE"
    },
    "Subscribers": [
      {
        "SubscriptionType": "EMAIL",
        "Address": "your-email@example.com"
      }
    ]
  }
]
EOF

# Create the budget
aws budgets create-budget \
  --account-id YOUR_ACCOUNT_ID \
  --budget file://budget.json \
  --notifications-with-subscribers file://notifications.json
```

### Using AWS Cost Explorer

1. Navigate to: https://console.aws.amazon.com/cost-management/home
2. Enable Cost Explorer (if not already enabled)
3. Create custom reports:
   - **By Service**: See which services cost the most
   - **By Tag**: Filter by Project=BRAVO, Stage=dev/prod
   - **Daily Costs**: Monitor spending trends
   - **Forecast**: Project future costs

### CloudWatch Cost Dashboards

Create dashboards to monitor usage metrics:

```typescript
// Example: Add to CDK stack
import * as cloudwatch from 'aws-cdk-lib/aws-cloudwatch';

const dashboard = new cloudwatch.Dashboard(this, 'CostDashboard', {
  dashboardName: 'BRAVO-Cost-Metrics'
});

dashboard.addWidgets(
  new cloudwatch.GraphWidget({
    title: 'Lambda Invocations',
    left: [deviceLambda.metricInvocations()],
  }),
  new cloudwatch.GraphWidget({
    title: 'DynamoDB Consumed Capacity',
    left: [
      deviceTable.metricConsumedReadCapacityUnits(),
      deviceTable.metricConsumedWriteCapacityUnits(),
    ],
  })
);
```

### Key Metrics to Monitor

| Metric | Alert Threshold | Action |
|--------|-----------------|--------|
| Lambda invocations | > 1M/day | Review for unexpected traffic |
| DynamoDB read/write units | > 100K/hour | Check for inefficient queries |
| API Gateway requests | > 50K/day | Review API usage patterns |
| CloudFront data transfer | > 100 GB/day | Check for large file downloads |
| IoT messages | > 1M/day | Verify device behavior |

## Cost Allocation Tags

All resources are tagged for cost allocation:

| Tag | Value | Purpose |
|-----|-------|---------|
| Project | BRAVO | Group all project resources |
| Stage | dev/prod | Separate dev and production costs |
| ManagedBy | CDK | Identify infrastructure-as-code resources |

Enable these tags in AWS Cost Allocation Tags settings to track costs by tag.

## Cost Comparison: Alternative Architectures

### Current Architecture (Serverless)
- **Fixed Cost**: $0/month
- **Variable Cost**: Scales with usage
- **Best For**: Variable workloads, dev/test, small-medium production

### Alternative: EC2-Based Architecture
- **Fixed Cost**: ~$50-200/month (t3.medium instances)
- **Variable Cost**: Lower per-request cost at high volume
- **Best For**: Consistent high-volume workloads

### Alternative: Container-Based (ECS/Fargate)
- **Fixed Cost**: ~$30-100/month (minimum tasks running)
- **Variable Cost**: Scales with usage
- **Best For**: Medium-large workloads with predictable traffic

**Recommendation**: Current serverless architecture is optimal for this use case due to variable workload patterns and zero fixed costs.

## Cost Analysis Tools

### 1. AWS Pricing Calculator
- URL: https://calculator.aws/
- Use to model different scenarios and architectures
- Export estimates for documentation

### 2. CDK Cost Analysis
```bash
# Run cost analysis script
./scripts/cost-analysis.sh

# Synthesize CloudFormation template for manual review
npm run synth
```

### 3. AWS Cost Explorer API
```bash
# Get current month's costs
START_DATE=$(date -u +%Y-%m-01)
END_DATE=$(date -u +%Y-%m-%d)

aws ce get-cost-and-usage \
  --time-period Start=$START_DATE,End=$END_DATE \
  --granularity MONTHLY \
  --metrics "UnblendedCost" \
  --group-by Type=TAG,Key=Project \
  --region us-east-1
```

### 4. Third-Party Tools
- **CloudHealth**: Multi-cloud cost management
- **CloudCheckr**: Cost optimization recommendations
- **Kubecost**: Kubernetes cost analysis (if using EKS)

## ROI Analysis

### Cost Savings vs. Self-Hosted Infrastructure

| Item | Self-Hosted | AWS Serverless | Savings |
|------|-------------|----------------|---------|
| Hardware | $5,000 upfront | $0 | $5,000 |
| Maintenance | $500/month | $0 | $500/month |
| Electricity | $100/month | $0 | $100/month |
| Network | $200/month | Included | $200/month |
| Personnel | $50/hour × 20 hours/month | $50/hour × 2 hours/month | $900/month |
| **Total Monthly** | **~$1,700** | **~$60-600** | **~$1,100+** |

### Break-Even Analysis

AWS serverless becomes more expensive than self-hosted at:
- **~$1,500/month** in AWS costs
- This corresponds to **~5,000 devices** at full utilization
- For most use cases, serverless is significantly cheaper

## Compliance and Governance

### Cost Governance Best Practices

1. ✅ **Resource Tagging**: All resources tagged consistently
2. ✅ **Least Privilege**: IAM policies minimize unnecessary resource usage
3. ✅ **Retention Policies**: Data automatically deleted after retention period
4. 💡 **Budget Alerts**: Set up alerts at 50%, 80%, 100% of budget
5. 💡 **Regular Reviews**: Monthly cost review meetings
6. 💡 **Cost Allocation**: Charge back to departments/projects

### FinOps Principles

Following FinOps best practices:
1. **Inform**: Provide cost visibility to all stakeholders
2. **Optimize**: Continuously improve cost efficiency
3. **Operate**: Align costs with business value

## Frequently Asked Questions

### Q: Why are there no fixed monthly costs?
**A**: All services use pay-per-use pricing. When idle (no devices, no requests), the infrastructure costs $0.

### Q: What happens if I exceed my budget?
**A**: Set up AWS Budgets with alerts. Consider implementing:
- API Gateway throttling (already enabled)
- Lambda concurrency limits
- CloudWatch alarms on cost metrics

### Q: Can I reduce costs further?
**A**: Yes, see "Future Optimizations" section. Main opportunities:
- API Gateway caching
- DynamoDB reserved capacity (for consistent workloads)
- CloudFront cache optimization

### Q: How do I track costs per environment (dev/prod)?
**A**: Use the `Stage` tag to filter costs in AWS Cost Explorer.

### Q: What's the most expensive service?
**A**: Typically IoT Core for connected devices, followed by DynamoDB for high-write workloads.

### Q: Are there hidden costs?
**A**: Minimal:
- Data transfer between AZs (usually negligible)
- CloudWatch Logs storage (controlled by retention)
- DynamoDB point-in-time recovery (already included in estimates)

## Summary

The B.R.A.V.O. infrastructure is designed for cost efficiency:

- ✅ **$0 fixed monthly costs** when idle
- ✅ **Scales efficiently** with actual usage
- ✅ **Predictable pricing** based on device count and API requests
- ✅ **Already optimized** with serverless best practices
- 💡 **Further optimization opportunities** available

**Estimated Cost Ranges**:
- **Development**: $1-5/month
- **Small Production**: $50-100/month
- **Large Production**: $500-1000/month

For most use cases, this represents **60-90% cost savings** compared to traditional self-hosted infrastructure.

## Additional Resources

- [AWS Pricing Calculator](https://calculator.aws/)
- [AWS Cost Explorer](https://console.aws.amazon.com/cost-management/home)
- [AWS Cost Optimization](https://aws.amazon.com/pricing/cost-optimization/)
- [AWS Well-Architected Framework - Cost Optimization](https://docs.aws.amazon.com/wellarchitected/latest/cost-optimization-pillar/welcome.html)
- [FinOps Foundation](https://www.finops.org/)
