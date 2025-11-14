# B.R.A.V.O. Infrastructure Deployment Checklist

Use this checklist to ensure a smooth deployment process.

## Pre-Deployment Checklist

### Local Environment
- [ ] Node.js 18+ installed (`node --version`)
- [ ] npm installed (`npm --version`)
- [ ] AWS CLI installed (`aws --version`)
- [ ] Git installed (`git --version`)

### AWS Account Preparation
- [ ] AWS account created
- [ ] AWS CLI configured (`aws configure`)
- [ ] AWS credentials verified (`aws sts get-caller-identity`)
- [ ] Sufficient permissions for:
  - [ ] CloudFormation
  - [ ] IAM
  - [ ] IoT Core
  - [ ] DynamoDB
  - [ ] S3
  - [ ] CloudFront
  - [ ] Lambda
  - [ ] API Gateway
  - [ ] CloudWatch Logs

### Configuration
- [ ] Update `config/stack-config.json` with your AWS account ID
- [ ] Review and adjust region in `config/stack-config.json`
- [ ] Review project name and stage
- [ ] (Optional) Copy `.env.example` to `.env` and customize

## Deployment Checklist

### Step 1: Setup
- [ ] Clone repository
- [ ] Run `npm install`
- [ ] Run `npm run build`
- [ ] Run `./scripts/validate.sh` (should pass all checks)

### Step 2: CDK Bootstrap (First-time only)
- [ ] Run `npx cdk bootstrap`
- [ ] Verify bootstrap stack in CloudFormation console

### Step 3: Review Stacks
- [ ] Run `npx cdk synth` to generate CloudFormation templates
- [ ] Review templates in `cdk.out/` directory
- [ ] Verify resource definitions match requirements
- [ ] Check for any security warnings

### Step 4: Deploy Infrastructure
- [ ] Run `./scripts/deploy.sh` OR `npm run deploy`
- [ ] Monitor deployment progress
- [ ] Wait for all stacks to complete (15-20 minutes)
- [ ] Verify all stacks show `CREATE_COMPLETE` status

### Step 5: Verify Deployment
- [ ] Check CloudFormation console for stack outputs
- [ ] Record important values:
  - [ ] IoT Endpoint: `___________________________`
  - [ ] API Gateway Endpoint: `___________________________`
  - [ ] CloudFront URL: `___________________________`
  - [ ] S3 Bucket Name: `___________________________`

### Step 6: Test Deployment

#### Test API Gateway
- [ ] Get API endpoint from stack outputs
- [ ] Test device creation:
  ```bash
  curl -X POST $API_ENDPOINT/devices \
    -H "Content-Type: application/json" \
    -d '{"deviceId":"test-001","status":"active"}'
  ```
- [ ] Test device listing:
  ```bash
  curl $API_ENDPOINT/devices
  ```
- [ ] Test telemetry submission:
  ```bash
  curl -X POST $API_ENDPOINT/telemetry \
    -H "Content-Type: application/json" \
    -d '{"deviceId":"test-001","metricType":"test","value":100}'
  ```

#### Verify DynamoDB
- [ ] Open DynamoDB console
- [ ] Check `BRAVO-Devices-dev` table exists
- [ ] Check `BRAVO-Telemetry-dev` table exists
- [ ] Verify test data from API tests

#### Verify S3/CloudFront
- [ ] Open S3 console
- [ ] Verify bucket created
- [ ] Upload test `index.html` file
- [ ] Access CloudFront URL in browser

#### Verify IoT Core
- [ ] Open IoT Core console
- [ ] Verify Thing Type created
- [ ] Verify Policy created
- [ ] (Optional) Create test IoT Thing

## Post-Deployment Checklist

### Monitoring Setup
- [ ] Open CloudWatch console
- [ ] Review log groups created
- [ ] Set up CloudWatch alarms (optional)
- [ ] Configure billing alerts

### Security Review
- [ ] Review IAM roles and policies
- [ ] Verify encryption enabled on all resources
- [ ] Check S3 bucket policies
- [ ] Review IoT device policies

### Documentation
- [ ] Document stack outputs in team wiki
- [ ] Share API endpoints with team
- [ ] Update any external documentation
- [ ] Record deployment date and version

### Backup and Disaster Recovery
- [ ] Verify DynamoDB point-in-time recovery enabled
- [ ] Verify S3 versioning enabled
- [ ] Document rollback procedure
- [ ] Test backup restoration (optional)

## GitHub Actions Setup (Optional)

If using automated deployment:

- [ ] Configure AWS OIDC provider in AWS account
- [ ] Create IAM role for GitHub Actions
- [ ] Add GitHub repository secrets:
  - [ ] `AWS_ROLE_ARN`
  - [ ] `AWS_REGION`
- [ ] Test workflow by pushing to main branch
- [ ] Verify automatic deployment succeeds

## Troubleshooting

If deployment fails:

1. Check CloudFormation Events
   - [ ] Open AWS CloudFormation console
   - [ ] Find failed stack
   - [ ] Review Events tab for error messages

2. Check CloudWatch Logs
   - [ ] Identify failing resource
   - [ ] Check associated log group
   - [ ] Review error messages

3. Common Issues
   - [ ] Insufficient IAM permissions → Add required permissions
   - [ ] Resource name conflicts → Update stack names
   - [ ] Region-specific limits → Request limit increase
   - [ ] CDK version mismatch → Update CDK: `npm update aws-cdk-lib`

## Cleanup Checklist

When tearing down infrastructure:

- [ ] Backup any important data from DynamoDB
- [ ] Download any files from S3 bucket
- [ ] Run `./scripts/destroy.sh`
- [ ] Confirm destruction
- [ ] Manually delete retained resources if needed:
  - [ ] CloudWatch Log Groups
  - [ ] S3 Bucket (if contains data)
  - [ ] DynamoDB Tables (if contains data)

## Success Criteria

Deployment is successful when:

- ✅ All 4 CDK stacks deployed (`CREATE_COMPLETE`)
- ✅ API endpoints responding to requests
- ✅ DynamoDB tables accessible and queryable
- ✅ CloudFront serving content
- ✅ No errors in CloudWatch Logs
- ✅ All stack outputs captured

## Next Steps After Deployment

1. **Integrate with Frontend**
   - Use API Gateway endpoint in web application
   - Upload website files to S3 bucket

2. **Connect IoT Devices**
   - Create IoT Things for each device
   - Attach certificates and policies
   - Configure MQTT connection

3. **Set Up Monitoring**
   - Create CloudWatch dashboards
   - Configure alarms for critical metrics
   - Set up SNS notifications

4. **Implement CI/CD**
   - Configure GitHub Actions
   - Test automated deployments
   - Document deployment process

5. **Optimize Costs**
   - Review AWS Cost Explorer
   - Implement resource tagging
   - Consider Reserved Capacity for predictable workloads

## Support

For issues or questions:
- Review [README.md](README.md)
- Check [ARCHITECTURE.md](ARCHITECTURE.md)
- Consult [QUICKSTART.md](QUICKSTART.md)
- Open an issue in the repository
