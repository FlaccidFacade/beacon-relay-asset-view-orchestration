#!/bin/bash

# Cost Analysis Script for B.R.A.V.O. Infrastructure
# This script helps estimate and analyze infrastructure costs

set -e

echo "======================================"
echo "B.R.A.V.O. Infrastructure Cost Analysis"
echo "======================================"
echo ""

# Colors for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print section headers
print_header() {
    echo ""
    echo -e "${BLUE}=== $1 ===${NC}"
    echo ""
}

# Function to print cost items
print_cost() {
    echo -e "${GREEN}$1${NC}: $2"
}

print_header "Static Monthly Costs (Baseline)"

echo "These are estimated monthly costs for the B.R.A.V.O. infrastructure:"
echo ""

print_cost "IoT Core" "\$0.00 (only charged for messages/connections)"
print_cost "DynamoDB Tables" "\$0.00 (pay-per-request, no baseline)"
print_cost "Lambda Functions" "\$0.00 (pay-per-invocation)"
print_cost "API Gateway" "\$0.00 (pay-per-request)"
print_cost "S3 Bucket" "~\$0.023 per GB stored/month"
print_cost "CloudFront Distribution" "\$0.00 (pay-per-request)"

echo ""
echo -e "${YELLOW}Total Static Monthly Cost: ~\$0.00 (excluding storage)${NC}"
echo ""
echo "Note: This infrastructure uses serverless services with pay-per-use pricing."
echo "Costs scale with actual usage (requests, data transfer, storage)."

print_header "Variable Costs (Usage-Based)"

echo "DynamoDB (Pay-per-Request):"
echo "  - Write Request Units: \$1.25 per million writes"
echo "  - Read Request Units: \$0.25 per million reads"
echo "  - Data Storage: \$0.25 per GB-month"
echo ""

echo "Lambda:"
echo "  - Requests: \$0.20 per 1 million requests"
echo "  - Duration: \$0.0000166667 per GB-second"
echo "  - Free Tier: 1M requests + 400,000 GB-seconds per month"
echo ""

echo "API Gateway:"
echo "  - REST API: \$3.50 per million requests"
echo "  - Data Transfer Out: \$0.09 per GB (first 10 TB)"
echo ""

echo "IoT Core:"
echo "  - Connectivity: \$0.08 per million minutes"
echo "  - Messaging: \$1.00 per million messages"
echo ""

echo "CloudFront:"
echo "  - Data Transfer Out (US/EU): \$0.085 per GB (first 10 TB)"
echo "  - HTTP/HTTPS Requests: \$0.0075 per 10,000 requests"
echo ""

echo "S3:"
echo "  - Storage: \$0.023 per GB-month (Standard)"
echo "  - PUT/POST: \$0.005 per 1,000 requests"
echo "  - GET/SELECT: \$0.0004 per 1,000 requests"
echo ""

print_header "Example Usage Scenarios"

echo "Scenario 1: Light Usage (Development/Testing)"
echo "  - 10 devices connected 24/7"
echo "  - 100 API requests per day"
echo "  - 1,000 telemetry messages per day"
echo "  - 100 MB website storage"
echo "  - 1 GB website data transfer per month"
echo ""
echo "  Estimated Monthly Cost: \$2-5"
echo ""

echo "Scenario 2: Moderate Usage (Small Production)"
echo "  - 100 devices connected 24/7"
echo "  - 10,000 API requests per day"
echo "  - 50,000 telemetry messages per day"
echo "  - 500 MB website storage"
echo "  - 50 GB website data transfer per month"
echo ""
echo "  Estimated Monthly Cost: \$25-50"
echo ""

echo "Scenario 3: Heavy Usage (Large Production)"
echo "  - 1,000 devices connected 24/7"
echo "  - 100,000 API requests per day"
echo "  - 1 million telemetry messages per day"
echo "  - 2 GB website storage"
echo "  - 500 GB website data transfer per month"
echo ""
echo "  Estimated Monthly Cost: \$250-400"
echo ""

print_header "Cost Monitoring Recommendations"

echo "1. Enable AWS Cost Explorer:"
echo "   - Track costs by service"
echo "   - Set up cost allocation tags"
echo "   - Create custom cost reports"
echo ""

echo "2. Set Up Budget Alerts:"
echo "   aws budgets create-budget --account-id YOUR_ACCOUNT_ID \\"
echo "     --budget file://budget.json \\"
echo "     --notifications-with-subscribers file://notifications.json"
echo ""

echo "3. Use CloudWatch Metrics:"
echo "   - Monitor Lambda invocations"
echo "   - Track API Gateway requests"
echo "   - Watch DynamoDB consumed capacity"
echo ""

echo "4. Enable Cost Allocation Tags:"
echo "   - Project: BRAVO"
echo "   - Stage: dev/prod"
echo "   - ManagedBy: CDK"
echo ""

print_header "Cost Optimization Tips"

echo "✓ DynamoDB: Use TTL to automatically delete old telemetry data (already enabled)"
echo "✓ Lambda: Right-size memory allocation based on usage patterns"
echo "✓ CloudFront: Use appropriate price class (currently PRICE_CLASS_100)"
echo "✓ API Gateway: Implement caching to reduce Lambda invocations"
echo "✓ S3: Use lifecycle policies to transition old objects to cheaper storage"
echo "✓ Consider Reserved Capacity for consistent high-volume workloads"
echo ""

print_header "Checking Current AWS Costs (if AWS CLI configured)"

if command -v aws &> /dev/null; then
    echo "Attempting to retrieve current month's costs..."
    echo ""
    
    START_DATE=$(date -u +%Y-%m-01)
    END_DATE=$(date -u +%Y-%m-%d)
    
    if aws ce get-cost-and-usage \
        --time-period Start=$START_DATE,End=$END_DATE \
        --granularity MONTHLY \
        --metrics "UnblendedCost" \
        --region us-east-1 2>/dev/null; then
        echo ""
        echo "Run the above AWS Cost Explorer command to see actual costs."
    else
        echo "Unable to retrieve costs. Ensure AWS CLI is configured with Cost Explorer permissions."
    fi
else
    echo "AWS CLI not found. Install it to retrieve actual cost data."
fi

echo ""
print_header "Additional Resources"

echo "📊 AWS Cost Calculator: https://calculator.aws/"
echo "📈 AWS Cost Explorer: https://console.aws.amazon.com/cost-management/home"
echo "📖 AWS Pricing: https://aws.amazon.com/pricing/"
echo "💡 CDK Costs: https://aws.amazon.com/cdk/pricing/ (CDK itself is free)"
echo ""

echo "======================================"
echo "Cost Analysis Complete"
echo "======================================"
