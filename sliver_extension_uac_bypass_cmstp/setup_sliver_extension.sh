#!/bin/bash

# sliver_extension_uac_bypass_cmstp Setup Script
# Builds the Rust project (DLL) and sets up the Sliver extension

EXTENSIONS_DIR="$HOME/.sliver-client/extensions"
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "Setting up uac_bypass_cmstp extension in: $EXTENSIONS_DIR"
echo "Source directory: $PROJECT_DIR"
echo ""

# Change to project directory for build
cd "$PROJECT_DIR" || exit 1

# Build Rust Project
echo "Building Rust project (Windows DLL)..."
# Check for cargo
if ! command -v cargo &> /dev/null; then
    echo "ERROR: cargo not found. Please install Rust and Cargo."
    exit 1
fi

# Check for Windows target
if ! rustup target list --installed | grep -q "x86_64-pc-windows-gnu"; then
    echo "ERROR: Rust target x86_64-pc-windows-gnu not found."
    echo "Please install it with: rustup target add x86_64-pc-windows-gnu"
    exit 1
fi

cargo build --target x86_64-pc-windows-gnu --release
if [ $? -ne 0 ]; then
    echo "ERROR: Build failed."
    exit 1
fi
echo "  ✓ Build successful"
echo ""

# Ensure extensions directory exists
mkdir -p "$EXTENSIONS_DIR"

ext_file="extension.json"
if [ ! -f "$ext_file" ]; then
    echo "ERROR: extension.json not found!"
    exit 1
fi

# Extract name
name=$(jq -r '.name' "$ext_file")
if [ -z "$name" ] || [ "$name" = "null" ]; then
    echo "ERROR: Could not extract name from extension.json"
    exit 1
fi

echo "Processing extension: $name"

# Create extension directory
ext_dir="$EXTENSIONS_DIR/$name"
mkdir -p "$ext_dir"

# Copy extension.json
cp "$ext_file" "$ext_dir/extension.json"
echo "  ✓ Copied extension.json"

# Copy DLL
dll_path="target/x86_64-pc-windows-gnu/release/uac_bypass_cmstp.dll"
if [ -f "$dll_path" ]; then
    cp "$dll_path" "$ext_dir/uac_bypass_cmstp.dll"
    echo "  ✓ Copied DLL: uac_bypass_cmstp.dll"
else
    echo "ERROR: Built DLL not found at $dll_path"
    exit 1
fi

echo ""
echo "✓ Extension setup complete: $name"
echo "Extension ready at: $ext_dir"
