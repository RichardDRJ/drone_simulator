/* User includes */
#include "util/error.h"

/* Video includes */
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>

/* Standard includes */
#include <unistd.h>
#include <stdio.h>
#include <time.h>

/* Networking includes */
#include <sys/socket.h>
#include <netinet/in.h>

#include <sys/stat.h> 
#include <fcntl.h>

static void send_video(int fd, int codec_id, char *filename);
static int write_packet(void *opaque, uint8_t *buf, int buf_size);

void *video_listen(void *args)
{
    av_register_all();

    int port = *(int*)args;

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

    printf("Accepted.\n");

    send_video(client_sockfd, AV_CODEC_ID_H264, "/dev/video0");

    close(client_sockfd);

    return NULL;
}

/*
 * Video encoding example
 */
void send_video_old(int fd, int codec_id, char *filename)
{
    AVFormatContext *pFormatDecodeCtx = NULL;
    AVCodecContext  *pCodecDecodeCtx = NULL;
    int             i, videoStreamIdx;
    AVCodec         *pDecodeCodec = NULL;
    AVPacket        packet;
    int             frameFinished;
    int             numBytes;
    uint8_t         *buffer;

    AVFormatContext *pFormatEncodeCtx = NULL;
    AVCodecContext  *pCodecEncodeCtx = NULL;
    AVCodec         *pEncodeCodec = NULL;
    AVFrame         *pFrame = NULL;
    AVFrame         *pFrameResize = NULL;

    static struct SwsContext *img_convert_ctx;

    // Register all formats and codecs
    av_register_all();
    avdevice_register_all();

    printf("Line %d\n", __LINE__);

    AVInputFormat *iFmt = av_find_input_format("video4linux2");

    /// Open video file
    //if(av_open_input_file(&pFormatDecodeCtx, argv[1], NULL, 0, NULL)!=0) // Deprecated
    if(avformat_open_input(&pFormatDecodeCtx, filename, iFmt, NULL) != 0)
        error("Couldn't open file");

    /// Retrieve stream information
    //if(av_find_stream_info(pFormatDecodeCtx)<0) // Deprecated
    if(avformat_find_stream_info(pFormatDecodeCtx, NULL) < 0)
        error("Couldn't find stream info");

    /// Dump information about file onto standard error
    av_dump_format(pFormatDecodeCtx, 0, filename, 0);

    /// Find the first video stream
    videoStreamIdx=-1;
    for(i=0; i<pFormatDecodeCtx->nb_streams; i++)
        if(pFormatDecodeCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) { //CODEC_TYPE_VIDEO
            videoStreamIdx=i;
            break;
        }
    if(videoStreamIdx==-1)
        error("Didn't find a video stream");

    /// Get a pointer to the codec context for the video stream
    pCodecDecodeCtx = pFormatDecodeCtx->streams[videoStreamIdx]->codec;

    printf("Line %d\n", __LINE__);

    pEncodeCodec = avcodec_find_encoder(codec_id);
    if(!pEncodeCodec)
        error("Codec not found");

    pCodecEncodeCtx = avcodec_alloc_context3(pEncodeCodec);
    if(!pCodecEncodeCtx)
        error("Could not allocate context");

    printf("Line %d\n", __LINE__);

    pCodecEncodeCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    pCodecEncodeCtx->width = 640;
    pCodecEncodeCtx->height = 360;
    pCodecEncodeCtx->time_base = pCodecDecodeCtx->time_base;
    pCodecEncodeCtx->gop_size = 25;
    pCodecEncodeCtx->max_b_frames = 1;
    pCodecEncodeCtx->bit_rate = 400000;

    if (avcodec_open2(pCodecEncodeCtx, pEncodeCodec, NULL) < 0)
        error("Could not open codec");

    printf("Line %d\n", __LINE__);
    /// Find the decoder for the video stream
    pDecodeCodec = avcodec_find_decoder( pCodecDecodeCtx->codec_id);
    if(pDecodeCodec==NULL) {
        fprintf(stderr, "Unsupported codec!\n");
        error("Codec not found");
    }
    /// Open codec
    //if( avcodec_open(pCodecDecodeCtx, pDecodeCodec) < 0 ) -- Deprecated
    if( avcodec_open2(pCodecDecodeCtx, pDecodeCodec, NULL) < 0 )
        error("Could not open codec");

    printf("Line %d\n", __LINE__);
    /// Allocate video frame
    pFrame = avcodec_alloc_frame();

    /// Allocate an AVFrame structure
    pFrameResize = avcodec_alloc_frame();
    if(pFrameResize==NULL)
        error("Could not allocate memory");
    printf("Line %d\n", __LINE__);

    /// Determine required buffer size and allocate buffer
    numBytes = avpicture_get_size(pCodecEncodeCtx->pix_fmt,
            pCodecEncodeCtx->width,
            pCodecEncodeCtx->height);

    buffer = (uint8_t *) av_malloc(numBytes*sizeof(uint8_t));
    printf("Line %d\n", __LINE__);

    /// Assign appropriate parts of buffer to image planes in pFrameResize
    // Note that pFrameResize is an AVFrame, but AVFrame is a superset
    // of AVPicture
    avpicture_fill((AVPicture *)pFrameResize, buffer, pCodecEncodeCtx->pix_fmt,
            pCodecEncodeCtx->width, pCodecEncodeCtx->height);

    int w = pCodecDecodeCtx->width;
    int h = pCodecDecodeCtx->height;
    img_convert_ctx = sws_getContext(w, h, pCodecDecodeCtx->pix_fmt,
            pCodecEncodeCtx->width, pCodecEncodeCtx->height, pCodecEncodeCtx->pix_fmt,
            SWS_BICUBIC, NULL, NULL, NULL);

    printf("Line %d\n", __LINE__);

    int64_t current_pts = 0;

    /* Allocate the output media context. */
    pFormatEncodeCtx = avformat_alloc_context();
    if (!pFormatEncodeCtx) {
        error("Memory error\n");
    }

    AVOutputFormat *fmt;
    fmt = av_guess_format("h264", NULL, NULL);
    if (!fmt) {
        error("Could not find suitable output format\n");
    }
    pFormatEncodeCtx->oformat = fmt;

    int buffer_size = sizeof(unsigned char) * 4 * 1024 * 1024;
    unsigned char *pb_buffer = av_malloc(buffer_size);

    pFormatEncodeCtx->pb = avio_alloc_context(pb_buffer, buffer_size, 1, &fd, NULL, &write_packet, NULL);

    AVStream *video_st = avformat_new_stream(pFormatEncodeCtx, pEncodeCodec);
    if(!video_st)
        error("Could not open output stream");

    //avformat_write_header(pFormatEncodeCtx, NULL);
    //
    //    fd = fopen("out.raw"

    while((av_read_frame(pFormatDecodeCtx, &packet)>=0)) {
        // Is this a packet from the video stream?
        if(packet.stream_index==videoStreamIdx)
        {
            /// Decode video frame
            avcodec_decode_video2(pCodecDecodeCtx, pFrame, &frameFinished, &packet);

            printf("Post-decode packet size: %d\n", packet.size);

            // Did we get a video frame?
            if(frameFinished)
            {
                write(fd, packet.data, packet.size);
                printf("Line %d\n", __LINE__);

                printf("sws_scale: %d\n", sws_scale(img_convert_ctx, (const uint8_t * const *)pFrame->data,
                            pFrame->linesize, 0, pCodecDecodeCtx->height,
                            pFrameResize->data, pFrameResize->linesize));

                pFrameResize->width = pCodecEncodeCtx->width;
                pFrameResize->height = pCodecEncodeCtx->height;

                printf("Line %d\n", __LINE__);

                av_free_packet(&packet);
                av_init_packet(&packet);

                packet.data = NULL;
                pFrameResize->pts = current_pts++;
                printf("pFrameResize->width: %d\npFrameResize->height: %d\n", pFrameResize->width, pFrameResize->height);

                int got_output = 1;
                int ret = avcodec_encode_video2(pCodecEncodeCtx, &packet, pFrameResize, &got_output);
                if (ret < 0)
                    error("Encoding failed");
                printf("Line %d\n", __LINE__);

                printf("Post-encode packet size: %d\n", packet.size);
                printf("Post-encode packet data pointer: %p\n", packet.data);

                if(got_output)
                {
                    printf("Line %d\n", __LINE__);
                    //av_write_frame(pFormatEncodeCtx, &packet);
                    //write(fd, packet.data, packet.size);
                    printf("Line %d\n", __LINE__);
                }
                printf("Line %d\n", __LINE__);
            }
        }

        // Free the packet that was allocated by av_read_frame
        av_free_packet(&packet);
    }

    // Free the RGB image
    av_free(buffer);
    av_free(pFrameResize);

    // Free the YUV frame
    av_free(pFrame);

    // Close the codec
    avcodec_close(pCodecDecodeCtx);

    // Close the video file
    avformat_close_input(&pFormatDecodeCtx);
}

