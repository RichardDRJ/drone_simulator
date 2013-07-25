#ifndef NAVDATA_SERVER_H
#define NAVDATA_SERVER_H

#include <pthread.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>

void *navdata_listen(void*);

#endif
