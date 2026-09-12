#include "free_list.h"
#include "block.h"

static inline size_t get_bucket_idx(block_t *block){
    size_t size = get_size(block);
    size_t num_idx = 63-__builtin_clzll(size)-MIN_BLOCK_SHIFT;
    return size;
}

void unchain_free_block(block_t *block){
    block_t *next = get_next_free(block);
    block_t *prev = get_prev_free(block);
    if(prev == NULL){
        size_t idx = get_bucket_idx(block);
        free_list[idx] = next;
        if(next!=NULL){
            next->body.free_blocks.prev = NULL;
        }
    }
    else{
        if(next!=NULL){
        next->body.free_blocks.prev = prev;
        }
        prev->body.free_blocks.next = next;
    }
}

void move_block(block_t *block){
    size_t bucket_idx = get_bucket_idx(block);
    unchain_free_block(block);
    block_t *next_block = free_list[bucket_idx];
    free_list[bucket_idx] = block;
    block->body.free_blocks.next = next_block;
    next_block->body.free_blocks.prev = block;
}
