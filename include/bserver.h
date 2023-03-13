//
// Created by Administrator on 2023-03-04.
//

#ifndef BROOM_BSERVER_H
#define BROOM_BSERVER_H

#include <broom.h>

void bserver_init(const char *ip, int port, const char *def);
void bserver_start();

_Noreturn void *bserver_events(void *args);
broom_client_t *bserver_new_client(int fd, struct sockaddr_in *cliaddr);
broom_client_t *bserver_get_client(int sfd, int cfd);
void bserver_del_client(int fd);

ssize_t bserver_write(broom_client_t *client, char *buffer, int length);
#endif //BROOM_BSERVER_H
