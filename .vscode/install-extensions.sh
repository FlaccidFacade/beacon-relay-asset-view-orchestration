#!/bin/bash
# Install all recommended VS Code extensions for B.R.A.V.O. development

set -e

echo "🚀 Installing B.R.A.V.O. recommended VS Code extensions..."
echo ""

# Check if code command is available
if ! command -v code &> /dev/null; then
    echo "❌ VS Code 'code' command not found in PATH"
    echo ""
    echo "Please ensure VS Code is installed and the 'code' command is available."
    echo "To enable the 'code' command:"
    echo "  - macOS/Linux: Open VS Code, press Cmd+Shift+P, search for 'Shell Command: Install code command in PATH'"
    echo "  - Windows: The 'code' command should be available after installation"
    exit 1
fi

# Array of extension IDs
extensions=(
    "dbaeumer.vscode-eslint"
    "esbenp.prettier-vscode"
    "bradlc.vscode-tailwindcss"
    "amazonwebservices.aws-toolkit-vscode"
    "platformio.platformio-ide"
    "mathiasfrohlich.kotlin"
    "vscjava.vscode-java-pack"
    "eamodio.gitlens"
    "github.vscode-pull-request-github"
    "yzhang.markdown-all-in-one"
    "davidanson.vscode-markdownlint"
    "editorconfig.editorconfig"
    "ms-vscode.live-server"
    "christian-kohler.path-intellisense"
    "visualstudioexptteam.vscodeintellicode"
)

total=${#extensions[@]}
installed=0
skipped=0
failed=0

# Install each extension
for extension in "${extensions[@]}"; do
    echo "📦 Installing: $extension"
    
    # Check if already installed
    if code --list-extensions | grep -q "^${extension}$"; then
        echo "   ✓ Already installed, skipping"
        ((skipped++))
    else
        # Try to install
        if code --install-extension "$extension" > /dev/null 2>&1; then
            echo "   ✓ Installed successfully"
            ((installed++))
        else
            echo "   ✗ Failed to install"
            ((failed++))
        fi
    fi
    echo ""
done

# Summary
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "📊 Installation Summary:"
echo "   Total extensions: $total"
echo "   ✓ Newly installed: $installed"
echo "   ⊘ Already installed: $skipped"
echo "   ✗ Failed: $failed"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

if [ $failed -eq 0 ]; then
    echo "✅ All extensions are now installed!"
    echo ""
    echo "Next steps:"
    echo "  1. Restart VS Code to activate all extensions"
    echo "  2. For PlatformIO: Additional components will download on first use"
    echo "  3. For AWS Toolkit: Configure your AWS credentials"
else
    echo "⚠️  Some extensions failed to install. Please try installing them manually."
    exit 1
fi
