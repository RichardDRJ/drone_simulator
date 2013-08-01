#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h> 
#include <stdarg.h> 
#include <errno.h>
#include <string.h>
#include "error.h"

void error(char *msg, ...)
{
    va_list args;
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    if(errno)
        printf(": %s\n", strerror(errno));
    else
        printf("\n");
    va_end(args);
    exit(1);
}
