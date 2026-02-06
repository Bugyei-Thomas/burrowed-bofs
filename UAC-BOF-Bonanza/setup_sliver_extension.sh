#!/bin/bash

# UAC-BOF-Bonanza Sliver Extensions Setup Script
# This script creates the proper directory structure for Sliver extensions

EXTENSIONS_DIR="$HOME/.sliver-client/extensions"
PROJECT_DIR="$(pwd)"

echo "Setting up UAC-BOF-Bonanza extensions in: $EXTENSIONS_DIR"
echo "Source directory: $PROJECT_DIR"
echo ""

# Build all BOFs first using make
echo "Building UAC-BOF-Bonanza BOFs..."
if command -v x86_64-w64-mingw32-gcc &> /dev/null; then
    make bof
    if [ $? -eq 0 ]; then
        echo "  ✓ All UAC-BOF-Bonanza BOFs built successfully"
    else
        echo "  ✗ Error building BOFs. Check make output."
        exit 1
    fi
else
    echo "ERROR: mingw-w64 compiler not found!"
    echo "Please install it: sudo apt install mingw-w64"
    exit 1
fi
echo ""

# Ensure extensions directory exists
mkdir -p "$EXTENSIONS_DIR"

# Counter for tracking
created_count=0
failed_count=0

# Find all extension.json files in subdirectories
find . -name "extension.json" | while read ext_file; do
    # Extract the name from the extension.json file
    name=$(jq -r '.name' "$ext_file")
    
    if [ -z "$name" ] || [ "$name" = "null" ]; then
        echo "ERROR: Could not extract name from $ext_file"
        ((failed_count++))
        continue
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
        # Extract just the filename from the path
        bof_filename=$(basename "$bof_path")
        
        # Construct source path
        # The ext_file path is like "./SubDir/extension.json"
        # The bof_path in json is like "bin/file.o"
        # We need to construct "./SubDir/bin/file.o"
        
        subdir=$(dirname "$ext_file")
        bof_source="$subdir/$bof_path"
        bof_dest="$ext_dir/$bof_filename"
        
        # If not found at the exact path in extension.json, check if it's in a 'bin' folder
        if [ ! -f "$bof_source" ]; then
            if [ -f "$subdir/bin/$bof_filename" ]; then
                bof_source="$subdir/bin/$bof_filename"
            fi
        fi
        
        if [ -f "$bof_source" ]; then
            cp "$bof_source" "$bof_dest"
            echo "  ✓ Copied BOF: $bof_filename"
        else
            echo "  ✗ WARNING: BOF file not found: $bof_source"
            all_bofs_found=false
        fi
    done
    
    if [ "$all_bofs_found" = true ]; then
        echo "  ✓ Extension setup complete: $name"
        ((created_count++))
    else
        echo "  ⚠ Extension setup incomplete: $name (some BOF files missing)"
        ((failed_count++))
    fi
    echo ""
done

echo "=========================================="
echo "UAC-BOF-Bonanza Setup Complete"
echo "=========================================="
echo ""
