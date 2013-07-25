/* User includes */
#include "util/port_numbers.h"
#include "util/error.h"
#include "ftp/ftp_server.h"
#include "video/video_server.h"
#include "control/control_server.h"
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
/*    simxInt ret = simxStart("127.0.0.1", 20000, 1, 0, 1000, 5);
    if(ret == -1)
        error("Could not connect to vrep");*/

    pthread_t ftp_thread;
//    pthread_t video_thread;
    pthread_t control_thread;
    pthread_t navdata_thread;

    int ftp_port_arg = FTP_LISTEN_PORT;
    create_ftp_command_trie();
    pthread_create(&ftp_thread, NULL, ftp_listen, (void*)&ftp_port_arg);

/*    int video_port_arg = VIDEO_PORT;
    pthread_create(&video_thread, NULL, video_listen, (void*)&video_port_arg);*/

    int navdata_port_arg = NAVDATA_PORT;
    pthread_create(&navdata_thread, NULL, navdata_listen, (void*)&navdata_port_arg);

    int control_port_arg = CONTROL_PORT;
    create_control_command_trie();
    pthread_create(&control_thread, NULL, control_listen, (void*)&control_port_arg);

    pthread_join(navdata_thread, NULL);
    pthread_join(control_thread, NULL);
//    pthread_join(video_thread, NULL);
    pthread_join(ftp_thread, NULL);
}
