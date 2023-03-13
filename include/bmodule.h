//
// Created by Administrator on 2023-03-04.
//

#ifndef BROOM_BMODULE_H
#define BROOM_BMODULE_H

#include <broom.h>
#include <iniparser.h>

typedef struct broom_module_s broom_module_t;
struct broom_module_s {
    const char       *name;
    const char       *host;
    int               port;

    void (*init_func)(broom_module_t *module);
    int (*connect)(broom_module_t *module);
};

broom_module_t *bmodule_get(const char *name);

void module_mysql_init(broom_module_t *module);

int module_mysql_connect(broom_module_t *module);

#endif //BROOM_BMODULE_H
