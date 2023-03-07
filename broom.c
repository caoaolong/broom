#include <broom.h>
#include <bserver.h>
#include <bsource.h>
#include <iniparser.h>

broom_server_t server;

int main() {

    broom_init();

    bsource_init();

    const char *ip = iniparser_getstring(server.dict, "server:bind", "127.0.0.1");
    int port = iniparser_getint(server.dict, "server:port", 8899);
    bserver_init(ip, port);

    bserver_start();
    return 0;
}
