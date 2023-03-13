//
// Created by Administrator on 2023-03-04.
//
#include <bsource.h>
#include <stdio.h>
#include <iniparser.h>
#include <pthread.h>
#include <bserver.h>
#include <bmodule.h>
#include <sys/sendfile.h>
#include <module/bmodule_mysql.h>

extern broom_module_t modules;
extern broom_server_t server;

void bsource_init()
{
    char path[BROOM_PATH_SIZE];
    sprintf(path, "%s/%s", BROOM_PREFIX, BROOM_CONF_PATH);

    dictionary *dict = iniparser_load(path);
    if (!dict) {
        perror("load failed");
        exit(1);
    }
    server.dict = dict;

    broom_module_t *module;
    int nsec = iniparser_getnsec(dict);
    for (int i = 0; i < nsec; ++i) {
        const char *name = iniparser_getsecname(dict, i);
        if (!strcasecmp(name, "server"))
            continue;

        module = bmodule_get(name);
        if (!module) {
            fprintf(stderr, "unknown module name: %s\n", name);
            exit(1);
        }
        char key[64];
        sprintf(key, "%s:host", name);
        module->host = iniparser_getstring(dict, key, "127.0.0.1");
        sprintf(key, "%s:port", name);
        module->port = iniparser_getint(dict, key, 0);
        module->init_func(module);
    }
}

_Noreturn void *bsource_listen(void *args)
{
    char buffer[BROOM_BUFFER_SIZE];
    memset(buffer, 0, BROOM_BUFFER_SIZE);

    fd_set *tmp;
    int ret;
    while (1) {
        tmp = &server.sfdset;
        for (int i = server.sockfd + 1; i <= server.maxsfd; ++i) {
            if (FD_ISSET(i, tmp)) {
                ret = (int)recv(i, buffer, BROOM_BUFFER_SIZE, 0);
                if (ret == -1 && i != server.sockfd) {
                    printf("source recv failed: %d\n", i);
                } else if (ret == 0) {
                    close(i);
                    FD_CLR(i, tmp);
                } else if (ret > 0) {
                    // 接收到服务器的数据，发送给客户端
                    broom_client_t *client = bserver_get_client(i, 0);
                    printf("read data length: %zd\n", bsource_write(client, buffer, ret));
                }
            }
        }
        sleep(1);
    }
}

void bsource_start()
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_t pthread;
    pthread_create(&pthread, &attr, bsource_listen, NULL);
    pthread_detach(pthread);
}

ssize_t bsource_write(broom_client_t *client, char *buffer, int length)
{
    // TODO: 对数据进行拦截
    // greeting
//    mysql_protocol_t *pro = (mysql_protocol_t *)buffer;
//    printf("Length: %d\n\n", length);
//    if ((int)pro->packet.number == 0) {
//        char *version = buffer + sizeof(mysql_protocol_t);
//        printf("Version: %s\n", version);
//        mysql_greeting_t *greeting = (mysql_greeting_t *)(version + strlen(version) + 1);
//        printf("Thread ID: %d\n", greeting->thread_id);
//        printf("Salt Start: %s\n", greeting->salt_start);
//        printf("Server Caps: %2X\n", greeting->server_caps);
//        printf("Server Language: %d\n", greeting->server_language);
//        printf("Server Status: %d\n", greeting->server_status);
//        printf("Server Ext Caps: %d\n", greeting->server_ext_caps);
//        printf("Salt End: %s\n", greeting->salt_end);
//        printf("Auth Plugin Length: %d\n", greeting->auth_plugin_length);
//    }

    memcpy(client->mem, buffer, length);
    client->nsend = length;
    return sendfile(client->clifd, client->memfd, NULL, client->nsend);
}