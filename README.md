# Burrowed Bofs - Sliver BOF Extensions

This repository contains a curated collection of Beacon Object Files (BOFs) configured as extensions for the [Sliver C2 framework](https://github.com/BishopFox/sliver).

## Components

This project bundles the following BOF suites:

*   **PrivKit** (10 extensions): Windows privilege escalation checks.
*   **SQL-BOF** (28 extensions): Tools for interacting with SQL servers.
*   **UAC-BOF-Bonanza** (7 extensions): Various UAC bypass techniques.
*   **BOF-enumfiles**: Efficient file enumeration.
*   **Defender Exclusions**: Tool to add Windows Defender exclusions.
*   **Uac_cmstp_bypass**: Tool that performs uac bypass using cmstp.

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

```
./setup_all_extensions.sh
```

Alternatively, you can build individual extensions by running the `./setup_sliver_extension.sh` script located inside each project folder.

This will:
1.  Compile all source code.
2.  Generate `extension.json` manifests (if not already in the folder).
3.  Move them to `~/.sliver-client/extensions`, where upon running sliver it will automagically add them to your arsenal.

## Cleanup

To remove all installed extensions from your Sliver environment:

```
./cleanup_all_extensions.sh
```
<br>

## Shoutout
Feel free to hit me up if there are some cool bofs that I didn't put on this list or stuff that I missed. <br>
Many Thanks to the creators of these bofs.

1. [mertdas](https://github.com/mertdas)
2. [Tw1sm](https://github.com/Tw1sm/)
3. [icyguider](https://github.com/icyguider)
4. [wsummerhill](https://github.com/wsummerhill)
5. [EspressoCake](https://github.com/EspressoCake/)
6. [0xb11a1](https://github.com/0xb11a1)


