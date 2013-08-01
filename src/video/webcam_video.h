#ifndef WEBCAM_VIDEO_H
#define WEBCAM_VIDEO_H

#include "video/video_server.h"
#include "util/data_options.h"

void open_webcam_stream(struct input_stream *in_stream);
void webcam_video_init(struct data_options *d, char *file);

#endif
