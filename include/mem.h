#ifndef MEM_H
#define MEM_H

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#define KILOBYTES(x) (((size_t)1 << 10) * (x))
#define MEGABYTES(x) (((size_t)1 << 20) * (x))
#define GIGABYTES(x) (((size_t)1 << 30) * (x))
#define TERABYTES(x) (((size_t)1 << 40) * (x))

typedef int8_t t_s8;
typedef uint8_t t_u8;
typedef int16_t t_s16;
typedef uint16_t t_u16;
typedef int32_t t_s32;
typedef uint32_t t_u32;
typedef int64_t t_s64;
typedef uint64_t t_u64;

static inline bool IsPowerOfTwo(const size_t n) {
    return n > 0 && (n & (n - 1)) == 0;
}

static inline bool IsAlignmentValid(const size_t n) {
    return n > 0 && IsPowerOfTwo(n);
}

static inline size_t AlignForward(const size_t n, const size_t alignment) {
    assert(IsAlignmentValid(alignment));
    return (n + alignment - 1) & ~(alignment - 1);
}

static inline void ZeroOut(void* const mem, const size_t size) {
    assert(mem);
    assert(size > 0);

    memset(mem, 0, size);
}

#define ZERO_OUT(x) ZeroOut(&(x), sizeof(x))

static inline bool IsZero(const void* const mem, const size_t size) {
    assert(mem);
    assert(size > 0);

    const t_u8* const mem_u8 = mem;

    for (t_u8 i = 0; i < size; i++) {
        if (mem_u8[i]) {
            return false;
        }
    }

    return true;
}

#define IS_ZERO(x) IsZero(&(x), sizeof(x))

typedef struct {
    t_u8* buf;
    size_t size;
    size_t offs;
} s_mem_arena;

static inline bool IsMemArenaValid(const s_mem_arena* const arena) {
    assert(arena);
    return arena->buf && arena->size > 0 && arena->offs <= arena->size;
}

bool MemArenaInit(s_mem_arena* const arena, const size_t size);
void MemArenaClean(s_mem_arena* const arena);
void* MemArenaPush(s_mem_arena* const arena, const size_t size, const size_t alignment);
void MemArenaRewind(s_mem_arena* const arena, const size_t rewind_offs);

#define MEM_ARENA_PUSH_TYPE(arena, type) (type*)PushToMemArena(arena, sizeof(type), ALIGN_OF(type))
#define MEM_ARENA_PUSH_TYPE_CNT(arena, type, cnt) (type*)PushToMemArena(arena, sizeof(type) * (cnt), ALIGN_OF(type))

#endif
