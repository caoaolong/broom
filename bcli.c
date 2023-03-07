//
// Created by Administrator on 2023-03-07.
//
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main (int argc, char *argv[])
{
    int fd = open("./test", O_RDONLY);
    void *p = mmap(NULL, 1024, PROT_READ, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED) {
        perror("mmap failed");
        exit(1);
    }
    printf("%s\n", (char *)p);
    close(fd);

    sleep(100);
    munmap(p, 1024);
    return 0;
}