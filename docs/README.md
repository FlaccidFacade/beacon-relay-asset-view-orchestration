# docs

repo for BOM, diagrams, and roadmap documents

- [ROADMAP.md](ROADMAP.md) - Project roadmap including stretch goals and implementation phases
- [DEV SETUP] - Development environment notes

## Development Setup

This repository uses [Prettier](https://prettier.io/) to ensure consistent code formatting across all documentation files.

### Prerequisites

- Node.js 20.x or higher
- npm

### Getting Started

1. Install dependencies:

   ```bash
   npm install
   ```

2. Format all files:

   ```bash
   npm run format
   ```

3. Check formatting (without modifying files):
   ```bash
   npm run format:check
   ```

### CI/CD

The repository includes a GitHub Actions workflow that automatically checks formatting on all pull requests and pushes to the main branch. All files must pass Prettier formatting checks before merging.

### Supported File Types

The Prettier configuration applies to the following file types:

- Markdown (`.md`)
- JSON (`.json`)
- YAML (`.yml`, `.yaml`)

### Contributing

Before committing changes, please ensure your files are properly formatted by running:

```bash
npm run format
```
