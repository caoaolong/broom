//
// Created by Administrator on 2023-03-04.
//

#include <bmodule.h>
#include <string.h>

broom_module_t modules[] = {
        {"mysql", 0, 0, module_mysql_init},
        {NULL, 0, 0, NULL}
};

broom_module_t *bmodule_get(const char *name)
{
    broom_module_t *module = &modules[0];
    while (module->name) {
        if (!strcasecmp(module->name, name)) {
            return module;
        }
        module ++;
    }
    return NULL;
}
