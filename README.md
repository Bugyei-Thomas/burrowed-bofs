# BOF Take - Sliver BOF Extensions

This repository contains a curated collection of Beacon Object Files (BOFs) configured as extensions for the [Sliver C2 framework](https://github.com/BishopFox/sliver).

## Components

This project bundles the following BOF suites:

*   **PrivKit** (10 extensions): Windows privilege escalation checks.
*   **SQL-BOF** (28 extensions): Tools for interacting with SQL servers.
*   **UAC-BOF-Bonanza** (7 extensions): Various UAC bypass techniques.
*   **BOF-enumfiles**: Efficient file enumeration.
*   **Defender Exclusions**: Tool to add Windows Defender exclusions.

## Requirements

To build and install these extensions on Linux, you need:

*   `mingw-w64` (for compiling the C code)
*   `jq` (for processing JSON manifests)

Install them via:
```bash
# Ubuntu/Debian
sudo apt install mingw-w64 jq
```

## Setup

To compile all BOFs and register them as Sliver extensions, run the master setup script:

```bash
./setup_all_extensions.sh
```

Alternatively, you can build individual extensions by running the `setup_sliver_extension.sh` (or `setup_sliver_extensions.sh`) script located inside each project folder.

This will:
1.  Compile all source code.
2.  Generate `extension.json` manifests.
3.  Install them to `~/.sliver-client/extensions`.

## Cleanup

To remove all installed extensions from your Sliver environment:

```bash
./cleanup_all_extensions.sh
```
