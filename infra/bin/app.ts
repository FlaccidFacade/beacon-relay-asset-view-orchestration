#!/usr/bin/env node
import 'source-map-support/register';
import * as cdk from 'aws-cdk-lib';
import { IoTCoreStack } from '../cdk/stacks/iot-core-stack';
import { DynamoDBStack } from '../cdk/stacks/dynamodb-stack';
import { S3CloudFrontStack } from '../cdk/stacks/s3-cloudfront-stack';
import { LambdaApiGatewayStack } from '../cdk/stacks/lambda-api-gateway-stack';
import * as config from '../config/stack-config.json';

const app = new cdk.App();

// Get environment from context or use default
const env = {
  account: process.env.CDK_DEFAULT_ACCOUNT || config.aws.account,
  region: process.env.CDK_DEFAULT_REGION || config.aws.region,
};

// IoT Core Stack - manages IoT devices and policies
const iotStack = new IoTCoreStack(app, `${config.projectName}-IoTCore`, {
  env,
  description: 'IoT Core stack for B.R.A.V.O - manages IoT devices, thing types, and policies',
  projectName: config.projectName,
  stage: config.stage,
});

// DynamoDB Stack - stores device data and telemetry
const dynamoDbStack = new DynamoDBStack(app, `${config.projectName}-DynamoDB`, {
  env,
  description: 'DynamoDB stack for B.R.A.V.O - stores device data and telemetry',
  projectName: config.projectName,
  stage: config.stage,
});

// S3/CloudFront Stack - hosts static website
const s3CloudFrontStack = new S3CloudFrontStack(app, `${config.projectName}-S3CloudFront`, {
  env,
  description: 'S3/CloudFront stack for B.R.A.V.O - hosts static website with CDN',
  projectName: config.projectName,
  stage: config.stage,
});

// Lambda/API Gateway Stack - provides REST API endpoints
const lambdaApiStack = new LambdaApiGatewayStack(app, `${config.projectName}-LambdaAPI`, {
  env,
  description: 'Lambda/API Gateway stack for B.R.A.V.O - provides REST API endpoints',
  projectName: config.projectName,
  stage: config.stage,
  deviceTable: dynamoDbStack.deviceTable,
  telemetryTable: dynamoDbStack.telemetryTable,
  iotEndpoint: iotStack.iotEndpoint,
});

// Add dependencies
lambdaApiStack.addDependency(dynamoDbStack);
lambdaApiStack.addDependency(iotStack);

// Tag all resources
cdk.Tags.of(app).add('Project', config.projectName);
cdk.Tags.of(app).add('Stage', config.stage);
cdk.Tags.of(app).add('ManagedBy', 'CDK');

app.synth();
