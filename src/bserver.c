//
// Created by Administrator on 2023-03-04.
//
#include <broom.h>
#include <bserver.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <pthread.h>

extern broom_server_t server;

void bserver_init(const char *ip, int port)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server.sockfd = server.maxfd = sockfd;

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = inet_addr(ip);

    int ret = bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret < 0) goto failed;

    ret = listen(sockfd, BROOM_CLIENT_SIZE);
    if (ret < 0) goto failed;

    fd_set *rdset = &server.rdset;
    FD_ZERO(rdset);
    FD_SET(sockfd, rdset);

    server.bind = ip;
    server.port = port;
    return;

    failed:
    perror("failed");
    exit(1);
}

void bserver_start()
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_t events;
    pthread_create(&events, &attr, bserver_events, NULL);
    pthread_detach(events);

    int sockfd = server.sockfd;
    fd_set *rdset = &server.rdset, *tmp;
    int ret;

    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);
    int cfd;

    while (1) {
        tmp = rdset;
        ret = select(server.maxfd + 1, tmp, NULL, NULL, NULL);
        if (ret == -1) {
            break;
        } else if (ret == 0) {
            continue;
        }
        if (ret > 0) {
            if (FD_ISSET(sockfd, tmp)) {
                cfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len);
                printf("Accept Client: %d\n", cfd);
                FD_SET(cfd, rdset);
                server.maxfd = server.maxfd > cfd ? server.maxfd : cfd;
                server.clients[cfd] = bserver_new_client(cfd, &cliaddr);
            }
        }
    }
}

void *bserver_events(void *args)
{
    char buffer[BROOM_BUFFER_SIZE];
    memset(buffer, 0, BROOM_BUFFER_SIZE);
    printf("Server Started\n");

    fd_set *tmp;
    int ret;
    while (1) {
        tmp = &server.rdset;
        for (int i = 3; i <= server.maxfd; ++i) {
            if (FD_ISSET(i, tmp)) {
                ret = (int)recv(i, buffer, BROOM_BUFFER_SIZE, 0);
                if (ret == -1 && i != server.sockfd) {
                    printf("%d\n", i);
                    pthread_exit(NULL);
                } else if (ret == 0) {
                    close(i);
                    FD_CLR(i, tmp);
                    bserver_del_client(i);
                } else if (ret > 0) {
                    if (i > server.sockfd) {
                        printf("client send: %d\n", ret);
                        send(server.srcfd, buffer, ret, 0);
                        memset(buffer, 0, ret);
                    } else if (i == server.srcfd) {
                        printf("mysql send: %d\n", ret);
                        send(i, buffer, ret, 0);
                    }
                }
            }
        }
        sleep(1);
    }
}

broom_client_t *bserver_new_client(int fd, struct sockaddr_in *cliaddr)
{
    broom_client_t *client = server.clients[fd] = malloc(sizeof(broom_client_t));
    client->addr = malloc(sizeof(struct sockaddr_in));
    client->addr = cliaddr;
    server.n_clients ++;

    // module_mysql_connect
    sendfile(fd, server.memfd, NULL, server.sendsize);

    return client;
}

void bserver_del_client(int fd)
{
    broom_client_t *client = server.clients[fd];
    if (client) {
        free(client);
    }
    server.n_clients --;
}