# AWS Setup Guide for B.R.A.V.O API

This guide walks through setting up the required AWS infrastructure for the B.R.A.V.O API.

## Prerequisites

- AWS Account
- AWS CLI installed and configured
- Node.js 20.x installed
- Basic understanding of AWS Lambda and API Gateway

## Step-by-Step Setup

### 1. Create IAM Role for Lambda

#### Create Trust Policy
Create a file `lambda-trust-policy.json`:

```json
{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Effect": "Allow",
      "Principal": {
        "Service": "lambda.amazonaws.com"
      },
      "Action": "sts:AssumeRole"
    }
  ]
}
```

#### Create IAM Role
```bash
aws iam create-role \
  --role-name bravo-api-lambda-role \
  --assume-role-policy-document file://lambda-trust-policy.json
```

#### Attach Basic Execution Policy
```bash
aws iam attach-role-policy \
  --role-name bravo-api-lambda-role \
  --policy-arn arn:aws:iam::aws:policy/service-role/AWSLambdaBasicExecutionRole
```

#### Create Custom Policy for Additional Permissions
Create `lambda-permissions-policy.json`:

```json
{
  "Version": "2012-10-17",
  "Statement": [
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
        "s3:GetObject",
        "s3:PutObject"
      ],
      "Resource": "arn:aws:s3:::bravo-firmware/*"
    }
  ]
}
```

```bash
aws iam create-policy \
  --policy-name bravo-api-lambda-permissions \
  --policy-document file://lambda-permissions-policy.json

aws iam attach-role-policy \
  --role-name bravo-api-lambda-role \
  --policy-arn arn:aws:iam::YOUR_ACCOUNT_ID:policy/bravo-api-lambda-permissions
```

### 2. Create DynamoDB Tables (Recommended for Production)

#### Devices Table
```bash
aws dynamodb create-table \
  --table-name bravo-devices \
  --attribute-definitions \
    AttributeName=deviceId,AttributeType=S \
  --key-schema \
    AttributeName=deviceId,KeyType=HASH \
  --billing-mode PAY_PER_REQUEST \
  --tags Key=Project,Value=BRAVO-API
```

#### Telemetry Table
```bash
aws dynamodb create-table \
  --table-name bravo-telemetry \
  --attribute-definitions \
    AttributeName=telemetryId,AttributeType=S \
    AttributeName=deviceId,AttributeType=S \
    AttributeName=timestamp,AttributeType=S \
  --key-schema \
    AttributeName=telemetryId,KeyType=HASH \
  --global-secondary-indexes \
    IndexName=DeviceIndex,KeySchema=["{AttributeName=deviceId,KeyType=HASH}","{AttributeName=timestamp,KeyType=RANGE}"],Projection="{ProjectionType=ALL}" \
  --billing-mode PAY_PER_REQUEST \
  --tags Key=Project,Value=BRAVO-API
```

#### OTA Updates Table
```bash
aws dynamodb create-table \
  --table-name bravo-ota-updates \
  --attribute-definitions \
    AttributeName=updateId,AttributeType=S \
    AttributeName=deviceId,AttributeType=S \
    AttributeName=createdAt,AttributeType=S \
  --key-schema \
    AttributeName=updateId,KeyType=HASH \
  --global-secondary-indexes \
    IndexName=DeviceIndex,KeySchema=["{AttributeName=deviceId,KeyType=HASH}","{AttributeName=createdAt,KeyType=RANGE}"],Projection="{ProjectionType=ALL}" \
  --billing-mode PAY_PER_REQUEST \
  --tags Key=Project,Value=BRAVO-API
```

### 3. Create S3 Bucket for Firmware Storage

```bash
aws s3 mb s3://bravo-firmware-YOUR_ACCOUNT_ID

# Enable versioning
aws s3api put-bucket-versioning \
  --bucket bravo-firmware-YOUR_ACCOUNT_ID \
  --versioning-configuration Status=Enabled

# Add bucket policy for Lambda access
cat > firmware-bucket-policy.json <<EOF
{
  "Version": "2012-10-17",
  "Statement": [
    {
      "Sid": "AllowLambdaRead",
      "Effect": "Allow",
      "Principal": {
        "AWS": "arn:aws:iam::YOUR_ACCOUNT_ID:role/bravo-api-lambda-role"
      },
      "Action": [
        "s3:GetObject",
        "s3:ListBucket"
      ],
      "Resource": [
        "arn:aws:s3:::bravo-firmware-YOUR_ACCOUNT_ID/*",
        "arn:aws:s3:::bravo-firmware-YOUR_ACCOUNT_ID"
      ]
    }
  ]
}
EOF

aws s3api put-bucket-policy \
  --bucket bravo-firmware-YOUR_ACCOUNT_ID \
  --policy file://firmware-bucket-policy.json
```

