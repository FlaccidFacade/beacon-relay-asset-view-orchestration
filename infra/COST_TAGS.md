# Cost Allocation Tags Reference

This document lists all cost allocation tags used in the B.R.A.V.O. infrastructure for cost tracking and analysis.

## Standard Tags (All Resources)

These tags are automatically applied to all resources in the infrastructure:

| Tag | Value | Purpose |
|-----|-------|---------|
| `Project` | `BRAVO` | Group all resources for the B.R.A.V.O. project |
| `Stage` | `dev`/`prod` | Differentiate between development and production environments |
| `ManagedBy` | `CDK` | Identify infrastructure managed by AWS CDK |

## Cost Center Tags

Additional granular tags for cost tracking by service type:

### IoT Core Stack
| Resource | CostCenter | Service |
|----------|------------|---------|
| Thing Type | `BRAVO-IoT` | `IoT-Core` |
| Device Policy | `BRAVO-IoT` | `IoT-Core` |

### DynamoDB Stack
| Resource | CostCenter | Service |
|----------|------------|---------|
| Device Table | `BRAVO-Storage` | `DynamoDB-Devices` |
| Telemetry Table | `BRAVO-Storage` | `DynamoDB-Telemetry` |

### S3/CloudFront Stack
| Resource | CostCenter | Service |
|----------|------------|---------|
| S3 Bucket | `BRAVO-WebHosting` | `S3-Website` |
| CloudFront Distribution | `BRAVO-WebHosting` | `CloudFront-CDN` |

### Lambda/API Gateway Stack
| Resource | CostCenter | Service |
|----------|------------|---------|
| Device Lambda Function | `BRAVO-API` | `Lambda-Devices` |
| Telemetry Lambda Function | `BRAVO-API` | `Lambda-Telemetry` |
| API Gateway | `BRAVO-API` | `API-Gateway` |

## Cost Centers Summary

| Cost Center | Services Included | Purpose |
|-------------|-------------------|---------|
| `BRAVO-IoT` | IoT Core | Device connectivity and messaging |
| `BRAVO-Storage` | DynamoDB Tables | Data storage for devices and telemetry |
| `BRAVO-WebHosting` | S3, CloudFront | Static website hosting and CDN |
| `BRAVO-API` | Lambda, API Gateway | REST API backend services |

## Using Cost Allocation Tags

### 1. Enable Cost Allocation Tags in AWS

1. Navigate to AWS Billing Console → Cost Allocation Tags
2. Activate the following tags:
   - `Project`
   - `Stage`
   - `ManagedBy`
   - `CostCenter`
   - `Service`
3. Wait 24 hours for tags to appear in Cost Explorer

### 2. Query Costs by Tag

#### Using AWS CLI

```bash
# Get costs by Project
aws ce get-cost-and-usage \
  --time-period Start=2024-01-01,End=2024-01-31 \
  --granularity MONTHLY \
  --metrics "UnblendedCost" \
  --group-by Type=TAG,Key=Project \
  --filter file://filter.json

# Get costs by CostCenter
aws ce get-cost-and-usage \
  --time-period Start=2024-01-01,End=2024-01-31 \
  --granularity MONTHLY \
  --metrics "UnblendedCost" \
  --group-by Type=TAG,Key=CostCenter
```

#### Using AWS Cost Explorer Console

1. Navigate to Cost Explorer
2. Create a new report
3. Group by: **Tag** → Select `CostCenter` or `Service`
4. Filter by: **Tag** → `Project` = `BRAVO`
5. Filter by: **Tag** → `Stage` = `dev` or `prod`

### 3. Create Cost Reports

Example reports you can create:

1. **Total BRAVO Project Cost**: Filter by `Project=BRAVO`
2. **Development vs Production**: Group by `Stage`
3. **Cost by Service Category**: Group by `CostCenter`
4. **Detailed Service Breakdown**: Group by `Service`

### 4. Set Up Budget Alerts by Tag

Create budgets for specific cost centers:

```bash
# Example: Budget for API services
cat > api-budget.json << EOF
{
  "BudgetName": "BRAVO-API-Budget",
  "BudgetLimit": {
    "Amount": "50",
    "Unit": "USD"
  },
  "TimeUnit": "MONTHLY",
  "BudgetType": "COST",
  "CostFilters": {
    "TagKeyValue": ["user:CostCenter$BRAVO-API"]
  }
}
EOF

aws budgets create-budget \
  --account-id YOUR_ACCOUNT_ID \
  --budget file://api-budget.json
```

## Cost Analysis Queries

### Query Examples

#### 1. Total Monthly Cost by Cost Center
```sql
-- Using AWS Athena on Cost and Usage Report
SELECT 
  resource_tags_user_cost_center as CostCenter,
  SUM(line_item_unblended_cost) as TotalCost
FROM cur_database.cost_and_usage_report
WHERE resource_tags_user_project = 'BRAVO'
  AND year = '2024'
  AND month = '01'
GROUP BY resource_tags_user_cost_center
ORDER BY TotalCost DESC;
```

#### 2. Cost Breakdown by Service
```sql
SELECT 
  resource_tags_user_service as Service,
  resource_tags_user_cost_center as CostCenter,
  SUM(line_item_unblended_cost) as TotalCost
FROM cur_database.cost_and_usage_report
WHERE resource_tags_user_project = 'BRAVO'
  AND year = '2024'
  AND month = '01'
GROUP BY resource_tags_user_service, resource_tags_user_cost_center
ORDER BY TotalCost DESC;
```

#### 3. Development vs Production Cost Comparison
```sql
SELECT 
  resource_tags_user_stage as Stage,
  SUM(line_item_unblended_cost) as TotalCost
FROM cur_database.cost_and_usage_report
WHERE resource_tags_user_project = 'BRAVO'
  AND year = '2024'
  AND month = '01'
GROUP BY resource_tags_user_stage;
```

## Best Practices

1. **Consistent Tagging**: All resources are automatically tagged via CDK
2. **Tag Governance**: Use AWS Config to enforce tagging policies
3. **Regular Reviews**: Review cost allocation reports monthly
4. **Budget Alerts**: Set up alerts at 50%, 80%, and 100% thresholds
5. **Cost Optimization**: Use tags to identify high-cost services for optimization

## Troubleshooting

### Tags Not Appearing in Cost Explorer

**Problem**: Cost allocation tags don't show up in reports

**Solution**:
1. Activate tags in Billing Console → Cost Allocation Tags
2. Wait 24 hours for tags to propagate
3. Deploy new resources after activation

### Missing Tags on Resources

**Problem**: Some resources don't have expected tags

**Solution**:
1. Rebuild and redeploy: `npm run build && npm run deploy`
2. Verify tags in CloudFormation template: `npm run synth`
3. Check AWS resource directly in console

### Cost Explorer Not Showing Detailed Data

**Problem**: Cost data is aggregated, not detailed

**Solution**:
1. Enable Cost and Usage Report (CUR) in AWS Billing
2. Use Athena to query detailed cost data
3. Use tags to filter and group as needed

## Additional Resources

- [AWS Cost Allocation Tags](https://docs.aws.amazon.com/awsaccountbilling/latest/aboutv2/cost-alloc-tags.html)
- [AWS Cost Explorer](https://docs.aws.amazon.com/cost-management/latest/userguide/ce-what-is.html)
- [Cost and Usage Report](https://docs.aws.amazon.com/cur/latest/userguide/what-is-cur.html)
- [AWS Tagging Best Practices](https://docs.aws.amazon.com/whitepapers/latest/tagging-best-practices/tagging-best-practices.html)
