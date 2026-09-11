#ifndef BLOCK_H
#define BLOCK_H
#include <cstdint>
#include <stddef.h>
#include <stdint.h>


typedef struct block_t{
    uint64_t header;
    union {
        struct{
            block_t *next;
            block_t *prev;
        } free_blocks;
        char payload[0];
    }body;

} block_t;

void delete_block(block_t *block);
block_t *check_prev(block_t *block);
block_t *check_next(block_t *block);
block_t *split_block(block_t *block);
block_t *move_block(block_t *block);
block_t *merge_block(block_t *block);
block_t *coalesce(block_t *curr);
inline static size_t get_size(block_t *block);
inline static bool is_alloc(block_t *block);
inline static void set_alloc(block_t *block);
inline static block_t *get_prev(block_t *block);
inline static block_t *get_next(block_t *block);

#endif
