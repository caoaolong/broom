//
// Created by Administrator on 2023-03-05.
//

#ifndef BROOM_BMODULE_MYSQL_H
#define BROOM_BMODULE_MYSQL_H

#include <broom.h>

typedef struct mysql_protocol_t {
    union {
        u32 packet_v;
        struct {
            u32 length:24;
            u32 number:8;
        } packet;
    };
    u8 protocol;
} mysql_protocol_t;

typedef struct mysql_greeting_t {
    u32 thread_id;
    u8 salt_start[9];
    u16 server_caps;
    u8 server_language;
    u16 server_status;
    u16 server_ext_caps;
    u8 auth_plugin_length;
    u8 unused[10];
    u8 salt_end[13];
} mysql_greeting_t;

#endif //BROOM_BMODULE_MYSQL_H
