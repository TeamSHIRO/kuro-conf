# kuro-conf

**kuro-conf** is a minimal command-line tool for securely configuring the KURO UEFI Bootloader using a configuration file. More info on the configuration in the [KURO configuration documentation](https://github.com/TeamSHIRO/KURO/blob/main/docs/kuro_config.md).

## Features

## Installation

> [!NOTE]
> Currently, kuro-conf is only compatible with linux-based operating systems... You can help us extend compatibility by contributing to the project!

Run this one-liner script to install kuro-conf on your system.

```bash
curl -fsSL https://raw.githubusercontent.com/TeamSHIRO/kuro-conf/main/install.sh | bash
```

Make sure you have all of the required dependencies, like Git and CMake!

## Usage

```
kuro-conf {command} {kuro-path} [options]
```

### Available commands

- `edit`: **Edit the configuration** of the KURO UEFI bootloader with a .conf file (path provided by the `-c` attribute).

- `read`: **Read the configuration** of the KURO UEFI bootloader.

- `mkconf`: **Generate a default configuration file** for kuro-conf.

### Attributes

- `-c --config <path>`: Specify the path to the .conf file for editing the KURO UEFI bootloader configuration.

- `-v --version`: Fetch the current version of kuro-conf and check for updates automatically.

## Quickstart

First, download KURO from the [Official repository](https://github.com/TeamSHIRO/KURO).

Next, create yourself a configuration file for kuro-conf to read. You can use the `kuro-conf mkconf` command to help getting you started, by generating automatically a default configuration file. Edit the configuration file to customize the settings according to your needs.

```
kuro-conf mkconf
```

Finally, use the `kuro-conf edit` command to apply your configuration to the KURO UEFI bootloader EFI executable. Here is an example usage

```bash
kuro-conf edit KUROX64.EFI -c kuro.conf
```

You can then read the configuration from the bootloader to verify its integrity using the `kuro-conf read` command:

```bash
kuro-conf read KUROX64.EFI
```

## Configuration Reference

**VERSION**: Specifies the kuro configuration to use.

| Version | Description     | Status         |
| ------- | --------------- | -------------- |
| 1       | Initial version | 🟢 SUPPORTED   |
| 0       | Unknown version | 🔴 UNSUPPORTED |

**HAS_PUBLIC_KEY**: Specifies if the public key is present and if the KURO Bootloader should verify the kernel's signature using the public key.

> ![WARNING]
> Enabling this feature is strongly recommended for security purposes. Disabling it may compromise the security of your system.

**ASLR_ENABLED**: Specifies if Address Space Layout Randomization (ASLR) is enabled for the KURO Bootloader.

> ![WARNING]
> Enabling this feature is strongly recommended for security purposes. Disabling it may compromise the security of your system.

**PUBLIC_KEY**: Specifies the binary-formatted public key file used by the KURO Bootloader to verify the kernel's signature. This argument may be omitted if the `HAS_PUBLIC_KEY` feature is disabled.

**EXECUTABLE_PATH**: Specifies the path of the kernel to be loaded by the KURO Bootloader.
