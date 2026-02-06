#!/bin/bash

# Cleanup Script for BOF Extensions
# This script removes all extension directories created by setup_all_extensions.sh

EXTENSIONS_DIR="$HOME/.sliver-client/extensions"

echo "=========================================="
echo "  BOF Extensions Cleanup Script"
echo "=========================================="
echo ""
echo "This will remove all 48 BOF extensions from:"
echo "  $EXTENSIONS_DIR"
echo ""
echo "Extensions to be removed:"
echo "  - enumfiles (BOF-enumfiles)"
echo "  - cGenerateDefenderExclusion (Defender-Exclusions-Creator-BOF)"
echo "  - 10 PrivKit extensions"
echo "  - 28 SQL-BOF extensions"
echo "  - 7 UAC-BOF-Bonanza extensions"
echo ""
read -p "Are you sure you want to continue? (yes/no): " confirm

if [ "$confirm" != "yes" ]; then
    echo "Cleanup cancelled."
    exit 0
fi

echo ""
echo "Starting cleanup..."
echo ""

# Initialize counters
removed_count=0
not_found_count=0

# Function to remove extension directory
remove_extension() {
    local ext_name="$1"
    local ext_path="$EXTENSIONS_DIR/$ext_name"
    
    if [ -d "$ext_path" ]; then
        rm -rf "$ext_path"
        echo "  ✓ Removed: $ext_name"
        ((removed_count++))
    else
        echo "  ⊘ Not found: $ext_name"
        ((not_found_count++))
    fi
}

# Remove BOF-enumfiles
echo "Removing BOF-enumfiles extension..."
remove_extension "enumfiles"
echo ""

# Remove Defender-Exclusions-Creator-BOF
echo "Removing Defender-Exclusions-Creator-BOF extension..."
remove_extension "cGenerateDefenderExclusion"
echo ""

# Remove PrivKit extensions (both old and new naming conventions)
echo "Removing PrivKit extensions..."
# Remove new CamelCase names
remove_extension "AlwaysInstallElevatedCheck"
remove_extension "AutologonCheck"
remove_extension "CredentialManagerCheck"
remove_extension "HijackablePathCheck"
remove_extension "ModifiableAutorunCheck"
remove_extension "ModifiableSVCCheck"
remove_extension "PowerShellHistoryCheck"
remove_extension "TokenPrivilegesCheck"
remove_extension "UACStatusCheck"
remove_extension "UnquotedSVCPathCheck"
echo ""

# Remove SQL-BOF extensions
echo "Removing SQL-BOF extensions..."
remove_extension "sql-1434udp"
remove_extension "sql-adsi"
remove_extension "sql-agentcmd"
remove_extension "sql-agentstatus"
remove_extension "sql-checkrpc"
remove_extension "sql-clr"
remove_extension "sql-columns"
remove_extension "sql-databases"
remove_extension "sql-disableclr"
remove_extension "sql-disableole"
remove_extension "sql-disablerpc"
remove_extension "sql-disablexp"
remove_extension "sql-enableclr"
remove_extension "sql-enableole"
remove_extension "sql-enablerpc"
remove_extension "sql-enablexp"
remove_extension "sql-impersonate"
remove_extension "sql-info"
remove_extension "sql-links"
remove_extension "sql-olecmd"
remove_extension "sql-query"
remove_extension "sql-rows"
remove_extension "sql-search"
remove_extension "sql-smb"
remove_extension "sql-tables"
remove_extension "sql-users"
remove_extension "sql-whoami"
remove_extension "sql-xpcmd"
echo ""

# Remove UAC-BOF-Bonanza extensions
echo "Removing UAC-BOF-Bonanza extensions..."
remove_extension "CmstpElevatedCOM"
remove_extension "ColorDataProxy"
remove_extension "EditionUpgradeManager"
remove_extension "RegistryShellCommand"
remove_extension "SilentCleanupWinDir"
remove_extension "SspiUacBypass"
remove_extension "TrustedPathDLLHijack"
echo ""

# Remove sliver_extension_uac_bypass_cmstp
echo "Removing sliver_extension_uac_bypass_cmstp..."
remove_extension "uac_bypass_cmstp"
echo ""

# Final summary
echo "=========================================="
echo "  Cleanup Complete"
echo "=========================================="
echo "Extensions removed: $removed_count"
echo "Extensions not found: $not_found_count"
echo "=========================================="

if [ $removed_count -gt 0 ]; then
    echo ""
    echo "All specified extensions have been removed from:"
    echo "  $EXTENSIONS_DIR"
fi

exit 0
