#!/bin/bash

# Setup script for PrintSpoofer-BOF Sliver Extension
# This script builds the BOFs and installs/updates the extension in the Sliver client directory.

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
EXTENSIONS_DIR="$HOME/.sliver-client/extensions"
EXTENSION_NAME="printspoofer"
TARGET_DIR="$EXTENSIONS_DIR/$EXTENSION_NAME"

echo "Setting up $EXTENSION_NAME extension in: $EXTENSIONS_DIR"
echo "Source directory: $PROJECT_DIR"

# 1. Build the BOFs
echo ""
echo "Building BOF..."
chmod +x "$PROJECT_DIR/build.sh"
cd "$PROJECT_DIR" && ./build.sh

if [ $? -eq 0 ]; then
    echo "  ✓ BOF built successfully"
else
    echo "  ✗ Build failed"
    exit 1
fi

# 2. Create target directory if it doesn't exist
mkdir -p "$TARGET_DIR"

# 3. Copy files
echo ""
echo "Processing extension: $EXTENSION_NAME"

cp "$PROJECT_DIR/extension.json" "$TARGET_DIR/"
echo "  ✓ Copied extension.json"

if [ -f "$PROJECT_DIR/printspoofer.x64.o" ]; then
    cp "$PROJECT_DIR/printspoofer.x64.o" "$TARGET_DIR/"
    echo "  ✓ Copied BOF: printspoofer.x64.o"
fi

if [ -f "$PROJECT_DIR/printspoofer.x86.o" ]; then
    cp "$PROJECT_DIR/printspoofer.x86.o" "$TARGET_DIR/"
    echo "  ✓ Copied BOF: printspoofer.x86.o"
fi

echo ""
echo "✓ Extension setup complete: $EXTENSION_NAME"
echo "Extension ready at: $TARGET_DIR"
