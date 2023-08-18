# DNP3 Plugin Code

Where possible, the Caldera for OT plugins leverage open-source libraries and payloads, unifying their exposure through the Caldera Adversary Emulation framework.

* The DNP3 plugin is written in C++ using the open-source [OpenDNP3 library](https://github.com/dnp3/opendnp3/tree/release/) - version 3.1.2, which is also the final release of the project.

* A custom command-line interface was developed to unify exposure of DNP3 protocol functionality (using the OpenDNP3 project source code) for threat emulation execution as Caldera Ability commands.

## Specific Code Modifications

The Caldera for OT DNP3 Plugin payload source code is contained in this repository, which is divided as follows:
* `bin`   - Contains built executables for Windows
* `deps`  - Project build dependencies, including from the OpenDNP3 library. For example, the files in [`include`](https://github.com/dnp3/opendnp3/tree/release/cpp/lib/include/opendnp3).
* `docs`  - An example `.json` for VSCode, used as part of the build process
* `src`   - Contains C++ code and necessary files to compile `dnp3-actions`, `dnp3-poll`, and `dnp3-server` executables.
    * `dnp3-actions` and corresponding files represent a custom created CLI interface to expose select DNP3 protocol functions.
    * `dnp3-poll` modifies the OpenDNP3 library's [master example](https://github.com/dnp3/opendnp3/blob/release/cpp/examples/master/main.cpp), to allow for genericized paramters at run time.
    * `dnp3-server` is a non-modified [outstation example](https://github.com/dnp3/opendnp3/blob/release/cpp/examples/outstation/main.cpp) provided for convenience.
* `utils` - Contains Clang `.cmake` files and a utility to copy built binaries into `bin`

in addition to the `CMakeLists.txt` (also see [Reproducing Builds](#reproducing-builds)).

The OpenDNP3 project falls under the [Apache License 2.0](https://github.com/dnp3/opendnp3/blob/release/LICENSE). The Copyright statement is included in the files under `src` as appropiate; code following this inclusions marks that it is leveraged from OpenDNP3 examples.

The source code contained in this repository is specific to building the three binaries described above. `dnp3-actions` specifically corresponds to the payload used to execute the DNP3 plugin abilities.

The primary files for `dnp3-actions` are:
* `dnp3_actions.cpp` - contains DNP3 protocol function code called by the CLI, aligns with the functions in the corresponding `.h` file. As indicated in the source file, some of the code is taken from pre-existing the OpenDNP3 examples.
* `dnp3_cli.cpp` - defines a CLI that handles the following commands:
    * Read
    * Integrity Poll
    * Cold Restart
    * Warm Restart
    * Disable Unsolicited Messages
    * Enable Unsolicited Messages
    * Select-before-Operate (SBO)
    * Direct Operate (DO)
    * and other sub-command variations for SBO and DO actions.

## **Reproducing Builds**

How to compile the binaries present in the plugin’s payloads folder.
It is highly advised to review all relevant instructions before proceeding to build.

See the included `CMakeLists.txt` for technical details.
Due to unknown deployment environments, we want to statically link as much as possible.

Building has been tested on Windows 10 64-bit using VSCode as described [here](https://code.visualstudio.com/docs/cpp/config-mingw).
The compiler used is Mingw-w64 installed via MSYS2, as described [here](https://www.msys2.org/).

An example `settings.json` file can be found in `docs/example_vscode_settings.json` which should
be placed as `.vscode/settings.json` in this project's directory.

There are three build targets:
- `dnp3-actions`: This has all the dnp3 threat emulation actions.
- `dnp3-poll`: This is a test polling (master) app.
- `dnp3-server`: This is a test server (outstation) app.

### Versions

For Windows 10 x84_64:

| Item              | Version   | Note                  |
|---------          |---------  |------                 |
| VSCode            | 1.67.1    |                       |
| C/C++ Ext         | 1.15.4    | VSCode extension      |
| CMake Tools Ext   | 1.14.31   | VSCode extension      |
| MSYS2             | 20220603  |                       |
| GCC               | 13.1.0    | Installed via MSYS2   |
| CMake             | 3.26.4    | Separate [download](https://cmake.org/download/)|

### Build Tips

As you follow the steps in [Reproducing Builds](#reproducing-builds), this may be helpful:

1. Install VSCode
    * Install the following extensions:
        * C/C++
        * CMake
        * CMake Tools
    * It also works if you have CMake installed on your system
        * https://cmake.org/download/
        * CMake may also come installed, depending on your version of VSCode
2. Follow the VSCode instructions for mingw
3. Follow the above instructions, to the MSYS2 Installer step
    * Follow the install instructions on the website
    * Notes:
        * Depending on version, MSYS, not UCRT64 is opened immediately
            * You can find and open UCRT64 and open yourself
            * Proceed with the `pacman` command
    * May need to run `pacman -Suy` on MSYS at the end
        *  i.e. "Step 9", the auto-update may not occur
4. Return to the VSCode instructions
    * Follow "Step 5" on MSYS
        * This should result in the binaries being installed in `/bin`
    * Complete setup guide, ensuring tools are installed
5. If it does not exist, create `.vscode` in your project directory
    * Copy and modify the `example_vscode_settings.json` as described above
    * If not prompted by CMakeTools
        * `ctrl + shift + p` and search for `CMake: Select a Kit`
            * Select your toolkit as appropriate, ensure `settings.json` matches

Other:
* If issues occur, it may help to delete the build
    * Double check settings
* Run `update_bins.sh` to copy the newly generated `.exe` to `/bin`

## Usage
`./dnp3actions.exe -h`
