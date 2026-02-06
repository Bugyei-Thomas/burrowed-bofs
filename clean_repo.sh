#!/bin/bash

# clean_repo.sh
# Removes extension.json and build artifacts from the repository
# Usage: ./clean_repo.sh

echo "=========================================="
echo "  Repo Cleanup Script"
echo "=========================================="
echo ""

# 1. Remove extension.json files (Only if they are generated, limiting scope)
# NOTE: extension.json in BOF-enumfiles and Defender-Exclusions-Creator-BOF are SOURCE files.
# We will only delete extension.json if it is clearly a generated artifact in a build dir (none found so far)
# or if it was copied to a location we are cleaning. 
# For now, we SKIP deleting extension.json to avoid deleting source code.
echo "Skipping extension.json deletion (verified as source files)."
# find . -name "extension.json" -type f -print -delete
echo "Done."
echo ""

# 2. Remove build artifacts (object files, binaries)
echo "Cleaning build artifacts..."
# Common compiled file extensions
find . -type f \( -name "*.o" -o -name "*.obj" -o -name "*.exe" -o -name "*.dll" -o -name "*.bin" \) -print -delete
echo "Done."
echo ""

# 3. Clean specific distribution/compiled directories if they exist and become empty
echo "Cleaning specific artifact directories..."

# BOF-enumfiles: Clean x64/Release/Compiled if it exists
if [ -d "BOF-enumfiles/x64/Release/Compiled" ]; then
    echo "Cleaning BOF-enumfiles/x64/Release/Compiled..."
    rm -rf "BOF-enumfiles/x64/Release/Compiled"
fi
# Remove x64/Debug/Compiled if it exists
if [ -d "BOF-enumfiles/x64/Debug/Compiled" ]; then
    echo "Cleaning BOF-enumfiles/x64/Debug/Compiled..."
    rm -rf "BOF-enumfiles/x64/Debug/Compiled"
fi

# Defender-Exclusions-Creator-BOF: dist/ usually contains artifacts
if [ -d "Defender-Exclusions-Creator-BOF/dist" ]; then
    echo "Cleaning Defender-Exclusions-Creator-BOF/dist..."
    # Only remove contents, keep directory as Makefile expects it to exist
    rm -f "Defender-Exclusions-Creator-BOF/dist/"*
fi

# sliver_extension_uac_bypass_cmstp: Clean Rust target directory
if [ -d "sliver_extension_uac_bypass_cmstp/target" ]; then
    echo "Cleaning sliver_extension_uac_bypass_cmstp/target..."
    rm -rf "sliver_extension_uac_bypass_cmstp/target"
fi

echo "Done."
echo ""

echo "=========================================="
echo "  Cleanup Complete"
echo "=========================================="
