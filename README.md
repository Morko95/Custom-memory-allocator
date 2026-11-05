# Custom-memory-allocator

A high-performance, explicit heap memory allocator written in C, utilizing **segregated free lists**, constant-time bidirectional coalescing, and power-of-two size classes. Designed to minimize internal and external memory fragmentation while achieving sub-microsecond allocation and deallocation throughput.

---

## Architectural Mechanisms

### 1. Segregated Free Lists & Power-of-Two Size Classes
* **Size Segregation:** Free blocks are partitioned into distinct, explicit doubly-linked lists bucketed by powers of two starting at 32 bytes (`[32-63]`, `[64-127]`, ..., `[4096+]`).
* **Search Strategy:** First-fit traversal constrained to the matching size bucket; escalates to subsequent size classes on miss, approximating best-fit memory utilization with near $O(1)$ search latency.
* **Explicit In-Place Pointers:** Deallocated blocks repurpose their uninitialized payload memory to store explicit `next` and `prev` pointers, eliminating external tracking metadata overhead.

### 2. Alignment & Boundary Tag Optimization (Footerless Allocated Blocks)
* **16-Byte Strict Alignment:** All payload boundaries adhere to 16-byte alignment (`0x0 mod 16`) for SIMD/AVX vector instruction compatibility.
* **Footerless Active Blocks:** To drastically reduce internal fragmentation on small allocations, boundary footers are entirely omitted from allocated blocks. Footers exist exclusively in free blocks to support backward traversal.
* **Two-Bit State Encoding:** Leveraging 16-byte alignment (where the lowest 4 bits of any valid block size are zero), the two lowest bits of each 8-byte header encode continuous block states:
  * Bit 0 (`0x1`): `CURR_ALLOC` (1 = current block is allocated, 0 = free).
  * Bit 1 (`0x2`): `PREV_ALLOC` (1 = preceding physical block is allocated, 0 = free).
* **Asymmetric $O(1)$ Coalescing:** During `free(p)`, the allocator inspects `PREV_ALLOC` in the current header. If unset ($0$), it accesses the immediately preceding free footer to determine size and coalesce backwards in $O(1)$ time—bypassing the need for footers on active chunks.

```text
Allocated Block (Payload maximized, zero footer overhead):
[ Header: Size | PREV_ALLOC | 1 ] -> [ Payload (16-byte aligned) ]

Free Block (Retains footer and explicit doubly-linked pointers):
[ Header: Size | PREV_ALLOC | 0 ] -> [ Next* ] -> [ Prev* ] -> [ Padding... ] -> [ Footer: Size ]
```

### 3. Allocation Lifecycle & Memory Reclamation
* **Dynamic Block Splitting:** Blocks exceeding the requested capacity by at least the minimum block threshold (32 bytes) are split dynamically. The trailing remainder is converted into a free block and re-inserted into the appropriate segregated list.
* **Constant-Time Coalescing:** Immediate coalescing across all four boundary combinations upon `free()` prevents creeping external fragmentation.
* **Heap Expansion (`sbrk`):** When free list buckets cannot service an allocation, the heap expands via page-aligned `sbrk()` system calls, followed by epilogue tag updates.

### 4. Implementation Details
* **Zero-Cost Inline Arithmetic:** All pointer arithmetic, tag masking, alignment padding, and boundary computations are declared as `static inline` functions to prevent call-stack overhead and allow aggressive LTO compiler vectorization.
* **Deterministic Constructor Initialization:** Employs `__attribute__((constructor))` to initialize static prologue and epilogue sentinels prior to executing `main()`.

---

## Interface

Exposes POSIX-compliant drop-in allocation primitives:

```c
void *mm_malloc(size_t size);
void  mm_free(void *ptr);
void *mm_realloc(void *ptr, size_t size);
```

---

## Build & Verification

### Prerequisites
* GCC or Clang (C99 / C11)
* CMake ≥ 3.20

### Compilation
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```
