# Hardware key-value storage mock utilities

This is a set of c and shell libraries to easily mock devices as key-value storage. It's not for performance and should only use for testing purposes.

## Usage of CLI utility

build:

```
make
```

set value: 

```shell
./mock-util led brightness 15
```

read value:

```shell
read value: ./mock-util led brightness
```

monitor value:

```shell
monitor value: ./mock-util led brightness
```

## Usage of C library

Include the file `mockhw.h`. You can use the `main.c` as example

```cpp
#include <stdio.h>
#include "mockhw.h"

int main(int argc, char *argv[])
{

    if (argc == 3)
    {
        printf("get value of \"%s\" from device %s: %d\n", argv[2], argv[1], readFromDevice(argv[1], argv[2]));
    }
    if (argc == 4)
    {
        printf("set value of \"%s\" to device %s\n", argv[2], argv[1]);
        writeToDevice(argv[1], argv[2],atoi(argv[3]));
    }
}
```

## Details

This library uses `/tmp` together with file system to achieve its functionality ~~quick and dirty~~. The following shell scripts command shows the details. 

**read info from device**

```shell
#!/bin/bash
echo name: $1
echo key: $2
printf `cat /tmp/ndnlite-mock/$1/$2`
```

**write to device**

```shell
#!/bin/bash
echo name: $1
echo key: $2
echo value $3
mkdir -p /tmp/ndnlite-mock/$1
printf $3 > /tmp/ndnlite-mock/$1/$2
```

