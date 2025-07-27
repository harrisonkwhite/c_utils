#include "mem.h"

#include <stdlib.h>
#include "io.h"

bool MemArenaInit(s_mem_arena* const arena, const size_t size) {
    assert(arena && IS_ZERO(*arena));
    assert(size > 0);

    arena->buf = malloc(size);

    if (!arena->buf) {
        LogError("Failed to initialise memory arena of size %zu bytes!", size);
        return false;
    }

    ZeroOut(arena->buf, size);

    arena->size = size;

    return true;
}

void MemArenaClean(s_mem_arena* const arena) {
    assert(arena && IsMemArenaValid(arena));

    free(arena->buf);
    ZERO_OUT(*arena);
}

void* MemArenaPush(s_mem_arena* const arena, const size_t size, const size_t alignment) {
    assert(arena && IsMemArenaValid(arena));
    assert(size > 0);
    assert(IsAlignmentValid(alignment));

    const size_t offs_aligned = AlignForward(arena->offs, alignment);
    const size_t offs_next = offs_aligned + size;

    if (offs_next > arena->size) {
        LogError("Failed to push %zu bytes to memory arena!", size);
        return NULL;
    }

    arena->offs = offs_next;

    return arena->buf + offs_aligned;
}

void MemArenaRewind(s_mem_arena* const arena, const size_t rewind_offs) {
    assert(arena && IsMemArenaValid(arena));
    assert(rewind_offs <= arena->offs);

    if (rewind_offs != arena->offs) {
        ZeroOut(arena->buf + rewind_offs, arena->offs - rewind_offs);
        arena->offs = rewind_offs;
    }
}
