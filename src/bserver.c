//
// Created by Administrator on 2023-03-04.
//
#include <broom.h>
#include <bserver.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

extern broom_server_t server;

void bserver_init(char *ip, int port)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server.sockfd = sockfd;
    server.maxfd = sockfd + 1;

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = inet_addr(ip);

    int ret = bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret < 0) goto failed;

    ret = listen(sockfd, 1024);
    if (ret < 0) goto failed;

    fd_set *rdset = &server.rdset;
    FD_ZERO(rdset);
    FD_SET(sockfd, rdset);
    server.maxfd = sockfd;

    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);
    int cfd;

    bserver_events();

    while (1) {
        ret = select(server.maxfd + 1, &server.tmp, NULL, NULL, NULL);
        if (ret == -1) {
            goto failed;
        } else if (ret == 0) {
            continue;
        }
        if (ret > 0) {
            if (FD_ISSET(sockfd, &server.tmp)) {
                cfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len);
                FD_SET(cfd, rdset);
                server.maxfd = server.maxfd > cfd ? server.maxfd : cfd;
                bserver_new_client(cfd, &cliaddr);
            }
        }
    }
    return;

    failed:
    perror("failed");
}

void bserver_events()
{
    fd_set *rdset = &server.rdset;
    for (int i = server.sockfd + 1; i < server.maxfd; ++i) {

    }
}

broom_client_t *bserver_new_client(int fd, struct sockaddr_in *cliaddr)
{
    broom_client_t *client = server.clients[fd];
    client->addr = malloc(sizeof(struct sockaddr_in));
    client->addr = cliaddr;

    return client;
}