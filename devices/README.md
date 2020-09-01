# NDN-lite device mock for testing

Generate code and binaries that act as IoT devices in ndn-lite network. Those devices will have "hardware states" mocked with  `../hardware-interface`. Thus, programmers can read/write those mocked hardware states with utilities of that library. In this way, automation tasks can be easily tested on generating mock devices.

## How to use

1. Write config (a c header file full of macros) of the device you want to mock into `configs/<your project name>/sensor` and `configs/<your project name>/command_receiver`
   1. sensor is a device that act as sensor(it reads hardware state).
   2. command_receiver should change hardware state based on commands (for example, a LED light that can be controlled).
2. pass the TASK parameter  to make to compile your mocked device
   1. `make TASK=<your project name>`
3. use the generated binaries together with their pre-defined encryption keys.

## Example config file

**command receiver config**

```c
#define MAX_VALUE 120
#define STATE_NAME brightness
#define DEVICE_NAME led
#define SERVICE_ID NDN_SD_LED
```

note: after the device runs, you can use ` watch ../hardware-interface/mock-util led brightness` to monitor hardware states.

**sensor config**

```c
#define STATE_NAME has-smoke
#define DEVICE_NAME smoke-detector
#define SERVICE_ID 51
#define REFRESH_PERIOD 500
```

note: after the device runs, you can use `../hardware-interface/mock-util smoke-detector has-smoke <new value>` to modify hardware states.
