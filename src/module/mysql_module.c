//
// Created by Administrator on 2023-03-04.
//

#include <bmodule.h>
#include <module/bmodule_mysql.h>
#include <arpa/inet.h>

extern broom_server_t server;

void module_mysql_init(broom_module_t *module)
{

}

int module_mysql_connect(broom_module_t *module)
{
    dictionary *dict = server.dict;
    const char *host = iniparser_getstring(dict, "mysql:host", "127.0.0.1");
    int port = iniparser_getint(dict, "mysql:port", 3306);

    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    server.maxsfd = fd;
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = inet_addr(host);

    int ret = connect(fd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret < 0) {
        fprintf(stderr, "connect failed: %s:%d", host, port);
        exit(1);
    }

    fd_set *fdset = &server.cfdset;
    FD_SET(fd, fdset);

    return fd;
}