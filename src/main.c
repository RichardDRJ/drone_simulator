/* User includes */
#include "util/port_numbers.h"
#include "util/server_init.h"
#include "util/error.h"
#include "util/config.h"
#include "ftp/ftp_server.h"
#include "video/video_server.h"
#include "video/vrep_video.h"
#include "video/webcam_video.h"
#include "control/control_server.h"
#include "control/vrep_control.h"
#include "navdata/navdata_server.h"
#include "navdata/vrep_navdata.h"
#include "control/print_control.h"
#include "controlcomm/controlcomm_server.h"

/* V-rep includes */
#include "libs/vrep/extApi.h"

/* Standard includes */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

static void usage(char *pname)
{
    printf("Usage: %s [options]\n"\
            "Options:\n"\
            "\t-h\t\tPrint this help text.\n"\
            "\t-v\t\tGet video stream from v-rep (requires v-rep to be running).\n"\
            "\t-w <filename>\tGet video stream from camera specified by filename. If no filename specified, defaults to /dev/video0.\n"\
            "\t-c {vrep|print}\tUse the specified method to deal with control commands.\n"\
            "\t-n {vrep}\tUse the specified source of navigation data. Right now, only v-rep is supported.\n",
            pname);
}

int main(int argc, char **argv)
{
    config_read_options();

    struct data_options data_options;

    uint8_t video_specified = 0;
    uint8_t navdata_specified = 0;
    uint8_t control_specified = 0;
    uint8_t vrep_init = 0;
    uint32_t vrep_port = 20000;
    char vrep_ip[16] = "127.0.0.1";
    simxInt vrep_client_id;

    int c;

    while ((c = getopt (argc, argv, "n:c:vw::hp:i:")) != -1)
    {
        switch (c)
        {
            case 'p':
                vrep_port = atol(optarg);
                break;
            case 'i':
                strncpy(vrep_ip, optarg, sizeof(vrep_ip));
                break;
            case 'v':
                if(video_specified)
                {
                    usage(argv[0]);
                    error("Can only have one video source");
                }

                if(!vrep_init)
                {
                    vrep_client_id = simxStart(vrep_ip, vrep_port, 1, 1, 2000, 5);
                    if(vrep_client_id == -1)
                        error("Could not connect to vrep");

                    vrep_init = 1;
                }

                vrep_video_init(&data_options, vrep_client_id);

                video_specified = 1;
                break;
            case 'h':
                usage(argv[0]);
                return 0;
            case 'n':
                if(navdata_specified)
                {
                    usage(argv[0]);
                    error("Can only have one navdata source");
                }

                if(!strcmp(optarg, "vrep"))
                {
                    vrep_navdata_init(&data_options, vrep_client_id);
                    navdata_specified = 1;
                }

                break;
            case 'w':
                if(video_specified)
                {
                    usage(argv[0]);
                    error("Can only have one video source");
                }

                webcam_video_init(&data_options, optarg);

                video_specified = 1;
                break;
            case 'c':
                if(control_specified)
                {
                    usage(argv[0]);
                    error("Can only have one control send");
                }

                if(!strcmp(optarg, "vrep"))
                {
                    if(!vrep_init)
                    {
                        vrep_client_id = simxStart(vrep_ip, vrep_port, 1, 1, 2000, 5);
                        if(vrep_client_id == -1)
                            error("Could not connect to vrep");

                        vrep_init = 1;
                    }

                    vrep_control_init(&data_options, vrep_client_id);

                    control_specified = 1;
                }
                else if(!strcmp(optarg, "print"))
                {
                    print_control_init(&data_options);
                    control_specified = 1;
                }

                break;
            case '?':
                if (optopt == 'c')
                    exit(1);
                else
                    exit(1);
            default:
                error("Could not parse arguments");;
        }
    }

    pthread_t ftp_thread;
    pthread_t video_thread;
    pthread_t control_thread;
    pthread_t controlcomm_thread;
    pthread_t navdata_thread;

    struct server_init ftp_server_init = {
        .port = FTP_LISTEN_PORT,
        .d = &data_options,
    };

    create_ftp_command_trie();
    pthread_create(&ftp_thread, NULL, ftp_listen, (void*)&ftp_server_init);

    struct server_init controlcomm_server_init = {
        .port = CONTROLCOMM_LISTEN_PORT,
        .d = &data_options,
    };

    pthread_create(&controlcomm_thread, NULL, controlcomm_listen, (void*)&controlcomm_server_init);

    if(video_specified)
    {
        struct server_init video_server_init = {
            .port = VIDEO_PORT,
            .d = &data_options,
        };

        pthread_create(&video_thread, NULL, video_listen, (void*)&video_server_init);
    }

    if(navdata_specified)
    {
        struct server_init navdata_server_init = {
            .port = NAVDATA_PORT,
            .d = &data_options,
        };

        pthread_create(&navdata_thread, NULL, navdata_listen, (void*)&navdata_server_init);
    }

    if(control_specified)
    {
        struct server_init control_server_init = {
            .port = CONTROL_PORT,
            .d = &data_options,
        };

        create_control_command_trie();
        pthread_create(&control_thread, NULL, control_listen, (void*)&control_server_init);
    }

    if(control_specified)
        pthread_join(control_thread, NULL);
    if(navdata_specified)
        pthread_join(navdata_thread, NULL);
    if(video_specified)
        pthread_join(video_thread, NULL);
    pthread_join(controlcomm_thread, NULL);
    pthread_join(ftp_thread, NULL);
}
