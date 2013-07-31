#ifndef SERVER_INIT_H
#define SERVER_INIT_H

#include "util/data_options.h"
#include <inttypes.h>

struct server_init
{
    uint32_t port;
    struct data_options *d;
};

#endif
