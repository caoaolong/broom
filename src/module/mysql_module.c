//
// Created by Administrator on 2023-03-04.
//

#include <bmodule.h>
#include <module/bmodule_mysql.h>
#include <arpa/inet.h>

void module_mysql_init(dictionary *dict, broom_module_t *module)
{
    const char *host = iniparser_getstring(dict, "mysql:host", "127.0.0.1");
    int port = iniparser_getint(dict, "mysql:port", 3306);

    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = inet_addr(host);

    int ret = connect(fd, (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret < 0) {
        fprintf(stderr, "connect failed: %s:%d", host, port);
        exit(1);
    }

    char buffer[BROOM_BUFFER_SIZE];
    ssize_t pktsize = recv(fd, buffer, sizeof(buffer), 0);
    if (pktsize < 0) {
        fprintf(stderr, "recv failed: %s:%d", host, port);
        exit(1);
    }

    // greeting
    mysql_protocol_t *pro = (mysql_protocol_t *)buffer;
    u32 length = pro->packet.length;
    printf("Length: %d\n\n", length);
    if ((int)pro->packet.number == 0) {
        char *version = buffer + sizeof(mysql_protocol_t);
        printf("Version: %s\n", version);
        mysql_greeting_t *greeting = (mysql_greeting_t *)(version + strlen(version) + 1);
        printf("Thread ID: %d\n", greeting->thread_id);
        printf("Salt Start: %s\n", greeting->salt_start);
        printf("Server Caps: %2X\n", greeting->server_caps);
        printf("Server Language: %d\n", greeting->server_language);
        printf("Server Status: %d\n", greeting->server_status);
        printf("Server Ext Caps: %d\n", greeting->server_ext_caps);
        printf("Salt End: %s\n", greeting->salt_end);
        printf("Auth Plugin Length: %d\n", greeting->auth_plugin_length);
    }
}