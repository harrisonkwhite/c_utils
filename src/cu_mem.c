#include "cu_mem.h"

#include <stdlib.h>
#include "cu_io.h"

int Bitset_IndexOfFirstUnsetBit(const s_bitset bitset) {
    Bitset_AssertValidity(bitset);

    for (int i = 0; i < (bitset.bit_cnt / 8); i++) {
        if (bitset.bytes[i] == 0xFF) {
            continue;
        }

        for (int j = 0; j < 8; j++) {
            if (!(bitset.bytes[i] & (1 << j))) {
                return (i * 8) + j;
            }
        }
    }

    const int excess_bits = bitset.bit_cnt % 8;

    if (excess_bits > 0) {
        // Get the last byte, masking out any bits we don't care about.
        const t_byte last_byte = KeepFirstNBitsOfByte(bitset.bytes[bitset.bit_cnt / 8], excess_bits);

        if (last_byte != 0xFF) {
            for (int i = 0; i < 8; i++) {
                if (!(last_byte & (1 << i))) {
                    return bitset.bit_cnt - excess_bits + i;
                }
            }
        }
    }
    
    return -1;
}

bool InitMemArena(s_mem_arena* const arena, const size_t size) {
    assert(arena && IS_ZERO(*arena));
    assert(size > 0);

    arena->buf = malloc(size);

    if (!arena->buf) {
        LOG_ERROR("Failed to initialise memory arena of size %zu bytes!", size);
        return false;
    }

    ZeroOut(arena->buf, size);

    arena->size = size;

    return true;
}

void CleanMemArena(s_mem_arena* const arena) {
    assert(arena && IsMemArenaValid(arena));

    free(arena->buf);
    ZERO_OUT(*arena);
}

void* PushToMemArena(s_mem_arena* const arena, const size_t size, const size_t alignment) {
    assert(arena && IsMemArenaValid(arena));
    assert(size > 0);
    assert(IsAlignmentValid(alignment));

    const size_t offs_aligned = AlignForward(arena->offs, alignment);
    const size_t offs_next = offs_aligned + size;

    if (offs_next > arena->size) {
        LOG_ERROR("Failed to push %zu bytes to memory arena!", size);
        return NULL;
    }

    arena->offs = offs_next;

    return arena->buf + offs_aligned;
}

void RewindMemArena(s_mem_arena* const arena, const size_t rewind_offs) {
    assert(arena && IsMemArenaValid(arena));
    assert(rewind_offs <= arena->offs);

    if (rewind_offs != arena->offs) {
        ZeroOut(arena->buf + rewind_offs, arena->offs - rewind_offs);
        arena->offs = rewind_offs;
    }
}
