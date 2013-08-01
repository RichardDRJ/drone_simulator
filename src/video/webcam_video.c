#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include "video/video_server.h"
#include "video/webcam_video.h"
#include "util/error.h"

static char *filename = "/dev/video0";

void open_webcam_stream(struct input_stream *in_stream)
{
    AVInputFormat *ifmt = av_find_input_format("video4linux2");

    //open rtsp
    if ( avformat_open_input( &in_stream->ifcx, filename, ifmt, NULL) != 0 )
        error("Cannot open input file");

    if ( avformat_find_stream_info( in_stream->ifcx, NULL ) < 0 )
        error( "Cannot find stream info");

/*    int in_buffer_size = sizeof(unsigned char) * 1024;
    unsigned char *in_pb_buffer = av_malloc(in_buffer_size);

    in_stream->ifcx->pb = avio_alloc_context(in_pb_buffer, in_buffer_size, 0, NULL, NULL, NULL, NULL);*/

    //search video stream
    in_stream->video_stream_index = -1;
    int ix = 0;
    for ( ; ix < in_stream->ifcx->nb_streams; ix++ ) {
        in_stream->iccx = in_stream->ifcx->streams[ ix ]->codec;
        if ( in_stream->iccx->codec_type == AVMEDIA_TYPE_VIDEO ) {
            in_stream->ist = in_stream->ifcx->streams[ ix ];
            in_stream->video_stream_index = ix;
            break;
        }
    }
    if ( in_stream->video_stream_index < 0 )
        error("Cannot find input video stream");

    in_stream->iccx->codec = avcodec_find_decoder(in_stream->iccx->codec_id);

    if (avcodec_open2(in_stream->iccx, in_stream->iccx->codec, NULL) < 0)
        error("Could not open codec");
}

void webcam_video_init(struct data_options *d, char *file)
{
    if(file)
        filename = file;
    d->open_video_stream = open_webcam_stream;
}
