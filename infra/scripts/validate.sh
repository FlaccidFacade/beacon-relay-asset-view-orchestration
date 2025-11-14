#!/bin/bash

# B.R.A.V.O Infrastructure Validation Script
# This script validates the CDK setup and AWS configuration

set -e

echo "======================================"
echo "B.R.A.V.O Infrastructure Validation"
echo "======================================"
echo ""

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check Node.js
echo -n "Checking Node.js... "
if command -v node &> /dev/null; then
    NODE_VERSION=$(node --version)
    echo -e "${GREEN}✓${NC} $NODE_VERSION"
else
    echo -e "${RED}✗${NC} Node.js not found"
    exit 1
fi

# Check npm
echo -n "Checking npm... "
if command -v npm &> /dev/null; then
    NPM_VERSION=$(npm --version)
    echo -e "${GREEN}✓${NC} $NPM_VERSION"
else
    echo -e "${RED}✗${NC} npm not found"
    exit 1
fi

# Check AWS CLI
echo -n "Checking AWS CLI... "
if command -v aws &> /dev/null; then
    AWS_VERSION=$(aws --version | cut -d' ' -f1)
    echo -e "${GREEN}✓${NC} $AWS_VERSION"
else
    echo -e "${RED}✗${NC} AWS CLI not found"
    exit 1
fi

# Check AWS credentials
echo -n "Checking AWS credentials... "
if aws sts get-caller-identity &> /dev/null; then
    AWS_ACCOUNT=$(aws sts get-caller-identity --query Account --output text)
    AWS_REGION=$(aws configure get region)
    AWS_REGION=${AWS_REGION:-us-east-1}
    echo -e "${GREEN}✓${NC} Account: $AWS_ACCOUNT, Region: $AWS_REGION"
else
    echo -e "${YELLOW}!${NC} AWS credentials not configured (required for deployment)"
    echo "  Configure with: aws configure"
fi

# Check if node_modules exists
echo -n "Checking dependencies... "
if [ -d "node_modules" ]; then
    echo -e "${GREEN}✓${NC} Dependencies installed"
else
    echo -e "${YELLOW}!${NC} Dependencies not installed. Run 'npm install'"
fi

# Check if TypeScript compiles
echo -n "Checking TypeScript build... "
if npm run build &> /dev/null; then
    echo -e "${GREEN}✓${NC} Build successful"
else
    echo -e "${RED}✗${NC} Build failed"
    exit 1
fi

# Check CDK synthesis
echo -n "Checking CDK synthesis... "
if npx cdk synth &> /tmp/cdk-synth.log; then
    STACK_COUNT=$(npx cdk list 2>/dev/null | wc -l)
    echo -e "${GREEN}✓${NC} $STACK_COUNT stacks found"
else
    echo -e "${RED}✗${NC} CDK synthesis failed"
    cat /tmp/cdk-synth.log
    exit 1
fi

# List stacks
echo ""
echo "Available stacks:"
npx cdk list 2>/dev/null | while read stack; do
    echo "  • $stack"
done

echo ""
echo "======================================"
echo -e "${GREEN}Validation completed successfully!${NC}"
echo "======================================"
echo ""
echo "You're ready to deploy. Run:"
echo "  ./scripts/deploy.sh"