static int write_packet(void *opaque, uint8_t *buf, int buf_size)
{
    printf("HERE\n");
    int net_fd = *(int*)opaque;

    write(net_fd, buf, buf_size);

    return 0;
}


static void send_video(int fd, int codec_id, char *filename)
{
    AVStream *ist = NULL;
    AVPacket pkt;

    // Initialize library
    av_log_set_level( AV_LOG_DEBUG );
    av_register_all();
    avcodec_register_all(); 
    avdevice_register_all();

    //
    // Input
    //

    AVInputFormat *ifmt = av_find_input_format("video4linux2");

    AVFormatContext *ifcx = NULL;
    AVCodecContext *iccx = NULL;
    //open rtsp
    if ( avformat_open_input( &ifcx, filename, ifmt, NULL) != 0 )
        error("Cannot open input file");

    if ( avformat_find_stream_info( ifcx, NULL ) < 0 )
        error( "Cannot find stream info");

    //search video stream
    int i_index = -1;
    int ix = 0;
    for ( ; ix < ifcx->nb_streams; ix++ ) {
        iccx = ifcx->streams[ ix ]->codec;
        if ( iccx->codec_type == AVMEDIA_TYPE_VIDEO ) {
            ist = ifcx->streams[ ix ];
            i_index = ix;
            break;
        }
    }
    if ( i_index < 0 )
        error("Cannot find input video stream");

    //
    // Output
    //

    //open output file
    AVOutputFormat *ofmt = av_guess_format( "avi", NULL, NULL );
    AVFormatContext *ofcx = avformat_alloc_context();
    ofcx->oformat = ofmt;

    // Create output stream
    //ost = avformat_new_stream( ofcx, (AVCodec *) iccx->codec );
    AVStream *ost = avformat_new_stream( ofcx, NULL );
    avcodec_copy_context( ost->codec, iccx );

    ost->sample_aspect_ratio.num = iccx->sample_aspect_ratio.num;
    ost->sample_aspect_ratio.den = iccx->sample_aspect_ratio.den;

    // Assume r_frame_rate is accurate
    ost->r_frame_rate = ist->r_frame_rate;
    ost->avg_frame_rate = ost->r_frame_rate;
    ost->time_base = av_inv_q( ost->r_frame_rate );
    ost->codec->time_base = ost->time_base;

    int buffer_size = sizeof(unsigned char) * 4 * 1024 * 1024;
    unsigned char *pb_buffer = av_malloc(buffer_size);

    ofcx->pb = avio_alloc_context(pb_buffer, buffer_size, 1, &fd, NULL, &write_packet, NULL);

    avformat_write_header( ofcx, NULL );

    //start reading packets from stream and write them to file

    av_dump_format( ifcx, 0, ifcx->filename, 0 );
    av_dump_format( ofcx, 0, ofcx->filename, 1 );

    time_t timenow = time(0);
    time_t timestart = timenow;
    time_t totaltime = 10;

    ix = 0;

    av_init_packet( &pkt );
    while ( av_read_frame( ifcx, &pkt ) >= 0 && timenow - timestart < totaltime) {
        if ( pkt.stream_index == i_index ) { //packet is video               
            // Make sure we start on a key frame
            if ( timestart == timenow && ! ( pkt.flags & AV_PKT_FLAG_KEY ) ) {
                timestart = timenow = time(0);
                continue;
            }

            pkt.stream_index = ost->id;

            pkt.pts = ix++;
            pkt.dts = pkt.pts;

            printf("Line %d\n", __LINE__);

            av_write_frame( ofcx, &pkt );
        }
        av_free_packet( &pkt );
        av_init_packet( &pkt );

        timenow = time(0);
    }

    printf("Line %d\n", __LINE__);
    av_read_pause( ifcx );
    printf("Line %d\n", __LINE__);
    av_write_trailer( ofcx );
    printf("Line %d\n", __LINE__);
    avformat_free_context( ofcx );
    printf("Line %d\n", __LINE__);
}
