#include <broom.h>
#include <bserver.h>
#include <bsource.h>

broom_server_t server;

int main() {

    bsource_init();

    char *ip = "127.0.0.1";
    int port = 8899;

//    bserver_init(ip, port);

    return 0;
}