### 4. Build and Deploy Lambda Functions

#### Build the Project
```bash
npm install
npm run build
```

#### Package Lambda Functions
```bash
cd dist
npm install --production --no-package-lock
zip -r ../lambda.zip .
cd ..
```

#### Deploy REST API Lambda
```bash
# Get the role ARN
ROLE_ARN=$(aws iam get-role --role-name bravo-api-lambda-role --query 'Role.Arn' --output text)

# Create Lambda function
aws lambda create-function \
  --function-name bravo-api-rest \
  --runtime nodejs20.x \
  --role $ROLE_ARN \
  --handler lambda/rest.handler \
  --zip-file fileb://lambda.zip \
  --timeout 30 \
  --memory-size 512 \
  --environment Variables="{NODE_ENV=production,AWS_REGION=us-east-1}" \
  --tags Project=BRAVO-API
```

#### Deploy GraphQL API Lambda
```bash
aws lambda create-function \
  --function-name bravo-api-graphql \
  --runtime nodejs20.x \
  --role $ROLE_ARN \
  --handler lambda/graphql.handler \
  --zip-file fileb://lambda.zip \
  --timeout 30 \
  --memory-size 512 \
  --environment Variables="{NODE_ENV=production,AWS_REGION=us-east-1}" \
  --tags Project=BRAVO-API
```

### 5. Create API Gateway

#### Create REST API
```bash
# Create API
API_ID=$(aws apigateway create-rest-api \
  --name "BRAVO-API" \
  --description "B.R.A.V.O IoT Device Management API" \
  --endpoint-configuration types=REGIONAL \
  --query 'id' \
  --output text)

echo "API ID: $API_ID"

# Get root resource ID
ROOT_ID=$(aws apigateway get-resources \
  --rest-api-id $API_ID \
  --query 'items[?path==`/`].id' \
  --output text)

echo "Root Resource ID: $ROOT_ID"
```

#### Create Resources and Methods for REST API

```bash
# Get Lambda ARNs
REST_LAMBDA_ARN=$(aws lambda get-function \
  --function-name bravo-api-rest \
  --query 'Configuration.FunctionArn' \
  --output text)

ACCOUNT_ID=$(aws sts get-caller-identity --query Account --output text)
REGION=$(aws configure get region)

# Create {proxy+} resource for REST API
PROXY_ID=$(aws apigateway create-resource \
  --rest-api-id $API_ID \
  --parent-id $ROOT_ID \
  --path-part '{proxy+}' \
  --query 'id' \
  --output text)

# Create ANY method on proxy resource
aws apigateway put-method \
  --rest-api-id $API_ID \
  --resource-id $PROXY_ID \
  --http-method ANY \
  --authorization-type NONE

# Set up Lambda integration
aws apigateway put-integration \
  --rest-api-id $API_ID \
  --resource-id $PROXY_ID \
  --http-method ANY \
  --type AWS_PROXY \
  --integration-http-method POST \
  --uri "arn:aws:apigateway:${REGION}:lambda:path/2015-03-31/functions/${REST_LAMBDA_ARN}/invocations"

# Grant API Gateway permission to invoke Lambda
aws lambda add-permission \
  --function-name bravo-api-rest \
  --statement-id apigateway-rest \
  --action lambda:InvokeFunction \
  --principal apigateway.amazonaws.com \
  --source-arn "arn:aws:execute-api:${REGION}:${ACCOUNT_ID}:${API_ID}/*/*"
```

#### Create GraphQL Resource

