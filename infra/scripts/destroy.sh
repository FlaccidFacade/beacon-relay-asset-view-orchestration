#!/bin/bash

# B.R.A.V.O. Infrastructure Destroy Script
# This script destroys all CDK stacks from AWS

set -e

echo "======================================"
echo "B.R.A.V.O. Infrastructure Destruction"
echo "======================================"
echo ""
echo "WARNING: This will delete all infrastructure resources!"
echo ""

read -p "Are you sure you want to destroy all stacks? (yes/no): " confirm
if [ "$confirm" != "yes" ]; then
    echo "Destruction cancelled."
    exit 0
fi

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

echo "Destroying from AWS Account: $AWS_ACCOUNT"
echo "Destroying from AWS Region: $AWS_REGION"
echo ""

# Destroy all stacks
echo "Destroying all stacks..."
npx cdk destroy --all --force
echo ""

echo "======================================"
echo "Destruction completed!"
echo "======================================"
