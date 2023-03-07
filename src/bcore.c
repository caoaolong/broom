//
// Created by Administrator on 2023-03-07.
//

#include <broom.h>
#include <sys/mman.h>
#include <fcntl.h>

extern broom_server_t server;

void broom_init()
{
    char buffer[BROOM_PATH_SIZE];
    sprintf(buffer, "%s%s", BROOM_PREFIX, BROOM_DATA_PATH);
    int fd = open(buffer, O_RDWR | O_CREAT, 0644);
    server.memfd = fd;
    ftruncate(fd, BROOM_MMAP_SIZE);
    server.mem = mmap(NULL, BROOM_MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (!server.mem) {
        perror("mmap failed");
        exit(1);
    }
}