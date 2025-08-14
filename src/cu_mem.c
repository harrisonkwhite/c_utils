#include "cu_mem.h"

#include <stdlib.h>
#include "cu_io.h"

bool InitMemArena(s_mem_arena* const arena, const size_t size) {
    assert(IS_ZERO(*arena));

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
    assert(arena->buf);

    free(arena->buf);
    ZERO_OUT(*arena);
}

void* PushToMemArena(s_mem_arena* const arena, const size_t size, const size_t alignment) {
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
    assert(rewind_offs <= arena->offs);

    if (rewind_offs != arena->offs) {
        ZeroOut(arena->buf + rewind_offs, arena->offs - rewind_offs);
        arena->offs = rewind_offs;
    }
}

t_s32 IndexOfFirstUnsetBit(const s_bitset_view bitset) {
    for (t_s32 i = 0; i < (bitset.bit_cnt / 8); i++) {
        const t_u8 byte = *U8ElemView(bitset.bytes, i);

        if (byte == 0xFF) {
            continue;
        }

        for (t_s32 j = 0; j < 8; j++) {
            if (!(byte & (1 << j))) {
                return (i * 8) + j;
            }
        }
    }

    const t_s32 excess_bits = bitset.bit_cnt % 8;

    if (excess_bits > 0) {
        // Get the last byte, masking out any bits we don't care about.
        const t_u8 last_byte = KeepFirstNBitsOfByte(*U8ElemView(bitset.bytes, bitset.bit_cnt / 8), excess_bits);

        if (last_byte != 0xFF) {
            for (t_s32 i = 0; i < 8; i++) {
                if (!(last_byte & (1 << i))) {
                    return bitset.bit_cnt - excess_bits + i;
                }
            }
        }
    }

    return -1;
}
