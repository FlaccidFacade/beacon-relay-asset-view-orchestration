# Quick Start Guide

Get the B.R.A.V.O API up and running in 5 minutes!

## 🚀 Local Development

### 1. Prerequisites

- Node.js 20.x or later
- npm 10.x or later

### 2. Install Dependencies

```bash
npm install
```

### 3. Build the Project

```bash
npm run build
```

### 4. Verify Everything Works

```bash
npm run lint
npm run typecheck
```

**✅ Success!** Your project is built and ready for deployment.

## 📦 What You Just Built

### Project Structure

```
api/
├── src/
│   ├── lambda/           # Lambda handlers (REST & GraphQL)
│   ├── handlers/         # Business logic (devices, telemetry, OTA)
│   ├── utils/            # Utilities (response, validation, database)
│   ├── types/            # TypeScript definitions
│   └── graphql/          # GraphQL schema & resolvers
├── dist/                 # Compiled JavaScript (after build)
├── README.md             # Full documentation
├── ARCHITECTURE.md       # System architecture
├── AWS_SETUP.md          # AWS deployment guide
└── examples.md           # API usage examples
```

### API Endpoints

#### REST API

- `POST /devices` - Register a device
- `GET /devices` - List all devices
- `GET /devices/{id}` - Get device details
- `DELETE /devices/{id}` - Delete device
- `POST /telemetry` - Submit telemetry
- `GET /telemetry/{id}` - Get telemetry
- `GET /devices/{id}/telemetry` - Get device telemetry
- `POST /ota-updates` - Create OTA update
- `GET /ota-updates/{id}` - Get OTA update
- `PATCH /ota-updates/{id}/status` - Update OTA status
- `GET /devices/{id}/ota-updates` - Get device OTA updates

#### GraphQL API

- `POST /graphql` - GraphQL endpoint with queries and mutations

## 🔧 Development Commands

```bash
# Clean build artifacts
npm run clean

# Build TypeScript to JavaScript
npm run build

# Run linter
npm run lint

# Fix linting issues automatically
npm run lint:fix

# Type check without building
npm run typecheck

# Package for Lambda deployment
npm run package
```

## ☁️ Deploy to AWS

### Option 1: Automated (GitHub Actions)

1. **Set up GitHub Secrets**:
   - `AWS_ACCESS_KEY_ID`
   - `AWS_SECRET_ACCESS_KEY`
   - `AWS_LAMBDA_ROLE_ARN`

2. **Push to main branch**:

   ```bash
   git push origin main
   ```

3. **Done!** GitHub Actions will automatically build and deploy.

### Option 2: Manual Deployment

Follow the detailed guide in [AWS_SETUP.md](AWS_SETUP.md)

**Quick version:**

```bash
# Build and package
npm run build
cd dist && npm install --production --no-package-lock
zip -r ../lambda.zip .
cd ..

# Deploy (requires AWS CLI configured)
aws lambda update-function-code \
  --function-name bravo-api-rest \
  --zip-file fileb://lambda.zip

aws lambda update-function-code \
  --function-name bravo-api-graphql \
  --zip-file fileb://lambda.zip
```

## 🧪 Test Your API

### Test REST API (after deployment)

```bash
API_URL="https://YOUR_API_ID.execute-api.us-east-1.amazonaws.com/prod"

# Register a device
curl -X POST $API_URL/devices \
  -H "Content-Type: application/json" \
  -d '{
    "name": "Test Sensor",
    "type": "temperature",
    "firmwareVersion": "1.0.0"
  }'

# List devices
curl $API_URL/devices
```

### Test GraphQL API

```bash
# Open GraphQL Playground in browser
open $API_URL/graphql

# Or use curl
curl -X POST $API_URL/graphql \
  -H "Content-Type: application/json" \
  -d '{
    "query": "query { devices { deviceId name status } }"
  }'
```

## 📚 Next Steps

### For Development

- [ ] Read [README.md](README.md) for detailed API documentation
- [ ] Review [examples.md](examples.md) for usage examples
- [ ] Explore [ARCHITECTURE.md](ARCHITECTURE.md) to understand the system

### For Production

