#include "free_list.h"
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include "allocator.h"

block_t *extend_heap(size_t size){
   char *ptr = (char *)sbrk(size); //todo handle failed sbrk
   block_t *block = (block_t *)ptr;
   set_size(block, size);
   set_free(block);
   ptr+=size;
   *(uint64_t *)ptr = (uint64_t)EPILOG;
   coalesce(block);
   return block;
}

int m_init(void){
    for(size_t i=0; i<NUM_BUCKETS; i++){
        free_list[i] = NULL;
    }
    uint64_t *heap_start = (uint64_t *)(sbrk(4*sizeof(uint64_t)));
    heap_start[0] = 0;
    heap_start[1] = (uint64_t)PROLOG;
    heap_start[2] = (uint64_t)PROLOG;
    heap_start[3] = (uint64_t)EPILOG;
    block_t *first_block = extend_heap(CHUNKSIZE);
    move_freed_block(first_block);
    return 0;

}

static int initialized = 0;

__attribute__((constructor))
static void allocator_global_init(void) {
    if (!initialized) {
        m_init();
        initialized = 1;
    }
}

void *malloc(size_t size) {
    if(size <= 0){
        return NULL;
    }
    if (!initialized) {
        allocator_global_init();
    }
    size_t asize = m_align(size + 2*WSIZE);
    void *ptr = find_fit(asize);
    if(ptr==NULL){
        size_t heap_grow = m_alloc_amount(asize);
        block_t *new_block = extend_heap(heap_grow);
        if(heap_grow-asize>=32){
            ptr = split_block(new_block,asize);
            return ptr;
        }
        else{
            return (void *)((char *)(new_block) + sizeof(uint64_t));
        }
    }
    else{
        return ptr;
    }
}

void free(void *ptr){
    if(ptr == NULL){
        return;
    }
    block_t *block = (block_t *)((char *)ptr - sizeof(uint64_t));
    set_free(block);
    coalesce(block);
}
