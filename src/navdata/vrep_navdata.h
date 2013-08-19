#ifndef VREP_NAVDATA_H
#define VREP_NAVDATA_H

#include "navdata/navdata_common.h"
#include "util/data_options.h"
#include "libs/vrep/extApi.h"

void vrep_navdata_init(struct data_options *d, simxInt id);

void vrep_fill_navdata_demo(navdata_demo_t *demo);

#endif
