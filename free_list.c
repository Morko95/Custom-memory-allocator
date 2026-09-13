#include "free_list.h"
#include "block.h"
#include "malloc_defs.h"
#include <stdint.h>

static inline size_t get_bucket_idx(size_t size){
    size_t num_idx = 63-__builtin_clzll(size)-MIN_BLOCK_SHIFT;
    if (num_idx>NUM_BUCKETS-1){
        return NUM_BUCKETS-1;
    }
    return num_idx;
}

void unchain_free_block(block_t *block){
    block_t *next = get_next_free(block);
    block_t *prev = get_prev_free(block);
    if(prev == NULL){
        size_t idx = get_bucket_idx(get_size(block));
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
    size_t bucket_idx = get_bucket_idx(get_size(block));
    unchain_free_block(block);
    block_t *next_block = free_list[bucket_idx];
    free_list[bucket_idx] = block;
    block->body.free_blocks.next = next_block;
    next_block->body.free_blocks.prev = block;
}
void move_freed_block(block_t *block){ //cant use move_block, as it operates on the free list and freed block still hasnt got these pointers initiated
    size_t bucket_idx = get_bucket_idx(get_size(block));
    block_t *next_block = free_list[bucket_idx];
    free_list[bucket_idx] = block;
    next_block ->body.free_blocks.prev = block;
    block->body.free_blocks.next = next_block;
    block->body.free_blocks.prev = NULL;
}
void coalesce(block_t *block){
    size_t size = get_size(block);
    size_t old_size = size;
    if(!is_next_alloc(block)){
        size+=get_size(get_next_ptr(block));
        unchain_free_block(get_next_ptr(block));
    }
    if(!is_prev_alloc(block)){
        block = get_prev_ptr(block);
        size+=get_size(block);
        unchain_free_block(get_prev_ptr(block));
    }
    if(old_size == size){
        set_prev_free(get_next_ptr(block));
        return ;
    }
    set_size(block, size);
    set_free(block);
    set_prev_free(get_next_ptr(block));
    move_freed_block(block);


}
void *split_block(block_t *block, size_t size){ //this function is called only if the block is large enough to be split.
    unchain_free_block(block);
    size_t remainder = get_size(block) - size;
    set_size(block, size);
    set_alloc(block);
    block_t *remainder_block = get_next_ptr(block);
    set_size(remainder_block, remainder);
    set_free(remainder_block);
    set_prev_alloc(remainder_block);
    move_freed_block(remainder_block);
    return (void *)((char*)block + sizeof(uint64_t));
}
void *find_fit(size_t size){
    size_t idx = get_bucket_idx(size);
    for(size_t i =idx; i < NUM_BUCKETS; i++){
        block_t *ptr = free_list[idx];
        while(ptr!=NULL){
            if ((ptr!=NULL)&&(get_size(ptr)>=size)){
                if(get_size(ptr)-size>=MIN_BLOCK_SIZE){
                    return split_block(ptr, size);
                }
                return (void*)((char *)ptr + sizeof(uint64_t));
            }
            else{
                ptr = ptr->body.free_blocks.next;
            }
        }
    }
    return NULL;
}
