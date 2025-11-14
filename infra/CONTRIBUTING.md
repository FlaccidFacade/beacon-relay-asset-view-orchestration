# Contributing to B.R.A.V.O. Infrastructure

Thank you for your interest in contributing to the B.R.A.V.O. Infrastructure project!

## Development Setup

1. Fork the repository
2. Clone your fork: `git clone <your-fork-url>`
3. Install dependencies: `npm install`
4. Build the project: `npm run build`
5. Create a feature branch: `git checkout -b feature/your-feature-name`

## Making Changes

1. Make your changes to the CDK stacks in `cdk/stacks/`
2. Update configuration if needed in `config/stack-config.json`
3. Build and test: `npm run build && npm run synth`
4. Ensure no TypeScript errors

## Testing Changes

Before submitting a pull request:

1. **Synthesize stacks**: `npm run synth`
2. **Review CloudFormation templates** in `cdk.out/`
3. **Check for errors**: Ensure synthesis completes without errors
4. **Test locally** if possible: Deploy to a test AWS account

## Submitting Changes

1. Commit your changes with a descriptive message
2. Push to your fork
3. Open a Pull Request against the `main` branch
4. Describe your changes in the PR description
5. Wait for review and address any feedback

## Code Style

- Follow TypeScript best practices
- Use meaningful variable and function names
- Add comments for complex logic
- Keep stack definitions modular and focused

## Stack Conventions

- Each stack should be self-contained
- Use CDK Outputs for cross-stack references
- Tag all resources appropriately
- Follow AWS Well-Architected Framework principles

## Documentation

- Update README.md if you add new features
- Document new configuration options
- Add comments to complex code sections

## Security

- Never commit AWS credentials or secrets
- Use IAM roles with least-privilege principle
- Enable encryption for all data stores
- Review security implications of changes

## Questions?

Open an issue for questions or clarifications.
