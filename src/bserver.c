//
// Created by Administrator on 2023-03-04.
//
#include <broom.h>
#include <bserver.h>
#include <bmodule.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/mman.h>
#include <pthread.h>
#include <fcntl.h>

extern broom_server_t server;

void bserver_init(const char *ip, int port, const char *def)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("Server Socket %d\n", sockfd);
    server.sockfd = server.maxcfd = sockfd;

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = inet_addr(ip);

    int ret = bind(sockfd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret < 0) goto failed;

    ret = listen(sockfd, BROOM_CLIENT_SIZE);
    if (ret < 0) goto failed;

    fd_set *fdset = &server.cfdset;
    FD_ZERO(fdset);
    FD_SET(sockfd, fdset);
    fdset = &server.sfdset;
    FD_ZERO(fdset);

    server.bind = ip;
    server.port = port;
    server.def = def;
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
    fd_set *cfdset = &server.cfdset, *tmp;
    int ret;

    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);
    int cfd;

    while (1) {
        tmp = cfdset;
        ret = select(server.maxcfd + 1, tmp, NULL, NULL, NULL);
        printf("select ret %d\n", ret);
        if (ret == -1) {
            break;
        } else if (ret == 0) {
            continue;
        }
        if (ret > 0) {
            if (FD_ISSET(sockfd, tmp)) {
                cfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len);
                FD_SET(cfd, cfdset);
                server.maxcfd = server.maxcfd > cfd ? server.maxcfd : cfd;
                broom_client_t *client = bserver_new_client(cfd, &cliaddr);
                if (client) {
                    printf("Accept client succeed: %d\n", cfd);
                    server.clients[cfd] = client;
                    server.n_clients ++;
                } else {
                    printf("Accept client failed: %d\n", cfd);
                }
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
        tmp = &server.cfdset;
        for (int i = server.sockfd + 1; i <= server.maxcfd; ++i) {
            if (FD_ISSET(i, tmp)) {
                ret = (int)recv(i, buffer, BROOM_BUFFER_SIZE, 0);
                if (ret == -1) {
                    printf("client recv failed: %d\n", i);
                } else if (ret == 0) {
                    printf("client: server closed %d\n", i);
                    close(i);
                    FD_CLR(i, tmp);
                    bserver_del_client(i);
                } else if (ret > 0) {
                    broom_client_t *client = bserver_get_client(0, i);
                    client->nsend = send(client->srcfd, buffer, ret, 0);
                    printf("send to server %d (%zd bytes)\n", client->clifd, client->nsend);
//                    printf("read data length: %zd\n", bserver_write(client, buffer, ret));
                }
            }
        }
        sleep(1);
    }
}

broom_client_t *bserver_new_client(int fd, struct sockaddr_in *cliaddr)
{
    broom_module_t *module = bmodule_get(server.def);
    broom_client_t *client = malloc(sizeof(broom_client_t));
    client->clifd = fd;
    client->addr = malloc(sizeof(struct sockaddr_in));
    client->addr = cliaddr;
    client->srcfd = module->connect(module);

    pthread_attr_init(&client->attr);
    int ret = pthread_create(&client->pthread, &client->attr, module->listen, client);
    if (ret != 0) {
        pthread_attr_destroy(&client->attr);
        return NULL;
    }
    pthread_detach(client->pthread);

    return client;
}

broom_client_t *bserver_get_client(int sfd, int cfd)
{
    if (cfd) {
        return server.clients[cfd];
    }

    if (sfd) {
        for (int i = server.sockfd + 1; i < BROOM_CLIENT_SIZE; ++i) {
            if (server.clients[i]->srcfd == sfd) {
                return server.clients[i];
            }
        }
    }

    return NULL;
}

void bserver_del_client(int fd)
{
    broom_client_t *client = server.clients[fd];
    if (client) {
        free(client);
    }
    server.n_clients --;
}

ssize_t bserver_write(broom_client_t *client, char *buffer, int length)
{
    // TODO: 对数据进行拦截

    memcpy(client->mem, buffer, length);
    return sendfile(client->srcfd, client->clifd, NULL, length);
}