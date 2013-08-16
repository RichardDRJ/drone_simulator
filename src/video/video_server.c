/* User includes */
#include "util/error.h"
#include "util/data_options.h"
#include "util/server_init.h"
#include "video/video_server.h"
#include "video/webcam_video.h"

/* Video includes */
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libavutil/opt.h>

/* Standard includes */
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/stat.h> 
#include <fcntl.h>

/* Networking includes */
#include <sys/socket.h>
#include <netinet/in.h>

uint8_t flip_video = 0;

static void send_video(int fd, int codec_id, struct data_options *dopts);
static int write_packet(void *opaque, uint8_t *buf, int buf_size);

void *video_listen(void *args)
{
    av_register_all();
    av_log_set_level(AV_LOG_QUIET);

    struct server_init *server_init = (struct server_init*)args;
    int port = server_init->port;

    struct sockaddr_in serv_addr;
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);

    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);

    if (bind(server_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");

    listen(server_sockfd, 5);

    int client_sockfd = accept(server_sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if(client_sockfd < 0)
        error("ERROR on accept");

    send_video(client_sockfd, AV_CODEC_ID_H264, server_init->d);

    close(client_sockfd);

    return NULL;
}

parrot_video_encapsulation_t *create_frame_header(uint32_t payload_size, AVFrame *frame, uint32_t frame_number, uint64_t stream_byte_position, uint8_t sps, uint8_t pps, uint8_t pkt_flags)
{
    parrot_video_encapsulation_t *new_header = calloc(sizeof(parrot_video_encapsulation_t), 1);

    new_header->signature[0] = 'P';
    new_header->signature[1] = 'a';
    new_header->signature[2] = 'V';
    new_header->signature[3] = 'E';

    new_header->version = 2;

    new_header->video_codec = 4;    // This is h.264. Need to examine the possibilities for this field.

    new_header->header_size = sizeof(parrot_video_encapsulation_t);
    new_header->payload_size = payload_size;

    new_header->encoded_stream_width = frame->width;
    new_header->display_width = frame->width;

    new_header->encoded_stream_height = frame->height;
    new_header->display_height = frame->height;

    new_header->frame_number = frame_number;

    new_header->timestamp = frame->pts;

    new_header->frame_type = frame->pict_type == AV_PICTURE_TYPE_P ? FRAME_TYPE_P_FRAME : FRAME_TYPE_I_FRAME;
    new_header->frame_type = pkt_flags & AV_PKT_FLAG_KEY ? FRAME_TYPE_I_FRAME : FRAME_TYPE_P_FRAME;
    printf("frame->type: %s\n", new_header->frame_type == FRAME_TYPE_P_FRAME ? "FRAME_TYPE_P_FRAME" : "FRAME_TYPE_I_FRAME");

    new_header->control = 0;

    new_header->stream_byte_position_lw = (uint32_t)(stream_byte_position & 0xFFFFFFFF);
    new_header->stream_byte_position_uw = (uint32_t)((stream_byte_position >> 32) & 0xFFFFFFFF);

    new_header->stream_id = 1;

    new_header->total_slices = 1;
    new_header->slice_index = 0;

    new_header->header1_size = sps;
    new_header->header2_size = pps;

    new_header->advertised_size = payload_size;

    return new_header;
}

static int write_packet(void *opaque, uint8_t *buf, int buf_size)
{
    int net_fd = *(int*)opaque;

    write(net_fd, buf, buf_size);

    return 0;
}

static AVStream *setup_output_context(int fd, AVFormatContext *ofcx, AVCodecContext *iccx, AVStream *ist);

void flip_frame(AVFrame* pFrame) { 
    for (int i = 0; i < 4; i++) { 
        pFrame->data[i] += pFrame->linesize[i] * (pFrame->height-1); 
        pFrame->linesize[i] = -pFrame->linesize[i]; 
    } 
} 

static void send_video(int fd, int codec_id, struct data_options *dopts)
{
    AVPacket pkt;

    // Initialize library
    av_log_set_level( AV_LOG_DEBUG );
    av_register_all();
    avcodec_register_all(); 
    avdevice_register_all();

    //
    // Input
    //

    struct input_stream in_st = 
    {
        .ist = NULL,
        .ifcx = NULL,
        .iccx = NULL,
    };

    dopts->open_video_stream(&in_st);

    //
    // Output
    //

    //open output file
    AVFormatContext *ofcx = avformat_alloc_context();

    AVCodec *out_codec = avcodec_find_encoder(codec_id);
    if(!out_codec)
        error("Codec not found");

    AVCodecContext *occx = avcodec_alloc_context3(out_codec);
    if(!occx)
        error("Could not allocate context");

    occx->pix_fmt = AV_PIX_FMT_YUV420P;
    occx->width = 640;
    occx->height = 360;
    occx->time_base= (AVRational){1,30};
    occx->gop_size = 30;
    occx->max_b_frames = 0;
    occx->bit_rate = 400000;

    //av_opt_set(occx->priv_data, "preset", "ultrafast", 0);
    av_opt_set(occx->priv_data, "tune", "zerolatency", 0);
    av_opt_set(occx->priv_data, "vprofile", "baseline", 0);

    if (avcodec_open2(occx, out_codec, NULL) < 0)
        error("Could not open codec");

    setup_output_context(fd, ofcx, occx, in_st.ist);

    int buffer_size = sizeof(unsigned char) * 1024 * 1024;
    unsigned char *pb_buffer = av_malloc(buffer_size);

    ofcx->pb = avio_alloc_context(pb_buffer, buffer_size, 1, &fd, NULL, &write_packet, NULL);

    avformat_write_header( ofcx, NULL );

    //start reading packets from stream and write them to file

    int ix = 0;

    AVFrame *frame;
    AVFrame *rFrame = avcodec_alloc_frame();

    /*int num_bytes = avpicture_get_size(occx->pix_fmt, occx->width, occx->height);
      uint8_t* rFrame_buffer = av_malloc(num_bytes*sizeof(uint8_t));
      avpicture_fill((AVPicture*)rFrame, rFrame_buffer, occx->pix_fmt, occx->width, occx->height);*/

    int w = in_st.iccx->width;
    int h = in_st.iccx->height;

    int num_bytes = avpicture_get_size(occx->pix_fmt, w, h);
    uint8_t* rFrame_buffer = av_malloc(num_bytes*sizeof(uint8_t));
    avpicture_fill((AVPicture*)rFrame, rFrame_buffer, occx->pix_fmt, w, h);

    struct SwsContext *img_convert_ctx;

    av_init_packet( &pkt );
    while ( av_read_frame( in_st.ifcx, &pkt ) >= 0) {
        if ( pkt.stream_index == in_st.video_stream_index ) { //packet is video 
            int got_picture;

            frame = avcodec_alloc_frame();

            avcodec_decode_video2(in_st.iccx, frame, &got_picture, &pkt);

            if(got_picture)
            {
                frame->pts = ix;

                img_convert_ctx = sws_getContext(w, h, frame->format, occx->width, occx->height, occx->pix_fmt, SWS_BILINEAR, NULL, NULL, NULL);
                if(img_convert_ctx == NULL)
                    error("Cannot initialize the conversion context");

                if(flip_video)
                    flip_frame(frame);

                sws_scale(img_convert_ctx, (const uint8_t* const*)frame->data, frame->linesize, 0, in_st.iccx->height, rFrame->data, rFrame->linesize);

                sws_freeContext(img_convert_ctx);

                av_free_packet( &pkt );
                av_init_packet(&pkt);
                pkt.data = NULL;

                int ret = avcodec_encode_video2(occx, &pkt, rFrame, &got_picture);
                if(ret < 0)
                    error("Encoding failed");

                if(got_picture)
                {
                    parrot_video_encapsulation_t *p = create_frame_header(pkt.size, frame, ix, pkt.pos, 0, 0, pkt.flags);
                    write(fd, p, sizeof(parrot_video_encapsulation_t));
                    av_write_frame( ofcx, &pkt );
                    free(p);
                }
            }
            
            avcodec_free_frame(&frame);

            ++ix;
        }
        av_free_packet( &pkt );
        av_init_packet( &pkt );
    }

    av_read_pause( in_st.ifcx );
    av_write_trailer( ofcx );

    avcodec_close( occx );

    for (int i = 0; i < ofcx->nb_streams; i++) {
        avcodec_close(ofcx->streams[i]->codec);
        av_freep(&ofcx->streams[i]);
    }

    avformat_free_context( ofcx );
}

static AVStream *setup_output_context(int fd, AVFormatContext *ofcx, AVCodecContext *occx, AVStream *ist)
{
    AVOutputFormat *ofmt = av_guess_format( "h264", NULL, NULL );
    ofcx->oformat = ofmt;

    // Create output stream
    AVStream *ost = avformat_new_stream( ofcx, NULL );
    ost->codec = occx;

    ost->sample_aspect_ratio.num = occx->sample_aspect_ratio.num;
    ost->sample_aspect_ratio.den = occx->sample_aspect_ratio.den;

    // Assume r_frame_rate is accurate
    ost->r_frame_rate = (AVRational){30,1};
    ost->avg_frame_rate = ost->r_frame_rate;
    ost->time_base = av_inv_q( ost->r_frame_rate );
    ost->codec->time_base = ost->time_base;

    return ost;
}
