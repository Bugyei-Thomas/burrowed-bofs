#!/bin/bash

# Master Setup Script for All BOF Extensions
# This script runs all individual setup scripts for each BOF project

BOFTAKE_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "=========================================="
echo "  BOF Extensions Master Setup Script"
echo "=========================================="
echo ""
echo "This will setup all 4 BOF projects as Sliver extensions:"
echo "  1. BOF-enumfiles (1 extension)"
echo "  2. Defender-Exclusions-Creator-BOF (1 extension)"
echo "  3. PrivKit (10 extensions)"
echo "  4. SQL-BOF (28 extensions)"
echo "  5. Uac_cmstp_bypass (1 extension)"
echo ""
echo "Total: 48 extensions"
echo "=========================================="
echo ""

# Check if jq is installed
if ! command -v jq &> /dev/null; then
    echo "ERROR: jq is not installed. Please install it first:"
    echo "  sudo apt install jq   (Debian/Ubuntu)"
    echo "  sudo yum install jq   (RHEL/CentOS)"
    echo "  brew install jq       (macOS)"
    exit 1
fi

# Initialize counters
total_success=0
total_failed=0

# Function to run a setup script
run_setup() {
    local project_dir="$1"
    local script_name="$2"
    local project_name="$3"
    
    echo "=========================================="
    echo "Setting up: $project_name"
    echo "=========================================="
    
    if [ ! -d "$project_dir" ]; then
        echo "ERROR: Directory not found: $project_dir"
        ((total_failed++))
        return 1
    fi
    
    if [ ! -f "$project_dir/$script_name" ]; then
        echo "ERROR: Setup script not found: $project_dir/$script_name"
        ((total_failed++))
        return 1
    fi
    
    # Make script executable if it isn't already
    chmod +x "$project_dir/$script_name"
    
    # Run the setup script
    cd "$project_dir" || exit 1
    ./"$script_name"
    local exit_code=$?
    cd "$BOFTAKE_DIR" || exit 1
    
    if [ $exit_code -eq 0 ]; then
        echo "✓ $project_name setup completed successfully"
        ((total_success++))
    else
        echo "✗ $project_name setup failed with exit code: $exit_code"
        ((total_failed++))
    fi
    
    echo ""
    return $exit_code
}

# Setup each project
run_setup "$BOFTAKE_DIR/BOF-enumfiles" "setup_sliver_extension.sh" "BOF-enumfiles"
run_setup "$BOFTAKE_DIR/Defender-Exclusions-Creator-BOF" "setup_sliver_extension.sh" "Defender-Exclusions-Creator-BOF"
run_setup "$BOFTAKE_DIR/PrivKit" "setup_sliver_extensions.sh" "PrivKit"
run_setup "$BOFTAKE_DIR/SQL-BOF" "setup_sliver_extensions.sh" "SQL-BOF"
run_setup "$BOFTAKE_DIR/UAC-BOF-Bonanza" "setup_sliver_extension.sh" "UAC-BOF-Bonanza"
run_setup "$BOFTAKE_DIR/sliver_extension_uac_bypass_cmstp" "setup_sliver_extension.sh" "sliver_extension_uac_bypass_cmstp"

# Final summary
echo "=========================================="
echo "  Master Setup Complete"
echo "=========================================="
echo "Projects successfully setup: $total_success / 6"
if [ $total_failed -gt 0 ]; then
    echo "Projects failed: $total_failed / 6"
fi
echo ""
echo "Extensions directory: ~/.sliver-client/extensions"
echo ""
echo "Next steps:"
echo "  1. Start Sliver client"
echo "  2. Use 'extensions list' to see available extensions"
echo "  3. Load extensions with 'extensions load <name>'"
echo "=========================================="

# Exit with error if any setup failed
if [ $total_failed -gt 0 ]; then
    exit 1
fi

exit 0
