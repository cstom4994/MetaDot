// Copyright(c) 2022, KaoruXun All rights reserved.

// Hack from https://github.com/cesarl/LiveMemTracer

#ifndef _METADOT_GCMANAGER_HPP_
#define _METADOT_GCMANAGER_HPP_

#include <cstdint>

#include "Engine/Memory/Allocator.h"
#include "Engine/Render/SDLWrapper.hpp"


#define METADOT_GC_USE_MALLOC SDL_malloc
#define METADOT_GC_USE_REALLOC SDL_realloc
#define METADOT_GC_USE_FREE SDL_free

#define METADOT_GC_ALLOC(size) SDL_malloc(size)
#define METADOT_GC_ALLOC_ALIGNED(size, alignment) SDL_malloc(size)
#define METADOT_GC_DEALLOC(ptr) SDL_free(ptr)
#define METADOT_GC_DEALLOC_ALIGNED(ptr) SDL_free(ptr)
#define METADOT_GC_REALLOC(ptr, size) SDL_realloc(ptr, size)
#define METADOT_GC_REALLOC_ALIGNED(ptr, size, alignment) SDL_realloc(ptr, size, alignment)

struct GC
{
    static CAllocator* C;
};

void METAENGINE_Memory_Init(int argc, char *argv[]);
void METAENGINE_Memory_End();

#if defined(METADOT_LEAK_TEST)

void *operator new(std::size_t sz);
void operator delete(void *ptr) noexcept;

void getInfo();

#endif

#endif// _METADOT_GCMANAGER_HPP_