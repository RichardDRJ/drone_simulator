#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "error.h"

void error(char *msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(1);
}
