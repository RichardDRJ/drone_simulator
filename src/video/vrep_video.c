#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <inttypes.h>
#include "video/video_server.h"
#include "util/error.h"
#include "util/data_options.h"
#include "video/vrep_video.h"

static simxInt client_id;
static simxInt sensor_handle;

extern uint8_t flip_video;

extern pthread_mutex_t vrep_mutex;

void vrep_video_init(struct data_options *d, simxInt id)
{
    flip_video = 1;
    client_id = id;
    d->open_video_stream = open_vrep_stream;
    simxInt ret = simxGetIntegerSignal(id, "QCFrontSensor", &sensor_handle, simx_opmode_oneshot_wait);
    if(ret < 0)
        error("Could not get sensor handle");

    printf("front sensor handle: %d\n", sensor_handle);
}

static int read_vrep_stream(void *opaque, uint8_t *buf, int buf_size)
{
    static uint8_t init = 1;
    static char *tmp_image = NULL;
    static char *tmp_image_ptr = NULL;
    static int size_left = 0;

    simxInt resolution[2] = {0,0};
    //    simxInt opmode;
    simxChar *image;

	pthread_mutex_lock(&vrep_mutex);

    if(!size_left)
    {
        int ret;

        if(tmp_image)
        {
            free(tmp_image);
            tmp_image = NULL;
            tmp_image_ptr = NULL;
        }

        do
        {
            if(init)
            {
                ret = simxGetVisionSensorImage(client_id, sensor_handle, resolution, &image, 0, simx_opmode_oneshot_wait);
                if(ret < 0)
                    error("Could not read from vision sensor");

                ret = simxGetVisionSensorImage(client_id, sensor_handle, resolution, &image, 0, simx_opmode_streaming);
                if(ret < 0)
                    error("Could not start streaming from vision sensor");

                init = 0;
            }
            else
            {
                do
                {
                    ret = simxGetVisionSensorImage(client_id, sensor_handle, resolution, &image, 0, simx_opmode_buffer);
                }
                while(ret != simx_error_noerror);

                if(ret != simx_error_noerror)
                    error("Could not read from vision sensor");
            }

            size_left = resolution[0] * resolution[1] * 3;

        } while(resolution[0] < 0 || resolution[1] < 0);

        //        init = !(size_left == 0);

        if(!tmp_image)
        {
            tmp_image = malloc(size_left);
            tmp_image_ptr = tmp_image;
        }

        memcpy(tmp_image, image, size_left);
    }
    
    pthread_mutex_unlock(&vrep_mutex);

    int copy_size;
    if(size_left <= buf_size)
        copy_size = size_left;
    else
        copy_size = buf_size;

    memcpy(buf, tmp_image_ptr, copy_size);

    if(size_left <= buf_size)
        tmp_image_ptr = tmp_image;
    else
        tmp_image_ptr += copy_size;

    size_left -= copy_size;

    return copy_size;
}

void open_vrep_stream(struct input_stream *in_stream)
{
    //open rtsp
    in_stream->ifcx = avformat_alloc_context();

    int in_buffer_size = sizeof(unsigned char) * 1024 * 512;
    unsigned char *in_pb_buffer = av_malloc(in_buffer_size);

    in_stream->ifcx->pb = avio_alloc_context(in_pb_buffer, in_buffer_size, 0, NULL, &read_vrep_stream, NULL, NULL);
    in_stream->ifcx->flags = AVFMT_NOFILE;

    AVInputFormat *ifmt = NULL;

    AVDictionary *options = NULL;
    av_dict_set(&options, "video_size", "640x360", 0);
    av_dict_set(&options, "pixel_format", "rgb24", 0);

    //open rtsp

    in_stream->ifcx->iformat = av_find_input_format("rawvideo");

    int ret;
    if ( (ret = avformat_open_input( &in_stream->ifcx, NULL, ifmt, &options)) != 0 )
    {
        char errbuf[100];
        av_strerror(ret, errbuf, 100);
//        printf("%s\n", errbuf);
        error("Cannot open input file");
    }

    if ( avformat_find_stream_info( in_stream->ifcx, NULL ) < 0 )
        error( "Cannot find stream info");

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


    return;
}
