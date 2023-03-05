//
// Created by Administrator on 2023-03-04.
//

#ifndef BROOM_BSERVER_H
#define BROOM_BSERVER_H

#include <broom.h>

void bserver_init(char *ip, int port);
void bserver_events();
broom_client_t *bserver_new_client(int fd, struct sockaddr_in *cliaddr);

#endif //BROOM_BSERVER_H
