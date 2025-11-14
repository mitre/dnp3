# MITRE Caldera™ for OT: DNP3 Payload Source Code

This directory contains payload source code for the [MITRE Caldera™ for OT](https://github.com/mitre/caldera-ot) DNP3 plugin. Where possible, the Caldera for OT plugins leverage open-source libraries and payloads, unifying their exposure through the Caldera Adversary Emulation framework.

* The DNP3 plugin is written in C++ using the open-source [OpenDNP3 library](https://github.com/dnp3/opendnp3/tree/release/) - version 3.1.2, which is also the final release of the project.

* A custom command-line interface was developed to unify exposure of DNP3 protocol functionality (using the OpenDNP3 project source code) for threat emulation execution as Caldera Ability commands.

## Build From Source

### Directory Layout

The Caldera for OT DNP3 Plugin payload source code is contained in this repository, which is divided as follows:
* `cpp`   - Contains C++ source code for `dnp3-actions`.
* `bin`   - Build commands will save final binaries in this directory
* `third_party`  - Local versions of project dependencies. By default, CMake will fetch the proper dependencies from GitHub, but if desired the build can be configured to use downloaded versions of OpenDNP3 and CLI11 saved under this directory.

### Step-by-Step Build Instructions
1. Clone repository:
```
git clone https://github.com/mitre/dnp3.git
```
2. Build the source using CMake:
```bash
# from the dnp3 directory
mkdir src/build && cd src/build
cmake ..
cmake --build .
```
This will build the payload and its dependent libraries, placing the final binary in the `src/bin` directory.

### Offline Builds

The normal build instructions above will fetch the required dependencies during the configuration process. If this is not possible or not desired, the dependencies can be manually copied to the `third_party/` directory using these steps:
1. Retrieve the `opendnp3` version 3.1.2 source code:
  - Download the source code from the library's GitHub repository [here](https://github.com/dnp3/opendnp3/releases/tag/3.1.2) as either a `.zip` or `.tar.gz` archive.
  - Unzip the archive file. This should produce a directory named `opendnp3-3.1.2`.
  - Copy that directory into the `third_party/` directory
2. Build the source using CMake:
```bash
# from the root directory
mkdir src/build && cd src/build
cd build
cmake -DBUILD_LOCAL=ON ..
cmake --build .
```
## Usage

The `dnp3_actions` executable supports the following general usage:
```
dnp3_actions [--help] (tcp|serial) [<connection_args>] <command> [<args>]...
```

Use `dnp3_actions --help` to get started
