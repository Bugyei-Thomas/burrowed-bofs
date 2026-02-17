# Burrowed-Bofs - Sliver BOF Extensions

This repository contains a curated collection of Beacon Object Files (BOFs) configured as extensions for the [Sliver C2 framework](https://github.com/BishopFox/sliver).

## Components

This project bundles the following BOF suites (Total 48 extensions):

*   **PrivKit** (10 extensions): Windows privilege escalation checks.
*   **SQL-BOF** (28 extensions): Tools for interacting with SQL servers.
*   **UAC-BOF-Bonanza** (7 extensions): Various UAC bypass techniques.
*   **BOF-enumfiles** (1 extension): Efficient file enumeration.
*   **Defender-Exclusions-Creator-BOF** (1 extension): Tool to add Windows Defender exclusions.
*   **uac_bypass_cmstp** (1 extension): Rust-based UAC bypass via `cmstp.exe`.
*   **PrintSpoofer-Bof** (1 extension): Bof Version of PrintSpoofer.

## Requirements

To build and install these extensions on Linux, you need

*   `mingw-w64` (for compiling C/C++ BOFs)
*   `rust` & `cargo` (for the CMSTP UAC bypass extension)
*   `jq` (for processing JSON manifests)
*   `make` (for build automation)

Install dependencies on Ubuntu/Debian:
```bash
sudo apt install mingw-w64 jq make
# For Rust, follow: https://rustup.rs/
```

## Setup

To compile all BOFs and register them as Sliver extensions, run the master setup script:

```bash
./setup_all_extensions.sh
```

This will:
1.  Compile all source code (using `mingw-w64`, `cargo`, or `make`).
2.  Maintain/Verify `extension.json` manifests.
3.  Install them to `~/.sliver-client/extensions`.

## Cleanup

To remove all installed extensions from your Sliver environment:

```bash
# Remove from Sliver
./cleanup_all_extensions.sh
```

## Shoutouts & Credits

A huge thank you to the authors of these amazing BOFs:

*   **[wsummerhill](https://github.com/wsummerhill)** for `BOF-enumfiles`.
*   **[EspressoCake](https://github.com/EspressoCake)** for `Defender-Exclusions-Creator-BOF`.
*   **[mertdas](https://github.com/mertdas)** for `PrivKit`.
*   **[Tw1sm](https://github.com/Tw1sm)** for `SQL-BOF`.
*   **[icyguider](https://github.com/icyguider)** for the `UAC-BOF-Bonanza` suite.
*   **[SlimeOnSecurity](https://github.com/SlimeOnSecurity)** for `PrintSpoofer-BOF`.
*   **[0xb11a1](https://github.com/0xb11a1)** for the Rust-based `uac_bypass_cmstp` extension.

---
*Note: This repository is for educational and authorized testing purposes only.*
