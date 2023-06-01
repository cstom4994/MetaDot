#ifndef ME_MEMORY_HPP
#define ME_MEMORY_HPP

#include "basic_types.h"

//--------------------------------------------------------------------------------------------------------------------------------//
// MEMORY FUNCTIONS USED

#ifndef ME_MALLOC_FUNC
#define ME_MALLOC_FUNC(s) malloc((s))
#endif

#ifndef ME_FREE_FUNC
#define ME_FREE_FUNC(p) free((p))
#endif

// #ifndef ME_REALLOC
// #define ME_REALLOC(p, s) realloc(p, s)
// #endif

typedef struct ME_memory_alloc_stack_t ME_memory_alloc_stack_t;
ME_memory_alloc_stack_t* ME_memory_alloc_stack_create(void* memory_chunk, size_t size);
void* ME_memory_alloc_stack_alloc(ME_memory_alloc_stack_t* stack, size_t size);
int ME_memory_alloc_stack_free(ME_memory_alloc_stack_t* stack, void* memory);
size_t ME_memory_alloc_stack_bytes_left(ME_memory_alloc_stack_t* stack);

typedef struct ME_memory_alloc_frame_t ME_memory_alloc_frame_t;
ME_memory_alloc_frame_t* ME_memory_alloc_frame_create(void* memory_chunk, size_t size);
void* ME_memory_alloc_frame_alloc(ME_memory_alloc_frame_t* frame, size_t size);
void ME_memory_alloc_frame_free(ME_memory_alloc_frame_t* frame);

typedef struct ME_memory_alloc_heap_t ME_memory_alloc_heap_t;
ME_memory_alloc_heap_t* ME_memory_alloc_heap_create(void* memory_chunk, size_t size);
void* ME_memory_alloc_heap_alloc(ME_memory_alloc_heap_t* frame, size_t size);
void ME_memory_alloc_heap_free(ME_memory_alloc_heap_t* frame);

// define these to your own user definition as necessary
#ifndef ME_MALLOC
#define ME_MALLOC(size) ME_memory_alloc_leak_check_alloc((size), (char*)__FILE__, __LINE__)
#endif

#ifndef ME_FREE
#define ME_FREE(mem) ME_memory_alloc_leak_check_free(mem)
#endif

#ifndef ME_CALLOC
#define ME_CALLOC(count, element_size) ME_memory_alloc_leak_check_calloc(count, element_size, (char*)__FILE__, __LINE__)
#endif

void* ME_memory_alloc_leak_check_alloc(size_t size, const char* file, int line);
void* ME_memory_alloc_leak_check_calloc(size_t count, size_t element_size, const char* file, int line);
void ME_memory_alloc_leak_check_free(void* mem);
int ME_memory_check_leaks();
int ME_memory_bytes_inuse();

typedef struct allocation_metrics {
    u64 total_allocated;
    u64 total_free;
} allocation_metrics;

extern allocation_metrics g_allocation_metrics;

u64 ME_mem_current_usage_bytes();
f32 ME_mem_current_usage_mb();

void ME_mem_init(int argc, char* argv[]);
void ME_mem_end();
void ME_mem_rungc();

// #define ME_LEAK_TEST

#if defined(ME_LEAK_TEST)

void* operator new(std::size_t sz);
void operator delete(void* ptr) noexcept;

void ME_get_memory_leak();

#endif

#endif