```bash
GRAPHQL_LAMBDA_ARN=$(aws lambda get-function \
  --function-name bravo-api-graphql \
  --query 'Configuration.FunctionArn' \
  --output text)

# Create /graphql resource
GRAPHQL_ID=$(aws apigateway create-resource \
  --rest-api-id $API_ID \
  --parent-id $ROOT_ID \
  --path-part 'graphql' \
  --query 'id' \
  --output text)

# Create POST method
aws apigateway put-method \
  --rest-api-id $API_ID \
  --resource-id $GRAPHQL_ID \
  --http-method POST \
  --authorization-type NONE

# Set up Lambda integration
aws apigateway put-integration \
  --rest-api-id $API_ID \
  --resource-id $GRAPHQL_ID \
  --http-method POST \
  --type AWS_PROXY \
  --integration-http-method POST \
  --uri "arn:aws:apigateway:${REGION}:lambda:path/2015-03-31/functions/${GRAPHQL_LAMBDA_ARN}/invocations"

# Grant permission
aws lambda add-permission \
  --function-name bravo-api-graphql \
  --statement-id apigateway-graphql \
  --action lambda:InvokeFunction \
  --principal apigateway.amazonaws.com \
  --source-arn "arn:aws:execute-api:${REGION}:${ACCOUNT_ID}:${API_ID}/*/POST/graphql"
```

#### Deploy API
```bash
aws apigateway create-deployment \
  --rest-api-id $API_ID \
  --stage-name prod \
  --stage-description "Production Stage" \
  --description "Initial deployment"

# Get API endpoint
echo "API Endpoint: https://${API_ID}.execute-api.${REGION}.amazonaws.com/prod"
```

### 6. Configure GitHub Secrets

Add these secrets to your GitHub repository:

```bash
# Go to: https://github.com/YOUR_ORG/api/settings/secrets/actions

# Add the following secrets:
# - AWS_ACCESS_KEY_ID: Your AWS access key
# - AWS_SECRET_ACCESS_KEY: Your AWS secret key
# - AWS_LAMBDA_ROLE_ARN: The Lambda role ARN (get it with the command below)

aws iam get-role --role-name bravo-api-lambda-role --query 'Role.Arn' --output text
```

### 7. Enable CORS (Optional)

If your API will be called from web browsers:

```bash
# Enable CORS on the API
aws apigateway put-method \
  --rest-api-id $API_ID \
  --resource-id $PROXY_ID \
  --http-method OPTIONS \
  --authorization-type NONE

aws apigateway put-integration \
  --rest-api-id $API_ID \
  --resource-id $PROXY_ID \
  --http-method OPTIONS \
  --type MOCK \
  --request-templates '{"application/json": "{\"statusCode\": 200}"}'

aws apigateway put-integration-response \
  --rest-api-id $API_ID \
  --resource-id $PROXY_ID \
  --http-method OPTIONS \
  --status-code 200 \
  --response-parameters '{
    "method.response.header.Access-Control-Allow-Headers": "'"'"'Content-Type,X-Amz-Date,Authorization,X-Api-Key,X-Amz-Security-Token'"'"'",
    "method.response.header.Access-Control-Allow-Methods": "'"'"'GET,POST,PUT,PATCH,DELETE,OPTIONS'"'"'",
    "method.response.header.Access-Control-Allow-Origin": "'"'"'*'"'"'"
  }'

aws apigateway put-method-response \
  --rest-api-id $API_ID \
  --resource-id $PROXY_ID \
  --http-method OPTIONS \
  --status-code 200 \
  --response-parameters '{
    "method.response.header.Access-Control-Allow-Headers": true,
    "method.response.header.Access-Control-Allow-Methods": true,
    "method.response.header.Access-Control-Allow-Origin": true
  }'

# Redeploy the API
aws apigateway create-deployment \
  --rest-api-id $API_ID \
  --stage-name prod
```

### 8. Set Up CloudWatch Alarms

