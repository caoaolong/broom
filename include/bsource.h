//
// Created by Administrator on 2023-03-04.
//

#ifndef BROOM_BSOURCE_H
#define BROOM_BSOURCE_H

#include <broom.h>

typedef struct broom_source_s broom_source_t;
struct broom_source_s {
    u32 host;
    u16 port;
    broom_source_t *next;
};

void bsource_init();

#endif //BROOM_BSOURCE_H
