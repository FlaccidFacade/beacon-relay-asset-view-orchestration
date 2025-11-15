# VS Code Setup for B.R.A.V.O.

This directory contains VS Code workspace configuration to ensure a consistent development experience across the team.

## Quick Start

When you open this workspace in VS Code, you should see a notification to install the recommended extensions. Click "Install All" to get started quickly.

Alternatively, you can:
1. Open the Command Palette (`Ctrl+Shift+P` / `Cmd+Shift+P`)
2. Type "Show Recommended Extensions"
3. Click "Install Workspace Recommended Extensions"

## Recommended Extensions

The following extensions are recommended for this monorepo:

### TypeScript & JavaScript Development
- **ESLint** (`dbaeumer.vscode-eslint`) - Linting for JavaScript/TypeScript
- **Prettier** (`esbenp.prettier-vscode`) - Code formatting

### Next.js & React Development
- **Tailwind CSS IntelliSense** (`bradlc.vscode-tailwindcss`) - Auto-completion for Tailwind CSS

### AWS & Infrastructure
- **AWS Toolkit** (`amazonwebservices.aws-toolkit-vscode`) - AWS service integration and CDK support

### Embedded & Firmware Development
- **PlatformIO IDE** (`platformio.platformio-ide`) - ESP32 firmware development

### Android & Kotlin Development
- **Kotlin** (`mathiasfrohlich.kotlin`) - Kotlin language support
- **Java Extension Pack** (`vscjava.vscode-java-pack`) - Java and Android development tools

### Version Control
- **GitLens** (`eamodio.gitlens`) - Enhanced Git capabilities
- **GitHub Pull Requests** (`github.vscode-pull-request-github`) - Manage PRs from VS Code

### Documentation
- **Markdown All in One** (`yzhang.markdown-all-in-one`) - Markdown editing support
- **markdownlint** (`davidanson.vscode-markdownlint`) - Markdown linting

### General Development
- **EditorConfig** (`editorconfig.editorconfig`) - Maintain consistent coding styles
- **Live Server** (`ms-vscode.live-server`) - Local development server
- **Path Intellisense** (`christian-kohler.path-intellisense`) - Auto-complete file paths
- **IntelliCode** (`visualstudioexptteam.vscodeintellicode`) - AI-assisted code completion

## Workspace Settings

The workspace settings (`settings.json`) include:
- Auto-formatting on save with Prettier
- ESLint auto-fix on save
- TypeScript workspace version
- File associations for Arduino/PlatformIO files
- Optimized search and file watching exclusions

## Manual Installation

If you prefer to install extensions manually, run this command in the terminal:

```bash
# Install all recommended extensions at once
code --install-extension dbaeumer.vscode-eslint \
     --install-extension esbenp.prettier-vscode \
     --install-extension bradlc.vscode-tailwindcss \
     --install-extension amazonwebservices.aws-toolkit-vscode \
     --install-extension platformio.platformio-ide \
     --install-extension mathiasfrohlich.kotlin \
     --install-extension vscjava.vscode-java-pack \
     --install-extension eamodio.gitlens \
     --install-extension github.vscode-pull-request-github \
     --install-extension yzhang.markdown-all-in-one \
     --install-extension davidanson.vscode-markdownlint \
     --install-extension editorconfig.editorconfig \
     --install-extension ms-vscode.live-server \
     --install-extension christian-kohler.path-intellisense \
     --install-extension visualstudioexptteam.vscodeintellicode
```

## Project-Specific Notes

### API (`/api`)
- Uses TypeScript, ESLint, and Prettier
- AWS Lambda functions

### Web (`/web`)
- Next.js with Tailwind CSS
- React 19 with TypeScript

### Mobile (`/mobile`)
- Android/Kotlin application
- Requires Java Extension Pack

### Firmware (`/firmware`)
- ESP32 development with PlatformIO
- Arduino framework

### Infra (`/infra`)
- AWS CDK with TypeScript
- Requires AWS Toolkit for deployment

## Troubleshooting

### Extensions Not Installing
If extensions fail to install automatically:
1. Check your internet connection
2. Restart VS Code
3. Try manual installation using the command above

### PlatformIO Issues
After installing PlatformIO IDE:
1. Restart VS Code
2. PlatformIO will download additional components on first use
3. Navigate to `/firmware` to initialize the PlatformIO environment

### AWS Toolkit Setup
To use AWS Toolkit:
1. Install the extension
2. Configure AWS credentials using AWS CLI or the extension's setup wizard
3. See `/infra/QUICKSTART.md` for deployment instructions
