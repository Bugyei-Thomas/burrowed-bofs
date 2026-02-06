#!/bin/bash

# SQL-BOF Sliver Extensions Setup Script
# This script creates the proper directory structure for Sliver extensions

EXTENSIONS_DIR="$HOME/.sliver-client/extensions"
SQL_BOF_DIR="$(pwd)"

echo "Setting up SQL-BOF extensions in: $EXTENSIONS_DIR"
echo "Source directory: $SQL_BOF_DIR"
echo ""

# Build all BOFs first
echo "Building SQL-BOF extensions..."
if command -v x86_64-w64-mingw32-gcc &> /dev/null; then
    ./make_all.sh
    echo "  ✓ All SQL BOFs built successfully"
else
    echo "  ⚠ Warning: mingw-w64 not found. Assuming BOFs are already built."
    echo "  (Install with: sudo apt install mingw-w64)"
fi
echo ""

# Ensure extensions directory exists
mkdir -p "$EXTENSIONS_DIR"

# Counter for tracking
created_count=0
failed_count=0

# Process each extension-sql-*.json file
for ext_file in extension-sql-*.json; do
    if [ ! -f "$ext_file" ]; then
        echo "No extension files found!"
        exit 1
    fi
    
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
    
    # Copy the extension.json file (rename to extension.json)
    cp "$ext_file" "$ext_dir/extension.json"
    echo "  ✓ Copied extension.json"
    
    # Get the BOF paths from the extension.json
    bof_paths=$(jq -r '.files[].path' "$ext_file")
    
    # Flag to track if all BOFs were copied
    all_bofs_found=true
    
    # Copy BOF files
    # Note: extension.json has flat paths, but we need to find files in nested SQL/ directories
    for bof_path in $bof_paths; do
        bof_filename=$(basename "$bof_path")
        
        # Map the flat filename back to the nested structure
        # Extract the BOF name without extension (e.g., whoami from whoami.x64.o)
        bof_name=$(echo "$bof_filename" | sed 's/\.\(x64\|x86\)\.o$//')
        
        # Construct the source path in nested structure
        bof_source="$SQL_BOF_DIR/SQL/$bof_name/$bof_filename"
        bof_dest="$ext_dir/$bof_filename"
        
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
echo "Setup Summary:"
echo "  Successfully created: $created_count"
echo "  Failed/Incomplete: $failed_count"
echo "=========================================="
echo ""
echo "Extensions are ready at: $EXTENSIONS_DIR"
echo "Use 'extensions load <path>' in Sliver to load them."
