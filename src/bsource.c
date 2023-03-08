//
// Created by Administrator on 2023-03-04.
//
#include <bsource.h>
#include <stdio.h>
#include <iniparser.h>
#include <bmodule.h>

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
