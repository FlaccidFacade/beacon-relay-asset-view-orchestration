# Quick Start Guide

Get your B.R.A.V.O. infrastructure up and running in minutes.

## Prerequisites Checklist

- [ ] Node.js 18+ installed
- [ ] AWS CLI installed and configured
- [ ] AWS account with appropriate permissions
- [ ] Git installed

## 5-Minute Setup

### 1. Clone and Install (2 minutes)

```bash
git clone https://github.com/beacon-relay-asset-view-orchestration/infra
cd infra
npm install
```

### 2. Configure AWS (1 minute)

```bash
# Set your AWS credentials
aws configure

# Verify credentials
aws sts get-caller-identity
```

### 3. Update Configuration (1 minute)

Edit `config/stack-config.json`:
```json
{
  "aws": {
    "account": "YOUR_AWS_ACCOUNT_ID",
    "region": "us-east-1"
  }
}
```

Replace `YOUR_AWS_ACCOUNT_ID` with your actual AWS account ID from step 2.

### 4. Deploy (1 minute)

```bash
# Bootstrap CDK (first time only)
npx cdk bootstrap

# Deploy all stacks
./scripts/deploy.sh
```

That's it! Your infrastructure is now deployed.

## What Gets Created?

- ✅ IoT Core infrastructure for device management
- ✅ DynamoDB tables for data storage
- ✅ S3 bucket and CloudFront distribution for web hosting
- ✅ Lambda functions and API Gateway for REST API

## Next Steps

1. **View Outputs**: Check AWS CloudFormation console for stack outputs
2. **Test API**: Use the API endpoint to test device and telemetry endpoints
3. **Deploy Website**: Upload your website files to the S3 bucket
4. **Connect Devices**: Configure IoT devices using the IoT endpoint

## Testing Your Deployment

```bash
# Get your API endpoint
API_ENDPOINT=$(aws cloudformation describe-stacks \
  --stack-name BRAVO-LambdaAPI \
  --query 'Stacks[0].Outputs[?OutputKey==`ApiEndpoint`].OutputValue' \
  --output text)

# Test device creation
curl -X POST $API_ENDPOINT/devices \
  -H "Content-Type: application/json" \
  -d '{"deviceId":"test-001","status":"active"}'

# List devices
curl $API_ENDPOINT/devices
```

## Troubleshooting

### "CDK not bootstrapped" error
```bash
npx cdk bootstrap
```

### "Credentials not found" error
```bash
aws configure
```

### "Permission denied" on scripts
```bash
chmod +x scripts/*.sh
```

## Clean Up

To remove all resources:
```bash
./scripts/destroy.sh
```

## Need Help?

- Check the full [README.md](README.md) for detailed documentation
- Review [COST_ANALYSIS.md](COST_ANALYSIS.md) for cost estimates and optimization
- Review [CONTRIBUTING.md](CONTRIBUTING.md) for development guidelines
- Open an issue for questions

## Cost Estimates

Want to know how much this will cost? Run:

```bash
./scripts/cost-analysis.sh
# or
npm run cost-analysis
```

**Quick estimate**: $1-5/month for development, $50-100/month for small production.

See [COST_ANALYSIS.md](COST_ANALYSIS.md) for detailed cost information.
