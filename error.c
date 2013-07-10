#include <stdio.h>
#include <stdlib.h>
#include "error.h"

void error(char *msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(1);
}
