#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "shared/memory.h"
#include "shared/log.h"

// global instance of arena,
// only place memory is stored in the program
// is not freed, assumes enough memory for all
// alocations to exist
static Arena arena;

// increments pointer until it is aligned to align
// align (bytes) must be a power of 2
// returns the amount the pointer was incremented by
// modifies the pointer
static int AlignForward(void* real_ptr, size_t align) {
    uintptr_t ptr = (uintptr_t)real_ptr;
    uintptr_t align_ptr = align;

    uintptr_t modulo = ptr % align_ptr;

    return modulo==0?0:align_ptr - modulo;
}

void ArenaInit() {
    CONTEXT("Memory Initialization");

    arena.pageSize = 4096 * 16;
    
    // add an initial page
    arena.align = 2 * sizeof(void*);
    arena.areas = malloc(sizeof(Arena));
    arena.areas[0].end = malloc(arena.pageSize);
    arena.arenaCount = 1;
    if(arena.areas == NULL || arena.areas[0].end == NULL) {
        FATAL("Could not allocate memory for arena");
        exit(1);
    }
    arena.areas[0].bytesLeft = arena.pageSize;
    TRACE("Allocated %u bytes", arena.pageSize);
}

void* ArenaAlloc(size_t size) {
    return ArenaAllocAlign(size, arena.align);
}

void* ArenaAllocAlign(size_t size, size_t align) {
    CONTEXT("Arena Allocation");

    void* ptr = NULL;
    size_t i;

    // find area with enough remaining memory
    for(i = 0; i < arena.arenaCount; i++){
        // ensure there is enough space incase the pointer needs re-aligning
        if(arena.areas[i].bytesLeft > size + align){
            ptr = (void*)arena.areas[i].end;
            break;
        }
    }

    // no large enough area found so add new area that is large enough
    if(ptr == NULL) {

        // ensure that new area will be large enough
        if(arena.pageSize < size) {
            arena.pageSize = size;
        }

        // expand the arena base pointer array
        arena.arenaCount++;
        arena.areas = realloc(arena.areas, sizeof(Arena)*arena.arenaCount);
        if(arena.areas == NULL) {
            FATAL("Could not expand arena area list from %u to %u",
                arena.arenaCount-1, arena.arenaCount);
            exit(1);
        }

        // allocate the area
        arena.areas[arena.arenaCount-1].end = malloc(arena.pageSize);
        if(arena.areas[arena.arenaCount-1].end == NULL) {
            FATAL("Could not create new area");
            exit(1);
        }
        ptr = (void*)arena.areas[arena.arenaCount-1].end;
        arena.areas[arena.arenaCount-1].bytesLeft = arena.pageSize;
    }

    // align end pointer and ensure area's pointers are updated
    int alignOffset = AlignForward(ptr, align);
    arena.areas[i].end = (void*)((uintptr_t)arena.areas[i].end + size + alignOffset);
    arena.areas[i].bytesLeft -= size + alignOffset;

    TRACE("Assigned %u bytes from arena, %u left until reallocation", 
        size + alignOffset, arena.areas[i].bytesLeft);
    return ptr;
}

void* ArenaReAlloc(void* old_ptr, size_t old_size, size_t new_size) {
    CONTEXT("Array re-allocation");
    void* new_ptr = ArenaAlloc(new_size);
    memcpy(new_ptr, old_ptr, old_size);
    return new_ptr;
}
