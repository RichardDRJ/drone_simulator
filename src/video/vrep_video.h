#ifndef VREP_VIDEO_H
#define VREP_VIDEO_H

#include "libs/vrep/extApi.h"

void vrep_video_init(struct data_options *d, simxInt id);
void open_vrep_stream(struct input_stream *in_stream);

#endif
