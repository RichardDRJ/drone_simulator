/* User includes */
#include "util/port_numbers.h"
#include "util/server_init.h"
#include "util/error.h"
#include "ftp/ftp_server.h"
#include "video/video_server.h"
#include "video/vrep_video.h"
#include "video/webcam_video.h"
#include "control/control_server.h"
#include "control/vrep_control.h"
#include "navdata/navdata_server.h"

/* V-rep includes */
#include "libs/vrep/extApi.h"

/* Standard includes */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    struct data_options data_options;

    simxInt ret = simxStart("127.0.0.1", 20000, 1, 1, 2000, 5);
    if(ret == -1)
        error("Could not connect to vrep");

    vrep_control_init(&data_options, ret);
    vrep_video_init(&data_options, ret);
//    webcam_video_init(&data_options);

    pthread_t ftp_thread;
    pthread_t video_thread;
//    pthread_t control_thread;
    pthread_t navdata_thread;

    struct server_init ftp_server_init = {
        .port = FTP_LISTEN_PORT,
        .d = &data_options,
    };

    create_ftp_command_trie();
    pthread_create(&ftp_thread, NULL, ftp_listen, (void*)&ftp_server_init);

    struct server_init video_server_init = {
        .port = VIDEO_PORT,
        .d = &data_options,
    };

    pthread_create(&video_thread, NULL, video_listen, (void*)&video_server_init);

    struct server_init navdata_server_init = {
        .port = NAVDATA_PORT,
        .d = &data_options,
    };

    pthread_create(&navdata_thread, NULL, navdata_listen, (void*)&navdata_server_init);
/*
    struct server_init control_server_init = {
        .port = CONTROL_PORT,
        .d = &data_options,
    };*/

    create_control_command_trie();
//    pthread_create(&control_thread, NULL, control_listen, (void*)&control_server_init);

    pthread_join(navdata_thread, NULL);
//    pthread_join(control_thread, NULL);
    pthread_join(video_thread, NULL);
    pthread_join(ftp_thread, NULL);
}
