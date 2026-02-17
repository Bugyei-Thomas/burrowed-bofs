#!/bin/bash

# Defender-Exclusions-Creator-BOF Sliver Extension Setup Script
# This script creates the proper directory structure for Sliver extension

EXTENSIONS_DIR="$HOME/.sliver-client/extensions"
BOF_DIR="$(pwd)"

echo "Setting up Defender-Exclusions-Creator-BOF extension in: $EXTENSIONS_DIR"
echo "Source directory: $BOF_DIR"
echo ""

# Build BOF first
echo "Building BOF..."
mkdir -p "$BOF_DIR/dist"
if command -v x86_64-w64-mingw32-g++ &> /dev/null; then
    (cd src && make)
    if [ $? -ne 0 ]; then
        echo "  ✗ ERROR: BOF build failed"
        exit 1
    fi
    echo "  ✓ BOF built successfully"
else
    echo "  ⚠ Warning: mingw-w64 not found. Assuming BOF is already built."
    echo "  (Install with: sudo apt install mingw-w64)"
fi
echo ""

# Ensure extensions directory exists
mkdir -p "$EXTENSIONS_DIR"

ext_file="extension.json"

if [ ! -f "$ext_file" ]; then
    echo "ERROR: extension.json not found!"
    exit 1
fi

# Extract the name from the extension.json file
name=$(jq -r '.name' "$ext_file")

if [ -z "$name" ] || [ "$name" = "null" ]; then
    echo "ERROR: Could not extract name from $ext_file"
    exit 1
fi

echo "Processing extension: $name"

# Create the extension directory
ext_dir="$EXTENSIONS_DIR/$name"
mkdir -p "$ext_dir"

# Copy the extension.json file
cp "$ext_file" "$ext_dir/extension.json"
echo "  ✓ Copied extension.json"

# Get the BOF paths from the extension.json
bof_paths=$(jq -r '.files[].path' "$ext_file")

# Flag to track if all BOFs were copied
all_bofs_found=true

# Copy BOF files
for bof_path in $bof_paths; do
    bof_filename=$(basename "$bof_path")
    bof_source="$BOF_DIR/dist/$bof_filename"
    bof_dest="$ext_dir/$bof_filename"
    
    if [ -f "$bof_source" ]; then
        cp "$bof_source" "$bof_dest"
        echo "  ✓ Copied BOF: $bof_filename"
    else
        echo "  ✗ WARNING: BOF file not found: $bof_source"
        all_bofs_found=false
    fi
done

echo ""
if [ "$all_bofs_found" = true ]; then
    echo "✓ Extension setup complete: $name"
    echo "Extension ready at: $ext_dir"
else
    echo "⚠ Extension setup incomplete: $name (some BOF files missing)"
    exit 1
fi

exit 0
