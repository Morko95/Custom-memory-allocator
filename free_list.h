#ifndef FREE_LIST_H
#define FREE_LIST_H
#include "block.h"
#include <stdint.h>
enum {
    MIN_BLOCK_SHIFT = 5, //helps calculate the bucket index, as minimum block size is 32 bytes, which equals 2^5
    NUM_BUCKETS = 12,
    MIN_BLOCK_SIZE = 32
};
extern block_t *free_list[NUM_BUCKETS];

void unchain_free_block(block_t *block);
void move_block(block_t *block);
block_t *coalesce(block_t *curr);
block_t *split_block(block_t *block);
block_t *find_fit(block_t *block);
#endif
