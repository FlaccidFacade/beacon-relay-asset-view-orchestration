# VSCode Development Setup

This directory contains VSCode workspace configuration to ensure a consistent development experience across the B.R.A.V.O. project.

## Quick Install Recommended Extensions

When you open this workspace in VSCode, you'll see a notification to install the recommended extensions. Click **Install All** to quickly set up your development environment.

Alternatively, you can:

1. Open the Command Palette (`Ctrl+Shift+P` or `Cmd+Shift+P`)
2. Type "Extensions: Show Recommended Extensions"
3. Click the cloud icon to install all recommended extensions

## Recommended Extensions Overview

### TypeScript & JavaScript
- **ESLint**: Linting for TypeScript and JavaScript
- **Prettier**: Code formatter for consistent style

### React & Next.js
- **ES7+ React/Redux/React-Native snippets**: Code snippets for React development

### PlatformIO & C++/Arduino
- **PlatformIO IDE**: Complete solution for ESP32 firmware development
- **C/C++ Extension Pack**: IntelliSense, debugging, and code browsing for C++

### Android & Kotlin
- **Kotlin Language**: Kotlin language support
- **Java Extension Pack**: Java development tools for Android

### AWS & Infrastructure
- **AWS Toolkit**: Interact with AWS services
- **Amazon Q**: AI-powered coding assistant for AWS

### Git & Version Control
- **GitLens**: Enhanced Git capabilities
- **GitHub Pull Requests**: Review and manage pull requests

### Documentation
- **Markdown All in One**: Shortcuts and preview for Markdown
- **markdownlint**: Markdown linting and style checking

### General Productivity
- **EditorConfig**: Maintain consistent coding styles
- **Code Spell Checker**: Spell checking for code
- **Todo Tree**: Track TODO, FIXME, and other action items

### Other
- **YAML**: YAML language support
- **Tailwind CSS IntelliSense**: Auto-completion for Tailwind CSS

## Manual Installation

If you prefer to manually install extensions, you can use the VSCode command line:

```bash
# Install all recommended extensions at once
code --install-extension dbaeumer.vscode-eslint \
     --install-extension esbenp.prettier-vscode \
     --install-extension dsznajder.es7-react-js-snippets \
     --install-extension platformio.platformio-ide \
     --install-extension ms-vscode.cpptools \
     --install-extension ms-vscode.cpptools-extension-pack \
     --install-extension mathiasfrohlich.kotlin \
     --install-extension fwcd.kotlin \
     --install-extension vscjava.vscode-java-pack \
     --install-extension amazonwebservices.aws-toolkit-vscode \
     --install-extension amazonwebservices.amazon-q-vscode \
     --install-extension eamodio.gitlens \
     --install-extension github.vscode-pull-request-github \
     --install-extension yzhang.markdown-all-in-one \
     --install-extension davidanson.vscode-markdownlint \
     --install-extension editorconfig.editorconfig \
     --install-extension streetsidesoftware.code-spell-checker \
     --install-extension gruntfuggly.todo-tree \
     --install-extension redhat.vscode-yaml \
     --install-extension bradlc.vscode-tailwindcss
```

## Component-Specific Extensions

Different parts of the project may use different extensions:

- **`/api`**: TypeScript, ESLint, Prettier, AWS Toolkit
- **`/web`**: TypeScript, React, Tailwind CSS, ESLint, Prettier
- **`/firmware`**: PlatformIO IDE, C/C++
- **`/mobile`**: Kotlin, Java Extension Pack
- **`/infra`**: TypeScript, AWS Toolkit, CDK

## Troubleshooting

### Extensions Not Showing
If VSCode doesn't prompt you to install recommended extensions:
1. Check that you've opened the **workspace root folder** (not a subdirectory)
2. Go to Extensions view (`Ctrl+Shift+X`)
3. Filter by "@recommended"

### PlatformIO Not Working
If PlatformIO IDE doesn't activate:
1. Ensure Python is installed on your system
2. Restart VSCode after installation
3. Check the PlatformIO documentation for platform-specific requirements

### Java/Kotlin Extensions Issues
1. Ensure you have JDK installed (version 11 or higher)
2. Set `JAVA_HOME` environment variable
3. Restart VSCode

## Contributing

When adding new development tools or languages to the project, please update the `extensions.json` file with relevant VSCode extensions.
