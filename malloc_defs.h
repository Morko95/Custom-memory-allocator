#ifndef MALLOC_DEFS_H
#define MALLOC_DEFS_H
#define WSIZE 8
#define ALIGN 16
#define ALLOCATED 0x1ULL
#define PREV_ALLOCATED 0x2ULL
#define BODY ~0x3ULL
#endif
