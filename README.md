# CALDERA for OT plugin: DNP3

A [Caldera for OT](https://github.com/mitre/caldera-ot) plugin supplying [CALDERA](https://github.com/mitre/caldera) with DNP3 protocol TTPs. 
This is part of a series of plugins that provide added threat emulation capability for Operational Technology (OT) environments. 

Full DNP3 plugin [documentation](docs/dnp3.md) can be viewed as part of fieldmanual, once the Caldera server is running. 

## Installation

To run CALDERA along with DNP3 plugin:
1. Download CALDERA as detailed in the [Installation Guide](https://github.com/mitre/caldera)
2. Install the dnp3 plugin in CALDERA's plugin directory: `caldera/plugins`
3. Enable the dnp3 plugin by adding `- dnp3` to the list of enabled plugins in `conf/local.yml` or `conf/default.yml` (if running CALDERA in insecure mode)

### Version
This plugin is compatible with the current version of CALDERA v4.1.0 as of 26 Jan 2023. This can be checked out using the following method:
```
git clone --recursive https://github.com/mitre/caldera.git
```
### Tested OS Versions for Plugin Payload(s)

Building of the DNP3 plugin payloads has been tested on Windows 10 64-bit using VSCode as described [here](https://code.visualstudio.com/docs/cpp/config-mingw). See the corresponding plugin payload source code for further build information. 

Testing of the binaries has occured on:
* Microsoft Windows 10 v21H2

## Plugin Usage
 - Import the plugin, and optionally set up the required facts (i.e. like the fact sources provided). 
 - Start an operation, optionally using the fact source you set up. 
 - Use "Add Potential Link" to run a specific ability from this plugin. You can enter the fact values manually, or use the ones from your fact source. 

 Sources contains a small [example fact set](/data/sources/1fd0f487-c3e4-4ab2-8946-c0c8f37aa197.yml) and the fieldmanual documentation contains a reference section on [DNP3 sources.](/docs/dnp3.md#dnp3-sources-and-facts) 
