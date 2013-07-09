#ifndef __H_COMMON__
#define __H_COMMON__

#include <stdio.h>
#include <stdlib.h>
#define DEBUG 1

#define debug_printf(fmt, ...) \
        do { if (DEBUG) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
        __LINE__, __func__, __VA_ARGS__); } while (0)


#endif
