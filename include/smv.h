#ifndef SMV_H
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <limits.h> /* for PAGESIZE */
#include <signal.h>
#include <string.h>
#include <sys/ucontext.h>
#include <ucontext.h>

#ifndef PAGESIZE
#define PAGESIZE 4096
#endif

#define NUMPAGE 100
#define MEMTOSWAPRATIO 0.5

char *init_page();

void end_page();

#endif
