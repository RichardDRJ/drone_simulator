#include <stdio.h>
#include "src/video/video_server.h"

int main()
{
    parrot_video_encapsulation_t *p;

    FILE *f = fopen("dronedump", "r");
    fread(p, sizeof(parrot_video_encapsulation_t), 1, f);

    fclose(f);
}
