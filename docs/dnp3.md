# DNP3
The DNP3 plugin for Caldera provides adversary emulation abilities specific to the DNP3 control systems protocol.

v2.0 released 05 Dec 2024

## Overview
The DNP3 plugin provides __11__ unique abilities specific to the DNP3 protocol offered for both serial and TCP connections (22 total abilities). This is accomplished not through exploitation, but rather by leveraging native functionality within the protocol. 

The following table outlines MITRE ATT&CK for ICS coverage provided by the DNP3 plugin.

| [Collection](#collection-abilities) | [Inhibit Response Function](#inhibit-response-function-abilities) | [Impair Process Control](#impair-process-control-abilities) |
|:--|:--|:--|
| Automated Collection       | Device Restart/Shutdown | Unauthorized Command Message | 
| Point & Tag Identification | Block Reporting Message |       Modify Parameter       |
|                            | Denial of Service       |                              |

### Ability Overview Tables
The following table(s) list each plugin ability by their corresponding tactic.

#### Collection Abilities
| Ability | Technique | Technique Id |
|:--|:--|:--|
|[DNP3 - Read](#dnp3---read)|Point & Tag Identification|T0861|
|[DNP3 - Read All](#dnp3---read-all)|Point & Tag Identification|T0861|
|[DNP3 - Integrity Poll](#dnp3---integrity-poll)|Automated Collection|T0802|
|[DNP3 - Enable Unsolicited Messages](#dnp3---enable-unsolicited-messages)|Automated Collection|T0802|

#### Inhibit Response Function Abilities
| Ability | Technique | Technique Id |
|:--|:--|:--|
|[DNP3 - Cold Restart](#dnp3---cold-restart) | Device Restart/Shutdown | T0816 |
|[DNP3 - Warm Restart](#dnp3---warm-restart) | Device Restart/Shutdown | T0816 |
|[DNP3 - Disable Unsolicited Messages](#dnp3---disable-unsolicited-messages) | Block Reporting Message | T0804 |

#### Impair Process Control Abilities
| Ability | Technique | Technique Id |
|:--|:--|:--|
|[DNP3 - Operate](#dnp3---operate)|Unauthorized Command Message|T0855|
|[DNP3 - Toggle (Two-Output Model)](#dnp3---toggle-two-output-model)|Unauthorized Command Message|T0855|
|[DNP3 - Toggle (Activation Model)](#dnp3---toggle-activation-model)|Unauthorized Command Message|T0855|
|[DNP3 - Set Analog](#dnp3---set-analog)|Modify Parameter|T0836|


## Architecture
This section describes the main components of the plugin and how they interface.

### Block Diagram
![block diagram](./assets/dnp3_bd.png)

The DNP3 plugin exposes several new protocol specific abilities to your Caldera instance. The abilities are executed from a host running a Caldera agent via the corresponding payload. Abilities must target devices that support the DNP3 protocol to achieve described effects.

### Payloads
The DNP3 plugin includes one payload that implements the abilities, compiled for three different host architectures:
- `dnp3-actions.exe` (Windows)
- `dnp3-actions` (Linux)
- `dnp3-actions_darwin` (Mac) 

### Libraries
The following libraries were used to build the DNP3 payloads:  

|Library|Version|License|
|:--|:--|:--|
|OpenDNP3|[v3.1.2](https://github.com/dnp3/opendnp3)|[Apache 2.0](https://github.com/dnp3/opendnp3/blob/release/LICENSE)|
|CLI11|[v2.4.2](https://github.com/CLIUtils/CLI11)|[License](https://github.com/CLIUtils/CLI11/blob/main/LICENSE)|

## Usage
This section describes how to initially deploy and execute the abilities present within the DNP3 Plugin.

### Deployment
1. Identify the target system you would like to communicate with via the DNP3 protocol.
2. Identify a viable host for the Caldera agent that will be sending DNP3
   messages to the target system. A viable host has a network connection to the target 
   system and has an architecture that is compatible with one of the plugin's payloads.
3. Deploy the Caldera agent to the viable host.
4. Run a combination of the DNP3 plugin abilities to achieve the desired effect.  

_If you don't know where to begin, try using [DNP3 - Integrity Poll](#dnp3---integrity-poll) to read all of the data on a device._

```{tip}
Reference the Caldera training plugin for a step-by-step tutorial on how to deploy an agent and run abilities via an operation.
```


### Test Server (Outstation) Usage
This plugin includes a test server (known in DNP3 parlance as an outstation) in the `plugins/` directory that can be used to create a test environment for the plugin.

1. Start the Caldera server and deploy a Caldera agent.
2. On the device where the agent is running, start the DNP3 server using the following commands:

<details open>
<summary>Windows (psh)</summary>

```powershell
.\dnp3-outstation.exe --tui
```
</details>
<details>
<summary>Linux (sh)</summary>

```sh
./dnp3-outstation --tui
```  

</details>
<details>
<summary>Darwin (sh)</summary>

```sh
./dnp3-outstation_darwin --tui
```  

</details>
<br>

3. Execute a DNP3 ability from your agent using the Caldera server. "DNP3 (TCP) - Integrity Poll" is a good first test. To target the DNP3 test server, ensure you select the following facts:
  - `dnp3.server.ip`: 127.0.0.1
  - `dnp3.link.local`: 1
  - `dnp3.link.remote`: 1024

4. The outstation is designed to demonstrate the different features and concepts in the DNP3 protocol and is not intended to be an authentic replica of any specific device. Try discovering data on the device, reading and writing analog values, and operating points with different operation types and trip control codes. 

### Network Connection

This plugin allows the agent to send DNP3 messages using a **IP/TCP** or **serial** connection. The first argument to the `dnp3-actions` payload selects the connection type. This must be followed by several additional arguments to establish the connection.

#### IP/TCP

__Positional arguments (required):__
| Name | Description | Type |
|:-----|:------------|:----:|
| `dnp3.server.ip` | IP address of the outstation | string  |
| `dnp3.local.link` | DNP3 link layer address of the local device | int |
| `dnp3.remote.link` | DNP3 link layer address of the remote device | int |

__Flags (optional):__
| Flag | Description | Type | Default |
|:-----|:------------|:----:|:-------:|
| `-p`, `--port` | port number of the outstation | int | 20000 |

#### Serial

__Positional arguments (required):__
| Name | Description | Type |
|:-----|:------------|:----:|
| `dnp3.server.serial_device` | Serial device name (e.g. COM1, /dev/ttyS0) | string  |
| `dnp3.local.link` | DNP3 link layer address of the local device | int |
| `dnp3.remote.link` | DNP3 link layer address of the remote device | int |

__Flags (optional):__
| Flag | Description | Type | Default |
|:-----|:------------|:----:|:-------:|
| `-b`,`--baud` | baud rate of the serial device | int | 9600 |
| `--databits` | data bits | int | 8 |
| `--stopbits` | stop bits, one of: 'One', 'OnePointFive', 'Two', 'None' | string | 'One' |
| `--parity` | parity, one of: 'Even', 'Odd', 'None' | string | 'None' |
| `--flowtype` | flow control setting, one of 'Hardware', 'XONXOFF', 'None' | string | 'None' |
| `--delay` | delay time in milliseconds before first tx | int | 500 |

### Abilities

#### DNP3 - Read
Read the specified values from the outstation.  

```{seealso}
For more information, see the [Data Model](#data-model) section.
```

<details open>
<summary><strong>TCP</strong></summary>
<br>

__Ability Command:__
<details open>
<summary>Windows (cmd/psh)</summary>
<br>

```caldera
.\dnp3-actions.exe tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} read #{dnp3.data.group} --start #{dnp3.data.start} --end #{dnp3.data.end}
```  

</details>
<details>
<summary>Linux (sh)</summary>
<br>

```caldera
./dnp3-actions tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} read #{dnp3.data.group} --start #{dnp3.data.start} --end #{dnp3.data.end}
```  

</details>
<details>
<summary>Darwin (sh)</summary>
<br>

```caldera
./dnp3-actions_darwin tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} read #{dnp3.data.group} --start #{dnp3.data.start} --end #{dnp3.data.end}
```  

</details>
<br>

__Facts:__  
| Name | Description | Type |
|:-----|:------------|:----:|
| `dnp3.server.ip` | IP address of the outstation | string |
| `dnp3.local.link` | DNP3 link layer address of the local device | int |
| `dnp3.remote.link` | DNP3 link layer address of the remote device | int |
| `dnp3.data.group` | DNP3 datatype | int  |
| `dnp3.data.start` | First index to read (inclusive) | int |
| `dnp3.data.end` | Final index to read (inclusive) | int |

__Optional Flags:__
| Flag | Description | Type | Default |
|:-----|:------------|:----:|:-------:|
| `-p`, `--port` | port number of the outstation | int | 20000 |
| `--variation` | variation (format) to read the data | int | None |
</details>

<details>
<summary><strong>Serial</strong></summary>
<br>

__Ability Command:__
<details open>
<summary>Windows (cmd/psh)</summary>
<br>

```caldera
.\dnp3-actions.exe serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} read #{dnp3.data.group} --start #{dnp3.data.start} --end #{dnp3.data.end}
```  

</details>
<details>
<summary>Linux (sh)</summary>
<br>

```caldera
./dnp3-actions serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} read #{dnp3.data.group} --start #{dnp3.data.start} --end #{dnp3.data.end}
```  

</details>
<details>
<summary>Darwin (sh)</summary>
<br>

```caldera
./dnp3-actions_darwin serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} read #{dnp3.data.group} --start #{dnp3.data.start} --end #{dnp3.data.end}
```  

</details>
<br>

__Facts:__  
| Name | Description | Type |
|:-----|:------------|:----:|
| `dnp3.server.serial_device` | Serial device name (e.g. COM1, /dev/ttyS0) | string  |
| `dnp3.local.link` | DNP3 link layer address of the local device | int |
| `dnp3.remote.link` | DNP3 link layer address of the remote device | int |
| `dnp3.data.group` | DNP3 datatype | int  |
| `dnp3.data.start` | First index to read (inclusive) | int |
| `dnp3.data.end` | Final index to read (inclusive) | int |

__Optional Flags:__
| Flag | Description | Type | Default |
|:-----|:------------|:----:|:-------:|
| `-b`,`--baud` | baud rate of the serial device | int | 9600 |
| `--databits` | data bits | int | 8 |
| `--stopbits` | stop bits, one of: 'One', 'OnePointFive', 'Two', 'None' | string | 'One' |
| `--parity` | parity, one of: 'Even', 'Odd', 'None' | string | 'None' |
| `--flowtype` | flow control setting, one of 'Hardware', 'XONXOFF', 'None' | string | 'None' |
| `--delay` | delay time in milliseconds before first tx | int | 500 |
| `--variation` | variation (format) to read the data | int | None |
</details>
<br>

#### DNP3 - Read All
Read the specified values from a certain group of the outstation.  

```{seealso}
For more information, see the [Data Model](#data-model) section.
```
<details open>
<summary><strong>TCP</strong></summary>
<br>

__Ability Command:__
<details open>
<summary>Windows (cmd/psh)</summary>
<br>

```caldera
.\dnp3-actions.exe tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} read #{dnp3.data.group} 
```  

</details>
<details>
<summary>Linux (sh)</summary>
<br>

```caldera
./dnp3-actions tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} read #{dnp3.data.group} 
```  

</details>
<details>
<summary>Darwin (sh)</summary>
<br>

```caldera
./dnp3-actions_darwin tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} read #{dnp3.data.group}
```  

</details>
<br>

__Facts:__  
| Name | Description | Type |
|:-----|:------------|:----:|
| `dnp3.server.ip` | IP address of the outstation | string |
| `dnp3.local.link` | DNP3 link layer address of the local device | int |
| `dnp3.remote.link` | DNP3 link layer address of the remote device | int |
| `dnp3.data.group` | DNP3 datatype | int  |
| `dnp3.data.variation` | Variation of the DNP3 datatype (group) | int |

__Optional Flags:__
| Flag | Description | Type | Default |
|:-----|:------------|:----:|:-------:|
| `-p`, `--port` | port number of the outstation | int | 20000 |
| `--variation` | variation (format) to read the data | int | None |
</details>

<details>
<summary><strong>Serial</strong></summary>
<br>

__Ability Command:__
<details open>
<summary>Windows (cmd/psh)</summary>
<br>

```caldera
.\dnp3-actions.exe serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} read #{dnp3.data.group} 
```  

</details>
<details>
<summary>Linux (sh)</summary>
<br>

```caldera
./dnp3-actions serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} read #{dnp3.data.group} 
```  

</details>
<details>
<summary>Darwin (sh)</summary>
<br>

```caldera
./dnp3-actions_darwin serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} read #{dnp3.data.group}
```  

</details>
<br>

__Facts:__  
| Name | Description | Type |
|:-----|:------------|:----:|
| `dnp3.server.serial_device` | Serial device name (e.g. COM1, /dev/ttyS0) | string  |
| `dnp3.local.link` | DNP3 link layer address of the local device | int |
| `dnp3.remote.link` | DNP3 link layer address of the remote device | int |
| `dnp3.data.group` | DNP3 datatype | int  |
| `dnp3.data.variation` | Variation of the DNP3 datatype (group) | int |
| `dnp3.data.start` | First index to read (inclusive) | int |
| `dnp3.data.end` | Final index to read (inclusive) | int |

__Optional Flags:__
| Flag | Description | Type | Default |
|:-----|:------------|:----:|:-------:|
| `-b`,`--baud` | baud rate of the serial device | int | 9600 |
| `--databits` | data bits | int | 8 |
| `--stopbits` | stop bits, one of: 'One', 'OnePointFive', 'Two', 'None' | string | 'One' |
| `--parity` | parity, one of: 'Even', 'Odd', 'None' | string | 'None' |
| `--flowtype` | flow control setting, one of 'Hardware', 'XONXOFF', 'None' | string | 'None' |
| `--delay` | delay time in milliseconds before first tx | int | 500 |
| `--variation` | variation (format) to read the data | int | None |
</details>
<br>

#### DNP3 - Integrity Poll
Read all data groups present on the outstation.  

<details open>
<summary><strong>TCP</strong></summary>
<br>

__Ability Command:__

<details open>
<summary>Windows (cmd/psh)</summary>
<br>

```caldera
.\dnp3-actions.exe tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} integrity-poll
```  

</details>
<details>
<summary>Linux (sh)</summary>
<br>

```caldera
./dnp3-actions tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} integrity-poll
```  

</details>
<details>
<summary>Darwin (sh)</summary>
<br>

```caldera
./dnp3-actions_darwin tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} integrity-poll
```  

</details>
<br>

__Facts:__  
| Name | Description | Type |
|:-----|:------------|:----:|
| `dnp3.server.ip` | IP address of the outstation | string  |
| `dnp3.local.link` | DNP3 link layer address of the local device | int |
| `dnp3.remote.link` | DNP3 link layer address of the remote device | int |


__Optional Flags:__
| Flag | Description | Type | Default |
|:-----|:------------|:----:|:-------:|
| `-p`, `--port` | port number of the outstation | int | 20000 |
</details>

<details>
<summary><strong>Serial</strong></summary>
<br>

__Ability Command:__

<details open>
<summary>Windows (cmd/psh)</summary>
<br>

```caldera
.\dnp3-actions.exe serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} integrity-poll
```  

</details>
<details>
<summary>Linux (sh)</summary>
<br>

```caldera
./dnp3-actions serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} integrity-poll
```  

</details>
<details>
<summary>Darwin (sh)</summary>
<br>

```caldera
./dnp3-actions_darwin serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} integrity-poll
```  

</details>
<br>

__Facts:__  
| Name | Description | Type |
|:-----|:------------|:----:|
| `dnp3.server.serial_device` | Serial device name (e.g. COM1, /dev/ttyS0) | string  |
| `dnp3.local.link` | DNP3 link layer address of the local device | int |
| `dnp3.remote.link` | DNP3 link layer address of the remote device | int |

__Optional Flags:__
| Flag | Description | Type | Default |
|:-----|:------------|:----:|:-------:|
| `-b`,`--baud` | baud rate of the serial device | int | 9600 |
| `--databits` | data bits | int | 8 |
| `--stopbits` | stop bits, one of: 'One', 'OnePointFive', 'Two', 'None' | string | 'One' |
| `--parity` | parity, one of: 'Even', 'Odd', 'None' | string | 'None' |
| `--flowtype` | flow control setting, one of 'Hardware', 'XONXOFF', 'None' | string | 'None' |
| `--delay` | delay time in milliseconds before first tx | int | 500 |
</details>
<br>

#### DNP3 - Operate
Operate specified points utilizing SELECT_BEFORE_OPERATE (SBO) or DIRECT_OPERATE (DO)

```{seealso}
For more information on operating points in DNP3 see the [Control Models](#control-models) section.
```

<details open>
<summary><strong>TCP</strong></summary>
<br>

__Ability Command:__
<details open>
<summary>Windows (cmd/psh)</summary>
<br>

```caldera
./dnp3-actions.exe tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} #{dnp3.op.mode} --indices #{dnp3.operate.indices} --op-type #{dnp3.operate.type} --tcc #{dnp3.operate.tcc}
```

</details>
<details>
<summary>Linux (sh)</summary>
<br>

```caldera
./dnp3-actions tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} #{dnp3.op.mode} --indices #{dnp3.operate.indices} --op-type #{dnp3.operate.type} --tcc #{dnp3.operate.tcc}
```

</details>
<details>
<summary>Darwin (sh)</summary>
<br>

```caldera
./dnp3-actions_darwin tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} #{dnp3.op.mode} --indices #{dnp3.operate.indices} --op-type #{dnp3.operate.type} --tcc #{dnp3.operate.tcc}
```

</details>
<br>

__Facts:__  
| Name | Description | Type |
|:-----|:------------|:----:|
| `dnp3.server.ip` | IP address of the outstation | string  |
| `dnp3.local.link` | DNP3 link layer address of the local device | int |
| `dnp3.remote.link` | DNP3 link layer address of the remote device | int |
| `dnp3.operate.mode` | Choose either SBO or DO Mode | string |
| `dnp3.operate.indices` | Indices to send the CROB to (operate) | comma separated list of int |
| `dnp3.operate.type` | One of 'NUL', 'PULSE_ON', 'PULSE_OFF', 'LATCH_ON', 'LATCH_OFF' | string |
| `dnp3.operate.tcc` | One of 'NUL', 'CLOSE', 'TRIP' | string |

__Optional Flags:__
| Flag | Description | Type | Default |
|:-----|:------------|:----:|:-------:|
| `-p`, `--port` | port number of the outstation | int | 20000 |
| `--on`| signal on-time value in ms | int | 100 |
| `--off`| signal off-time value in ms | int | 100 |
| `--count` | times to repeat signal | int | 1 |
| `--clear` | set the control code clear bit | string | 
</details>
<details>
<summary><strong>Serial</strong></summary>
<br>

__Ability Command:__
<details open>
<summary>Windows (cmd/psh)</summary>
<br>

```caldera
./dnp3-actions.exe serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} #{dnp3.op.mode} --indices #{dnp3.operate.indices} --op-type #{dnp3.operate.type} --tcc #{dnp3.operate.tcc}
```

</details>
<details>
<summary>Linux (sh)</summary>
<br>

```caldera
./dnp3-actions serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} #{dnp3.op.mode} --indices #{dnp3.operate.indices} --op-type #{dnp3.operate.type} --tcc #{dnp3.operate.tcc}
```

</details>
<details>
<summary>Darwin (sh)</summary>
<br>

```caldera
./dnp3-actions_darwin serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} #{dnp3.op.mode} --indices #{dnp3.operate.indices} --op-type #{dnp3.operate.type} --tcc #{dnp3.operate.tcc}
```

</details>
<br>

__Facts:__  
| Name | Description | Type |
|:-----|:------------|:----:|
| `dnp3.server.serial_device` | Serial device name (e.g. COM1, /dev/ttyS0) | string  |
| `dnp3.local.link` | DNP3 link layer address of the local device | int |
| `dnp3.remote.link` | DNP3 link layer address of the remote device | int |
| `dnp3.operate.mode` | Choose either SBO or DO Mode | string |
| `dnp3.operate.indices` | Indices to send the CROB to (operate) | comma separated list of int |
| `dnp3.operate.type` | One of 'NUL', 'PULSE_ON', 'PULSE_OFF', 'LATCH_ON', 'LATCH_OFF' | string |
| `dnp3.operate.tcc` | One of 'NUL', 'CLOSE', 'TRIP' | string |

__Optional Flags:__
| Flag | Description | Type | Default |
|:-----|:------------|:----:|:-------:|
| `-b`,`--baud` | baud rate of the serial device | int | 9600 |
| `--databits` | data bits | int | 8 |
| `--stopbits` | stop bits, one of: 'One', 'OnePointFive', 'Two', 'None' | string | 'One' |
| `--parity` | parity, one of: 'Even', 'Odd', 'None' | string | 'None' |
| `--flowtype` | flow control setting, one of 'Hardware', 'XONXOFF', 'None' | string | 'None' |
| `--delay` | delay time in milliseconds before first tx | int | 500 |
| `--on`| signal on-time value in ms | int | 100 |
| `--off`| signal off-time value in ms | int | 100 |
| `--count` | times to repeat signal | int | 1 |
| `--clear` | set the control code clear bit | string | 
</details>
<br>

#### DNP3 - Toggle (Activation Model)
Toggle breakers using the activation model (Utilizing trip-indices and close-indices)

```{seealso}
For more information on operating points in DNP3 see the [Control Models](#control-models) section.
```

<details open>
<summary><strong>TCP</strong></summary>
<br>

__Ability Command:__
<details open>
<summary>Windows (cmd/psh)</summary>
<br>

```caldera
./dnp3-actions.exe tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} toggle-activation #{dnp3.op.mode} --trip-indices #{dnp3.trip.indices} --close-indices #{dnp3.close.indices}
```


</details>
<details>
<summary>Linux (psh)</summary>
<br>

```caldera
./dnp3-actions_darwin tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} toggle-activation #{dnp3.op.mode} --trip-indices #{dnp3.trip.indices} --close-indices #{dnp3.close.indices}
```

</details>
<details>
<summary>Darwin (psh)</summary>
<br>

```caldera
./dnp3-actions_darwin tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} toggle-activation #{dnp3.op.mode} --trip-indices #{dnp3.trip.indices} --close-indices #{dnp3.close.indices}
```

</details>
<br>

__Facts:__  
| Name | Description | Type |
|:-----|:------------|:----:|
| `dnp3.server.ip` | IP address of the outstation | string  |
| `dnp3.local.link` | DNP3 link layer address of the local device | int |
| `dnp3.remote.link` | DNP3 link layer address of the remote device | int |
| `dnp3.operate.mode` | Choose either SBO or DO Mode | string |
| `dnp3.trip.indices` | Trip-indices to send the CROB to (operate) | comma separated list of int |
| `dnp3.close.indices` | Close-indices to send the CROB to (operate) | comma separated list of int |
| `dnp3.operate.type` | One of 'NUL', 'PULSE_ON', 'PULSE_OFF', 'LATCH_ON', 'LATCH_OFF' | string |
| `dnp3.operate.tcc` | One of 'NUL', 'CLOSE', 'TRIP' | string |

__Optional Flags:__
| Flag | Description | Type | Default |
|:-----|:------------|:----:|:-------:|
| `-p`, `--port` | port number of the outstation | int | 20000 |
| `--on`| signal on-time value in ms | int | 100 |
| `--off`| signal off-time value in ms | int | 100 |
| `--iterations` | times to repeat signal | int | 1 |
| `--delay` | delay between trip and close operations in ms | int | 30000 |
</details>
<details>
<summary><strong>Serial</strong></summary>
<br>

__Ability Command:__
<details open>
<summary>Windows (cmd/psh)</summary>
<br>

```caldera
./dnp3-actions.exe serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} toggle-activation #{dnp3.op.mode} --trip-indices #{dnp3.trip.indices} --close-indices #{dnp3.close.indices}
```


</details>
<details>
<summary>Linux (psh)</summary>
<br>

```caldera
./dnp3-actions_darwin serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} toggle-activation #{dnp3.op.mode} --trip-indices #{dnp3.trip.indices} --close-indices #{dnp3.close.indices}
```

</details>
<details>
<summary>Darwin (psh)</summary>
<br>

```caldera
./dnp3-actions_darwin serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} toggle-activation #{dnp3.op.mode} --trip-indices #{dnp3.trip.indices} --close-indices #{dnp3.close.indices}
```

</details>
<br>

__Facts:__  
| Name | Description | Type |
|:-----|:------------|:----:|
| `dnp3.server.serial_device` | Serial device name (e.g. COM1, /dev/ttyS0) | string  |
| `dnp3.local.link` | DNP3 link layer address of the local device | int |
| `dnp3.remote.link` | DNP3 link layer address of the remote device | int |
| `dnp3.operate.mode` | Choose either SBO or DO Mode | string |
| `dnp3.trip.indices` | Trip-indices to send the CROB to (operate) | comma separated list of int |
| `dnp3.close.indices` | Close-indices to send the CROB to (operate) | comma separated list of int |
| `dnp3.operate.type` | One of 'NUL', 'PULSE_ON', 'PULSE_OFF', 'LATCH_ON', 'LATCH_OFF' | string |
| `dnp3.operate.tcc` | One of 'NUL', 'CLOSE', 'TRIP' | string |

__Optional Flags:__
| Flag | Description | Type | Default |
|:-----|:------------|:----:|:-------:|
| `-b`,`--baud` | baud rate of the serial device | int | 9600 |
| `--databits` | data bits | int | 8 |
| `--stopbits` | stop bits, one of: 'One', 'OnePointFive', 'Two', 'None' | string | 'One' |
| `--parity` | parity, one of: 'Even', 'Odd', 'None' | string | 'None' |
| `--flowtype` | flow control setting, one of 'Hardware', 'XONXOFF', 'None' | string | 'None' |
| `--delay` | delay time in milliseconds before first tx | int | 500 |
| `--on`| signal on-time value in ms | int | 100 |
| `--off`| signal off-time value in ms | int | 100 |
| `--iterations` | times to repeat signal | int | 1 |
| `--delay` | delay between trip and close operations in ms | int | 30000 |
</details>
<br>

#### DNP3 - Toggle (Two Output Model)
Toggle breakers using the complementary two-output model (Utilizing TRIP/CLOSE on the same index)

```{seealso}
For more information on operating points in DNP3 see the [Control Models](#control-models) section.
```

<details open>
<summary><strong>TCP</strong></summary>
<br>

__Ability Command:__
<details open>
<summary>Windows (cmd/psh)</summary>
<br>

```caldera
./dnp3-actions.exe tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} toggle-two-output #{dnp3.op.mode} --indices #{dnp3.operate.indices}
```

</details>
<details>
<summary>Linux (psh)</summary>
<br>

```caldera
./dnp3-actions_darwin tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} toggle-two-output #{dnp3.op.mode} --indices #{dnp3.operate.indices}
```

</details>
<details>
<summary>Darwin (psh)</summary>
<br>

```caldera
./dnp3-actions_darwin tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} toggle-two-output #{dnp3.op.mode} --indices #{dnp3.operate.indices}
```

</details>
<br>

__Facts:__  
| Name | Description | Type |
|:-----|:------------|:----:|
| `dnp3.server.ip` | IP address of the outstation | string  |
| `dnp3.local.link` | DNP3 link layer address of the local device | int |
| `dnp3.remote.link` | DNP3 link layer address of the remote device | int |
| `dnp3.operate.mode` | Choose either SBO or DO Mode | string |
| `dnp3.operate.indices` | Indices to send the CROB to (operate) | comma separated list of int |
| `dnp3.operate.type` | One of 'NUL', 'PULSE_ON', 'PULSE_OFF', 'LATCH_ON', 'LATCH_OFF' | string |
| `dnp3.operate.tcc` | One of 'NUL', 'CLOSE', 'TRIP' | string |

__Optional Flags:__
| Flag | Description | Type | Default |
|:-----|:------------|:----:|:-------:|
| `-p`, `--port` | port number of the outstation | int | 20000 |
| `--on`| signal on-time value in ms | int | 100 |
| `--off`| signal off-time value in ms | int | 100 |
| `--iterations` | times to repeat signal | int | 1 |
| `--delay` | delay between trip and close operations in ms | int | 30000 |
</details>

<details>
<summary><strong>Serial</strong></summary>
<br>

__Ability Command:__
<details open>
<summary>Windows (cmd/psh)</summary>
<br>

```caldera
./dnp3-actions.exe serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} toggle-two-output #{dnp3.op.mode} --indices #{dnp3.operate.indices}
```

</details>
<details>
<summary>Linux (psh)</summary>
<br>

```caldera
./dnp3-actions_darwin serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} toggle-two-output #{dnp3.op.mode} --indices #{dnp3.operate.indices}
```

</details>
<details>
<summary>Darwin (psh)</summary>
<br>

```caldera
./dnp3-actions_darwin serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} toggle-two-output #{dnp3.op.mode} --indices #{dnp3.operate.indices}
```

</details>
<br>

__Facts:__  
| Name | Description | Type |
|:-----|:------------|:----:|
| `dnp3.server.serial_device` | Serial device name (e.g. COM1, /dev/ttyS0) | string  |
| `dnp3.local.link` | DNP3 link layer address of the local device | int |
| `dnp3.remote.link` | DNP3 link layer address of the remote device | int |
| `dnp3.operate.mode` | Choose either SBO or DO Mode | string |
| `dnp3.operate.indices` | Indices to send the CROB to (operate) | comma separated list of int |
| `dnp3.operate.type` | One of 'NUL', 'PULSE_ON', 'PULSE_OFF', 'LATCH_ON', 'LATCH_OFF' | string |
| `dnp3.operate.tcc` | One of 'NUL', 'CLOSE', 'TRIP' | string |

__Optional Flags:__
| Flag | Description | Type | Default |
|:-----|:------------|:----:|:-------:|
| `-b`,`--baud` | baud rate of the serial device | int | 9600 |
| `--databits` | data bits | int | 8 |
| `--stopbits` | stop bits, one of: 'One', 'OnePointFive', 'Two', 'None' | string | 'One' |
| `--parity` | parity, one of: 'Even', 'Odd', 'None' | string | 'None' |
| `--flowtype` | flow control setting, one of 'Hardware', 'XONXOFF', 'None' | string | 'None' |
| `--delay` | delay time in milliseconds before first tx | int | 500 |
| `--on`| signal on-time value in ms | int | 100 |
| `--off`| signal off-time value in ms | int | 100 |
| `--iterations` | times to repeat signal | int | 1 |
| `--delay` | delay between trip and close operations in ms | int | 30000 |
</details>
<br>

#### DNP3 - Set Analog
Change the values of specified points


<details open>
<summary><strong>TCP</strong></summary>
<br>

__Ability Command:__
<details open>
<summary>Windows (cmd/psh)</summary>
<br>

```caldera
./dnp3-actions.exe tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} set-analog --indices #{dnp3.operate.indices} --values {dnp3.set.values}
```

</details>
<details>
<summary>Linux (psh)</summary>
<br>

```caldera
./dnp3-actions_darwin tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} set-analog --indices #{dnp3.operate.indices} --values {dnp3.set.values}
```

</details>
<details>
<summary>Darwin (psh)</summary>
<br>

```caldera
./dnp3-actions_darwin tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} set-analog --indices #{dnp3.operate.indices} --values {dnp3.set.values}
```

</details>
<br>

__Facts:__  
| Name | Description | Type |
|:-----|:------------|:----:|
| `dnp3.server.ip` | IP address of the outstation | string  |
| `dnp3.local.link` | DNP3 link layer address of the local device | int |
| `dnp3.remote.link` | DNP3 link layer address of the remote device | int |
| `dnp3.set.analog` | Set analog output values command | string |
| `dnp3.operate.indices` | Indices to send the CROB to (operate) | comma separated list of int |
| `dnp3.set.values` | Set values to output on specified point | comma separated list of int |

__Optional Flags:__
| Flag | Description | Type | Default |
|:-----|:------------|:----:|:-------:|
| `-p`, `--port` | Port number of the outstation | int | 20000 |
| `--dtype`| Datatype of values | string | 'DOUBLE64' |
</details>
<details>
<summary><strong>Serial</strong></summary>
<br>

__Ability Command:__
<details open>
<summary>Windows (cmd/psh)</summary>
<br>

```caldera
./dnp3-actions.exe serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} set-analog --indices #{dnp3.operate.indices} --values {dnp3.set.values}
```

</details>
<details>
<summary>Linux (psh)</summary>
<br>

```caldera
./dnp3-actions_darwin serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} set-analog --indices #{dnp3.operate.indices} --values {dnp3.set.values}
```

</details>
<details>
<summary>Darwin (psh)</summary>
<br>

```caldera
./dnp3-actions_darwin serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} set-analog --indices #{dnp3.operate.indices} --values {dnp3.set.values}
```

</details>
<br>

__Facts:__  
| Name | Description | Type |
|:-----|:------------|:----:|
| `dnp3.server.serial_device` | Serial device name (e.g. COM1, /dev/ttyS0) | string  |
| `dnp3.local.link` | DNP3 link layer address of the local device | int |
| `dnp3.remote.link` | DNP3 link layer address of the remote device | int |
| `dnp3.set.analog` | Set analog output values command | string |
| `dnp3.operate.indices` | Indices to send the CROB to (operate) | comma separated list of int |
| `dnp3.set.values` | Set values to output on specified point | comma separated list of int |

__Optional Flags:__
| Flag | Description | Type | Default |
|:-----|:------------|:----:|:-------:|
| `-b`,`--baud` | baud rate of the serial device | int | 9600 |
| `--databits` | data bits | int | 8 |
| `--stopbits` | stop bits, one of: 'One', 'OnePointFive', 'Two', 'None' | string | 'One' |
| `--parity` | parity, one of: 'Even', 'Odd', 'None' | string | 'None' |
| `--flowtype` | flow control setting, one of 'Hardware', 'XONXOFF', 'None' | string | 'None' |
| `--delay` | delay time in milliseconds before first tx | int | 500 |
| `--dtype`| Datatype of values | string | 'DOUBLE64' |
</details>
<br>

#### DNP3 - Cold Restart
Perform a full restart (cold restart) of the outstation. 

```{caution}
May leave the outstation in an unknown or invalid state
```

<details open>
<summary><strong>TCP</strong></summary>
<br>

__Ability Command:__
<details open>
<summary>Windows (cmd/psh)</summary>
<br>

```caldera
.\dnp3-actions.exe tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} cold-restart
```  

</details>
<details>
<summary>Linux (sh)</summary>
<br>

```caldera
./dnp3-actions tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} cold-restart
```  

</details>
<details>
<summary>Darwin (sh)</summary>
<br>

```caldera
./dnp3-actions_darwin tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} cold-restart
```  

</details>
<br>

__Facts:__  
| Name | Description | Type | Default |
|:-----|:------------|:----:|:-------:|
| `dnp3.server.ip` | IP address of the outstation | string  | None |
| `dnp3.local.link` | DNP3 link layer address of the local device | int | None |
| `dnp3.remote.link` | DNP3 link layer address of the remote device | int | None |

__Optional Flags:__
| Flag | Description | Type | Default |
|:-----|:------------|:----:|:-------:|
| `-p`, `--port` | port number of the outstation | int | 20000 |
</details>
<details>
<summary><strong>Serial</strong></summary>
<br>

__Ability Command:__
<details open>
<summary>Windows (cmd/psh)</summary>
<br>

```caldera
.\dnp3-actions.exe serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} cold-restart
```  

</details>
<details>
<summary>Linux (sh)</summary>
<br>

```caldera
./dnp3-actions serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} cold-restart
```  

</details>
<details>
<summary>Darwin (sh)</summary>
<br>

```caldera
./dnp3-actions_darwin serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} cold-restart
```  

</details>
<br>

__Facts:__  
| Name | Description | Type | Default |
|:-----|:------------|:----:|:-------:|
| `dnp3.server.serial_device` | Serial device name (e.g. COM1, /dev/ttyS0) | string  |
| `dnp3.local.link` | DNP3 link layer address of the local device | int | None |
| `dnp3.remote.link` | DNP3 link layer address of the remote device | int | None |

__Optional Flags:__
| Flag | Description | Type | Default |
|:-----|:------------|:----:|:-------:|
| `-b`,`--baud` | baud rate of the serial device | int | 9600 |
| `--databits` | data bits | int | 8 |
| `--stopbits` | stop bits, one of: 'One', 'OnePointFive', 'Two', 'None' | string | 'One' |
| `--parity` | parity, one of: 'Even', 'Odd', 'None' | string | 'None' |
| `--flowtype` | flow control setting, one of 'Hardware', 'XONXOFF', 'None' | string | 'None' |
| `--delay` | delay time in milliseconds before first tx | int | 500 |
</details>
<br>

#### DNP3 - Warm Restart
Perform a partial restart (warm restart) of the outstation. DNP3 applications
will be reset but not affect other processes. Sometimes, this may revert values
to defaults or reset to a known configuration.

```{caution}
May leave the outstation in an unknown or invalid state
```

<details open>
<summary><strong>TCP</strong></summary>
<br>

__Ability Command:__
<details open>
<summary>Windows (cmd/psh)</summary>
<br>

```caldera
.\dnp3-actions.exe tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} warm-restart
```  

</details>
<details>
<summary>Linux (sh)</summary>
<br>

```caldera
./dnp3-actions tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} warm-restart
```  

</details>
<details>
<summary>Darwin (sh)</summary>
<br>

```caldera
./dnp3-actions_darwin tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} warm-restart
```  

</details>
<br>

__Facts:__  
| Name | Description | Type | Default |
|:-----|:------------|:----:|:-------:|
| `dnp3.server.ip` | IP address of the outstation | string  | None |
| `dnp3.local.link` | DNP3 link layer address of the local device | int | None |
| `dnp3.remote.link` | DNP3 link layer address of the remote device | int | None |

__Optional Flags:__
| Flag | Description | Type | Default |
|:-----|:------------|:----:|:-------:|
| `-p`, `--port` | port number of the outstation | int | 20000 |
</details>
<details>
<summary><strong>Serial</strong></summary>
<br>

__Ability Command:__
<details open>
<summary>Windows (cmd/psh)</summary>
<br>

```caldera
.\dnp3-actions.exe serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} warm-restart
```  

</details>
<details>
<summary>Linux (sh)</summary>
<br>

```caldera
./dnp3-actions serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} warm-restart
```  

</details>
<details>
<summary>Darwin (sh)</summary>
<br>

```caldera
./dnp3-actions_darwin serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} warm-restart
```  

</details>
<br>

__Facts:__  
| Name | Description | Type | Default |
|:-----|:------------|:----:|:-------:|
| `dnp3.server.serial_device` | Serial device name (e.g. COM1, /dev/ttyS0) | string  |
| `dnp3.local.link` | DNP3 link layer address of the local device | int | None |
| `dnp3.remote.link` | DNP3 link layer address of the remote device | int | None |

__Optional Flags:__
| Flag | Description | Type | Default |
|:-----|:------------|:----:|:-------:|
| `-b`,`--baud` | baud rate of the serial device | int | 9600 |
| `--databits` | data bits | int | 8 |
| `--stopbits` | stop bits, one of: 'One', 'OnePointFive', 'Two', 'None' | string | 'One' |
| `--parity` | parity, one of: 'Even', 'Odd', 'None' | string | 'None' |
| `--flowtype` | flow control setting, one of 'Hardware', 'XONXOFF', 'None' | string | 'None' |
| `--delay` | delay time in milliseconds before first tx | int | 500 |
</details>
<br>

#### DNP3 - Disable Unsolicited Messages
Disable unsolicited messages on the outstation. May prevent clients connected to
the outstation from receiving event data that would otherwise be self-reported
by the outstation.

```{seealso}
[Enable Unsolicited Messages](#dnp3---enable-unsolicited-messages) Ability 
```

<details open>
<summary><strong>TCP</strong></summary>
<br>

__Ability Command:__
<details open>
<summary>Windows (cmd/psh)</summary>
<br>

```caldera
.\dnp3-actions.exe tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} disable-unsolicited
```  

</details>
<details>
<summary>Linux (sh)</summary>
<br>

```caldera
./dnp3-actions tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} disable-unsolicited
```  

</details>
<details>
<summary>Darwin (sh)</summary>
<br>

```caldera
./dnp3-actions_darwin tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} disable-unsolicited
```  

</details>
<br

__Facts:__  
| Name | Description | Type | Default |
|:-----|:------------|:----:|:-------:|
| `dnp3.server.ip` | IP address of the outstation | string  | None |
| `dnp3.local.link` | DNP3 link layer address of the local device | int | None |
| `dnp3.remote.link` | DNP3 link layer address of the remote device | int | None |

__Optional Flags:__
| Flag | Description | Type | Default |
|:-----|:------------|:----:|:-------:|
| `-p`, `--port` | port number of the outstation | int | 20000 |
| `--classes` | space separated list of classes to disable | string | '1,2,3' |
</details>
<details>
<summary><strong>Serial</strong></summary>
<br>

__Ability Command:__
<details open>
<summary>Windows (cmd/psh)</summary>
<br>

```caldera
.\dnp3-actions.exe serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} disable-unsolicited
```  

</details>
<details>
<summary>Linux (sh)</summary>
<br>

```caldera
./dnp3-actions serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} disable-unsolicited
```  

</details>
<details>
<summary>Darwin (sh)</summary>
<br>

```caldera
./dnp3-actions_darwin serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} disable-unsolicited
```  

</details>
<br

__Facts:__  
| Name | Description | Type | Default |
|:-----|:------------|:----:|:-------:|
| `dnp3.server.serial_device` | Serial device name (e.g. COM1, /dev/ttyS0) | string  |
| `dnp3.local.link` | DNP3 link layer address of the local device | int | None |
| `dnp3.remote.link` | DNP3 link layer address of the remote device | int | None |

__Optional Flags:__
| Flag | Description | Type | Default |
|:-----|:------------|:----:|:-------:|
| `-b`,`--baud` | baud rate of the serial device | int | 9600 |
| `--databits` | data bits | int | 8 |
| `--stopbits` | stop bits, one of: 'One', 'OnePointFive', 'Two', 'None' | string | 'One' |
| `--parity` | parity, one of: 'Even', 'Odd', 'None' | string | 'None' |
| `--flowtype` | flow control setting, one of 'Hardware', 'XONXOFF', 'None' | string | 'None' |
| `--delay` | delay time in milliseconds before first tx | int | 500 |
| `--classes` | space separated list of classes to disable | string | '1,2,3' |
</details>
<br>

#### DNP3 - Enable Unsolicited Messages
Enable unsolicited messages on the outstation. 

```{seealso}
[Disable Unsolicited Messages](#dnp3---disable-unsolicited-messages) Ability 
```

<details open>
<summary><strong>TCP</strong></summary>
<br>

__Ability Command:__
<details open>
<summary>Windows (cmd/psh)</summary>
<br>

```caldera
.\dnp3-actions.exe tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} enable-unsolicited
```  

</details>
<details>
<summary>Linux (sh)</summary>
<br>

```caldera
./dnp3-actions tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} enable-unsolicited
```  

</details>
<details>
<summary>Darwin (sh)</summary>
<br>

```caldera
./dnp3-actions_darwin tcp #{dnp3.server.ip} #{dnp3.local.link} #{dnp3.remote.link} enable-unsolicited
```  

</details>
<br>

__Facts:__  
| Name | Description | Type | Default |
|:-----|:------------|:----:|:-------:|
| `dnp3.server.ip` | IP address of the outstation | string  | None |
| `dnp3.local.link` | DNP3 link layer address of the local device | int | None |
| `dnp3.remote.link` | DNP3 link layer address of the remote device | int | None |

__Optional Flags:__
| Flag | Description | Type | Default |
|:-----|:------------|:----:|:-------:|
| `-p`, `--port` | port number of the outstation | int | 20000 |
| `--classes` | space separated list of classes to disable | string | '1,2,3' |
</details>
<details>
<summary><strong>Serial</strong></summary>
<br>

__Ability Command:__
<details open>
<summary>Windows (cmd/psh)</summary>
<br>

```caldera
.\dnp3-actions.exe serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} enable-unsolicited
```  

</details>
<details>
<summary>Linux (sh)</summary>
<br>

```caldera
./dnp3-actions serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} enable-unsolicited
```  

</details>
<details>
<summary>Darwin (sh)</summary>
<br>

```caldera
./dnp3-actions_darwin serial #{dnp3.server.serial_device} #{dnp3.local.link} #{dnp3.remote.link} enable-unsolicited
```  

</details>
<br>

__Facts:__  
| Name | Description | Type | Default |
|:-----|:------------|:----:|:-------:|
| `dnp3.server.serial_device` | Serial device name (e.g. COM1, /dev/ttyS0) | string  |
| `dnp3.local.link` | DNP3 link layer address of the local device | int | None |
| `dnp3.remote.link` | DNP3 link layer address of the remote device | int | None |

__Optional Flags:__
| Flag | Description | Type | Default |
|:-----|:------------|:----:|:-------:|
| `-b`,`--baud` | baud rate of the serial device | int | 9600 |
| `--databits` | data bits | int | 8 |
| `--stopbits` | stop bits, one of: 'One', 'OnePointFive', 'Two', 'None' | string | 'One' |
| `--parity` | parity, one of: 'Even', 'Odd', 'None' | string | 'None' |
| `--flowtype` | flow control setting, one of 'Hardware', 'XONXOFF', 'None' | string | 'None' |
| `--delay` | delay time in milliseconds before first tx | int | 500 |
| `--classes` | space separated list of classes to disable | string | '1,2,3' |
</details>
<br>

### DNP3 Protocol Notes

#### Data Model
When reading data from a DNP3 outstation, the user must specify the 'Group' and
'Variation' to read. The 'Group' can be thought of as similar to a datatype (see
the table below for examples). Each Group has several 'Variations', which are
different ways in which a Group can be read. For example, Group 2 is 'Binary
Input Events', Variation 1 reads the data without a time value, while Variation
1 reads the data with absolute time. **By default, this plugin requests all reads**
**with variation 0, indicating no preference of read type.** The optional `--variation`
flag can be used to specify a particular variation.

| Group | Name |
|:-----:|:----:|
| 1 | Binary Input |
| 2 | Binary Input Event |
| 3 | Double-bit Binary Input |
| 4 | Double-bit Binary Input Event |
| 10 | Binary Output |
| 11 | Binary Output Event |
| 12 | Binary Command |
| 13 | Binary Command Event |
| 20 | Counter |
| 21 | Frozen Counter |
| 22 | Counter Event |
| 23 | Frozen Counter Event |
| 30 | Analog Input |
| 32 | Analog Input Event |
| 40 | Analog Output Status |
| 41 | Analog Output |
| 42 | Analog Output Event |
| 43 | Analog Command Event |
| 50 | Time and Date |
| 51 | Time and Date CTO |
| 52 | Time Delay |
| 60 | Class Data |
| 70 | File-control |
| 80 | Internal Indications |
| 110 | Octet String |
| 111 | Octet String Event |
| 112 | Virtual Terminal Output Block |
| 113 | Virtual Terminal Event Data |

#### Control Models

There are two control philosophies in DNP3 (and this plugin):
select-before-operate and direct operate.  Using the select-before-operate
procedure, the controlling station (the Caldera payload) must first send a
SELECT message, indicating the intention to operate a point. This is followed by
an OPERATE message to actually execute the operation. If the outstation is set
for select-before-operate mode and receives an unexpected OPERATE message (one
not preceded by a SELECT message), it will ignore the command. In the direct
operate mode, the preceding SELECT message is not required.

The action of an OPERATE command is defined by its control relay output block
(CROB). Control abilities in this plugin contain the necessary fact templates
(arguments) to build a CROB. There are three control models that a outstation may implement
and the CROB must be constructed according to the appropriate model. The models are:
- Activation model
- Complementary latch model
- Complementary two-output model

The **activation model** functions like a push-button, where a single virtual or
physical output is linked to a specific event, such as "trip breaker" or
"acknowledge alarm." Pressing the button triggers the associated event. These
outputs are usually configured in pairs, with separate indices for actions like
"trip breaker" and "close breaker." **In this model, Control Relay Output Blocks**
**(CROBs) typically use the 'PULSE_ON' operation type and the 'NUL' trip close**
**code.** Using the push-button analogy, 'PULSE_ON' represents the act of pressing
the button, not the action itself. To trip a breaker, the operator sends a
(NUL, PULSE_ON) CROB to the trip breaker index, and to close the breaker, a
(NUL, PULSE_ON) CROB is sent to the close breaker index.

The **complementary latch model** operates like a light switch, where a single
virtual or physical output remains in the on (active) or off (inactive) state
based on the command it receives. This allows a single index to manage both
on-off or enable-disable functions. **This model is configured for the 'NUL' trip**
**close code and uses 'LATCH_ON' and 'LATCH_OFF' operation types to activate or**
**deactivate the output, respectively.** It is generally used for simple controls,
such as turning an indicator light on or off, rather than for controlling
breakers.

The **complementary two-output model** consists of two virtual or physical outputs:
a close and a trip output. Depending on the command received, one of these
outputs is momentarily activated, allowing a breaker to be controlled from a
single index. **This model is configured for the 'TRIP' and 'CLOSE' trip close codes**
**and uses the 'PULSE_ON' operation type.** To trip a breaker, the operator sends
a (TRIP, PULSE_ON) CROB to the breaker index, and to close the breaker, a
(CLOSE, PULSE_ON) CROB is sent to the breaker index.

The CROB also contains on-time and off-time fields to set the time durations of the
signal. These values are suggestions and may be overridden by the outstation.


## Copyright Notice
As of September 2nd, 2022, the OpenDNP3 project was archived by the project owner. The latest and final release is 3.1.2 on April 22nd, 2022. The source may still be found [on GitHub](https://github.com/dnp3/opendnp3).

OpenDNP3 is (c) to Green Energy Corp (2010, 2011), Step Function I/O LLC (2013-2022, 2020-2022), and various contributors (2010-2022). Note that Automatak rebranded as Step Function I/O.

It is licensed under the terms of the [Apache License 2.0](https://github.com/dnp3/opendnp3/blob/release/LICENSE), whose main condition requires "preservation of copyright and license notices."

The Caldera plugin is named "DNP3" as that is a short identifier of is purpose/scope. The plugin is not produced by Step Function I/O LLC.
