//
// Created by Administrator on 2023-03-04.
//

#ifndef BROOM_BROOM_H
#define BROOM_BROOM_H

#define BROOM_CLIENT_SIZE   0x3FF
#define BROOM_PATH_SIZE     0xFF
#define BROOM_BUFFER_SIZE   1500
#define BROOM_MMAP_SIZE     0x2000

#define BROOM_PREFIX        "./"
#define BROOM_CONF_PATH     "conf/broom.ini"
#define BROOM_DATA_PATH     "data/broom.brm"
#define BROOM_LOG_PATH      "log/broom.log"

#include <netinet/ip.h>
#include <pthread.h>
#include <iniparser.h>

typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;
typedef unsigned long   u64;

typedef struct broom_client_s broom_client_t;
struct broom_client_s {
    int     srcfd;
    int     clifd;
    int     memfd;
    void   *mem;
    ssize_t nsend;

    /** Pthread Attributes **/
    pthread_attr_t attr;
    pthread_t pthread;

    struct sockaddr_in *addr;
};

typedef struct broom_server_s broom_server_t;
struct broom_server_s {
    /** Server Attributes **/
    const char  *bind;
    int          port;
    const char  *def;

    int     sockfd;
    int     maxcfd, maxsfd;
    fd_set  cfdset, sfdset;

    /** Global Data **/
    dictionary *dict;

    /** Client Attributes **/
    // 客户端数量
    size_t          n_clients;
    broom_client_t *clients[BROOM_CLIENT_SIZE];
    int             sources[BROOM_CLIENT_SIZE];
};

void broom_init();

#endif //BROOM_BROOM_H
