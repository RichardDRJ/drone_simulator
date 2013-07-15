/* User includes */
#include "util/port_numbers.h"
#include "util/error.h"
#include "ftp/ftp_server.h"
#include "video/video_server.h"

/* Standard includes */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    pthread_t ftp_thread;
    pthread_t video_thread;

    int ftp_port_arg = FTP_LISTEN_PORT;
    create_ftp_command_trie();
    pthread_create(&ftp_thread, NULL, ftp_listen, (void*)&ftp_port_arg);

    int video_port_arg = VIDEO_PORT;
    pthread_create(&video_thread, NULL, video_listen, (void*)&video_port_arg);

    pthread_join(ftp_thread, NULL);
    pthread_join(video_thread, NULL);
}