- [ ] Follow [AWS_SETUP.md](AWS_SETUP.md) to set up AWS infrastructure
- [ ] Replace mock database with DynamoDB or RDS
- [ ] Set up authentication (AWS Cognito, API Keys, etc.)
- [ ] Configure CloudWatch alarms and monitoring
- [ ] Set up custom domain name for API Gateway
- [ ] Enable CloudWatch Logs for debugging

### For Customization

- [ ] Modify `src/types/index.ts` to add custom data models
- [ ] Update `src/handlers/` to add custom business logic
- [ ] Extend `src/graphql/schema.ts` with new queries/mutations
- [ ] Add authentication middleware in Lambda handlers
- [ ] Integrate with external services (SNS, SES, etc.)

## 🐛 Troubleshooting

### Build Errors

```bash
# Clear everything and rebuild
npm run clean
rm -rf node_modules package-lock.json
npm install
npm run build
```

### TypeScript Errors

```bash
# Check for type errors
npm run typecheck

# Common issues:
# - Missing type definitions: npm install --save-dev @types/package-name
# - Outdated dependencies: npm update
```

### Linting Errors

```bash
# Auto-fix most issues
npm run lint:fix

# Check what can't be auto-fixed
npm run lint
```

### AWS Deployment Issues

```bash
# Test Lambda function locally
cd dist
node -e "const handler = require('./lambda/rest'); console.log(handler);"

# Check Lambda logs
aws logs tail /aws/lambda/bravo-api-rest --follow

# Verify IAM permissions
aws iam get-role --role-name bravo-api-lambda-role
```

## 🆘 Getting Help

- **Issues**: Open an issue on GitHub
- **Documentation**: Check README.md, ARCHITECTURE.md, AWS_SETUP.md
- **Examples**: See examples.md for code samples
- **AWS Docs**: https://docs.aws.amazon.com/lambda/

## 📝 Important Notes

### Current Database

The project uses an **in-memory mock database** for development. This means:

- ✅ No setup required for local development
- ✅ Easy testing and debugging
- ⚠️ Data is lost when Lambda restarts
- ❌ Not suitable for production

**For production**, replace the mock database in `src/utils/database.ts` with:

- DynamoDB (recommended for serverless)
- RDS (PostgreSQL/MySQL)
- Aurora Serverless

### Security Notes

- Currently, there's **no authentication** (development mode)
- For production, add authentication:
  - AWS Cognito
  - API Gateway API Keys
  - Custom Lambda authorizers
  - OAuth 2.0 / JWT

### Cost Considerations

- AWS Lambda Free Tier: 1M requests/month
- API Gateway Free Tier: 1M requests/month (first 12 months)
- DynamoDB Free Tier: 25GB storage + 25 RCU/WCU
- Typical monthly cost: $10-50 for moderate usage

## ✨ Features Implemented

- ✅ TypeScript with strict type checking
- ✅ REST API with full CRUD operations
- ✅ GraphQL API with queries and mutations
- ✅ Device registration and management
- ✅ Telemetry data collection
- ✅ OTA firmware update management
- ✅ Input validation
- ✅ Standardized API responses
- ✅ ESLint code quality checks
- ✅ AWS Lambda deployment ready
- ✅ GitHub Actions CI/CD pipeline
- ✅ Comprehensive documentation

## 🎯 What's Next?

The B.R.A.V.O API is fully functional and ready for deployment! Here are suggested enhancements:

1. **Add Authentication**: Secure your API with AWS Cognito
2. **Switch to Real Database**: Replace mock DB with DynamoDB
3. **Add WebSockets**: Real-time device updates via API Gateway WebSockets
4. **Device Shadows**: Cache device state for offline scenarios
5. **Analytics**: Add AWS IoT Analytics or Kinesis integration
6. **Notifications**: Integrate SNS for alerts
7. **Rate Limiting**: Add API throttling and quotas
8. **API Versioning**: Support multiple API versions
9. **Automated Tests**: Add unit and integration tests
10. **Monitoring Dashboard**: Create CloudWatch dashboard

---

**Ready to deploy?** Head to [AWS_SETUP.md](AWS_SETUP.md) for step-by-step AWS deployment instructions!
