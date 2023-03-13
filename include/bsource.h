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

void bsource_start();

ssize_t bsource_write(broom_client_t *client, char *buffer, int length);

#endif //BROOM_BSOURCE_H
