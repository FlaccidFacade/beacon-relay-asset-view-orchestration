#!/bin/bash

# B.R.A.V.O. Infrastructure Deployment Script
# This script deploys all CDK stacks to AWS

set -e

echo "======================================"
echo "B.R.A.V.O. Infrastructure Deployment"
echo "======================================"
echo ""

# Check if AWS credentials are configured
if ! aws sts get-caller-identity &> /dev/null; then
    echo "Error: AWS credentials are not configured."
    echo "Please configure your AWS credentials using 'aws configure' or set environment variables."
    exit 1
fi

# Get AWS account and region
AWS_ACCOUNT=$(aws sts get-caller-identity --query Account --output text)
AWS_REGION=$(aws configure get region)
AWS_REGION=${AWS_REGION:-us-east-1}

echo "Deploying to AWS Account: $AWS_ACCOUNT"
echo "Deploying to AWS Region: $AWS_REGION"
echo ""

# Install dependencies if needed
if [ ! -d "node_modules" ]; then
    echo "Installing dependencies..."
    npm install
    echo ""
fi

# Build the project
echo "Building CDK project..."
npm run build
echo ""

# Bootstrap CDK (if not already done)
echo "Bootstrapping CDK..."
npx cdk bootstrap aws://$AWS_ACCOUNT/$AWS_REGION
echo ""

# Synthesize CloudFormation templates
echo "Synthesizing CloudFormation templates..."
npx cdk synth
echo ""

# Deploy all stacks
echo "Deploying all stacks..."
npx cdk deploy --all --require-approval never
echo ""

echo "======================================"
echo "Deployment completed successfully!"
echo "======================================"
echo ""
echo "To view stack outputs, run:"
echo "  aws cloudformation describe-stacks --region $AWS_REGION"
