#ifndef DATA_OPTIONS_H
#define DATA_OPTIONS_H

#include "control/control_server.h"
#include "video/video_server.h"
#include "navdata/navdata_common.h"
#include <inttypes.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

struct data_options
{
    void (*at_ref)(struct control_session_data*, uint8_t, uint8_t);
    void (*at_pcmd_mag)(struct control_session_data*, uint32_t, float, float, float, float, float, float);
    void (*at_pcmd)(struct control_session_data*, uint32_t, float, float, float, float);
    void (*open_video_stream)(struct input_stream *in_stream);
    void (*fill_navdata_demo)(navdata_demo_t *nd);
};

#endif
