#ifndef ALLOCATOR_H
#define ALLOCATOR_H
#include <stddef.h>
enum{
    WSIZE = 8,
    ALIGN = 16,
    PROLOG = 16ULL | 0x1ULL | 0x2ULL,
    EPILOG = 0ULL  | 0x1ULL | 0x2ULL,
    CHUNKSIZE = 4096
};
static inline size_t m_align(size_t size){
    return (size<32) ? (((size) + 15ULL) & ~0xFULL): 32;
}
static inline size_t m_alloc_amount(size_t size){
    return (size<CHUNKSIZE)?CHUNKSIZE:size;
}
#endif
