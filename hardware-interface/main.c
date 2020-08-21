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