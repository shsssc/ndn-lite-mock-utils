#ifndef _NDNLITE_MOCK
#define _NDNLITE_MOCK

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int readFromDevice(const char  *deviceName, const char  *key)
{
    char fullFileName[256], content[16];
    strcpy(fullFileName, "/tmp/ndnlite-mock/");
    strcat(fullFileName, deviceName);
    strcat(fullFileName, "/");
    strcat(fullFileName, key);
    int fd = open(fullFileName, O_RDONLY);
    if (fd < 0)
    {
        fprintf(stderr, "reading value that is not ready!\n");
        exit(1);
    }
    int nbytes = read(fd, content, 15);
    if (nbytes < 0)
    {
        fprintf(stderr, "error reading value!\n");
        exit(1);
    }
    content[nbytes] = 0;
    close(fd);
    return atoi(content);
}

void writeToDevice(const char *deviceName, const char *key, int value)
{
    char fullFileName[256], content[16];
    strcpy(fullFileName, "/tmp/ndnlite-mock/");
    mkdir(fullFileName, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    strcat(fullFileName, deviceName);
    strcat(fullFileName, "/");
    mkdir(fullFileName, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    strcat(fullFileName, key);
    sprintf(content, "%d", value);
    int fd = open(fullFileName, O_CREAT | O_WRONLY|O_TRUNC, 0644);
    if (fd < 0)
    {
        fprintf(stderr, "writing file that is not ready!\n");
        exit(1);
    }
    write(fd, content, strlen(content));
    close(fd);
}

#endif
