#ifndef BLOCK_H
#define BLOCK_H
#include "malloc_defs.h"
#include <stdint.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <wchar.h>
typedef struct block_t{

    uint64_t header;
    union {
        struct{
            struct block_t *next;
            struct block_t *prev;
        } free_blocks;
        char payload[0];
    }body;

} block_t;

static inline block_t *get_next_free(block_t *block){
    return block->body.free_blocks.next;
}
static inline block_t *get_prev_free(block_t *block){
    return block->body.free_blocks.prev;
}
static inline size_t get_size(block_t *block){
    return (size_t)((block->header)&((uint64_t)BODY));
}
static inline bool is_alloc(block_t *block){
    return (size_t)((block->header)&((uint64_t)ALLOCATED));
}
static inline bool is_prev_alloc(block_t *block){
    return (size_t)((block->header)&((uint64_t)PREV_ALLOCATED));
}
static inline void set_alloc(block_t *block){
    block->header = (block->header)|(uint64_t)ALLOCATED;
}
static inline block_t *get_prev_ptr(block_t *block){ //called only if previous block is not allocated
    char *ptr = (char *)block;
    ptr-=sizeof(uint64_t); //setting the pointer at the beginning of the footer
    uint64_t *size_ptr = (uint64_t *)ptr;
    size_t size = (*size_ptr)&BODY;
    ptr=ptr-size+8; //size is size of the whole block so we have to add the 8 footer bytes back
    return (block_t *)ptr;
}

static inline block_t *get_next_ptr(block_t *block){
    char *ptr = (char *) block;
    size_t size = get_size(block);
    ptr+= size;
    return (block_t *)ptr;
}
#endif