```bash
# Create SNS topic for alarms
TOPIC_ARN=$(aws sns create-topic \
  --name bravo-api-alarms \
  --query 'TopicArn' \
  --output text)

# Subscribe to the topic
aws sns subscribe \
  --topic-arn $TOPIC_ARN \
  --protocol email \
  --notification-endpoint your-email@example.com

# Create alarm for Lambda errors
aws cloudwatch put-metric-alarm \
  --alarm-name bravo-api-rest-errors \
  --alarm-description "Alert on Lambda errors" \
  --metric-name Errors \
  --namespace AWS/Lambda \
  --statistic Sum \
  --period 300 \
  --threshold 5 \
  --comparison-operator GreaterThanThreshold \
  --evaluation-periods 1 \
  --dimensions Name=FunctionName,Value=bravo-api-rest \
  --alarm-actions $TOPIC_ARN

# Create alarm for high latency
aws cloudwatch put-metric-alarm \
  --alarm-name bravo-api-rest-latency \
  --alarm-description "Alert on high latency" \
  --metric-name Duration \
  --namespace AWS/Lambda \
  --statistic Average \
  --period 300 \
  --threshold 5000 \
  --comparison-operator GreaterThanThreshold \
  --evaluation-periods 2 \
  --dimensions Name=FunctionName,Value=bravo-api-rest \
  --alarm-actions $TOPIC_ARN
```

### 9. Test the Deployment

```bash
# Get your API endpoint
API_ENDPOINT="https://${API_ID}.execute-api.${REGION}.amazonaws.com/prod"

# Test device registration
curl -X POST ${API_ENDPOINT}/devices \
  -H "Content-Type: application/json" \
  -d '{"name":"Test Device","type":"sensor","firmwareVersion":"1.0.0"}'

# Test GraphQL
curl -X POST ${API_ENDPOINT}/graphql \
  -H "Content-Type: application/json" \
  -d '{"query":"query { devices { deviceId name status } }"}'
```

## Clean Up Resources

To remove all resources:

```bash
# Delete Lambda functions
aws lambda delete-function --function-name bravo-api-rest
aws lambda delete-function --function-name bravo-api-graphql

# Delete API Gateway
aws apigateway delete-rest-api --rest-api-id $API_ID

# Delete DynamoDB tables
aws dynamodb delete-table --table-name bravo-devices
aws dynamodb delete-table --table-name bravo-telemetry
aws dynamodb delete-table --table-name bravo-ota-updates

# Delete S3 bucket (must be empty first)
aws s3 rb s3://bravo-firmware-YOUR_ACCOUNT_ID --force

# Delete IAM role and policies
aws iam detach-role-policy \
  --role-name bravo-api-lambda-role \
  --policy-arn arn:aws:iam::aws:policy/service-role/AWSLambdaBasicExecutionRole
aws iam detach-role-policy \
  --role-name bravo-api-lambda-role \
  --policy-arn arn:aws:iam::YOUR_ACCOUNT_ID:policy/bravo-api-lambda-permissions
aws iam delete-policy --policy-arn arn:aws:iam::YOUR_ACCOUNT_ID:policy/bravo-api-lambda-permissions
aws iam delete-role --role-name bravo-api-lambda-role

# Delete CloudWatch alarms
aws cloudwatch delete-alarms --alarm-names bravo-api-rest-errors bravo-api-rest-latency

# Delete SNS topic
aws sns delete-topic --topic-arn $TOPIC_ARN
```

## Troubleshooting

### Lambda Function Errors
```bash
# View recent logs
aws logs tail /aws/lambda/bravo-api-rest --follow

# Get function configuration
aws lambda get-function-configuration --function-name bravo-api-rest
```

### API Gateway Issues
```bash
# Test invoke Lambda directly
aws lambda invoke \
  --function-name bravo-api-rest \
  --payload '{"httpMethod":"GET","path":"/devices"}' \
  response.json

cat response.json
```

### Permission Issues
```bash
# Check Lambda execution role
aws iam get-role --role-name bravo-api-lambda-role

# List attached policies
aws iam list-attached-role-policies --role-name bravo-api-lambda-role
```

## Cost Estimation

**Monthly costs for moderate usage:**
- Lambda: 1M requests, 512MB, 1s avg duration = ~$5
- API Gateway: 1M requests = ~$3.50
- DynamoDB: 1M writes, 1M reads (on-demand) = ~$2
- S3: 10GB storage, 1000 downloads = ~$0.30
- CloudWatch: Logs and metrics = ~$1

**Total: ~$12/month**

For production workloads, costs will vary based on:
- Request volume
- Lambda execution time
- Data storage
- Data transfer

Use AWS Cost Explorer and set up billing alerts to monitor costs.
