#ifndef MEM_H
#define MEM_H

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#define STATIC_ARRAY_LEN(array) (sizeof(array) / sizeof((array)[0]))
#define STATIC_ARRAY_LEN_CHECK(array, len_targ) static_assert(STATIC_ARRAY_LEN(array) == (len_targ), "Static array length must match target!")

#define KILOBYTES(x) (((size_t)1 << 10) * (x))
#define MEGABYTES(x) (((size_t)1 << 20) * (x))
#define GIGABYTES(x) (((size_t)1 << 30) * (x))

#define BITS_TO_BYTES(x) (((x) + 7) / 8)
#define BYTES_TO_BITS(x) ((x) * 8)

#define SIZE_IN_BITS(x) BYTES_TO_BITS(sizeof(x))

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

#ifdef _MSC_VER
#define ALIGN_OF(x) __alignof(x)
#else
#include <stdalign.h>
#define ALIGN_OF(x) alignof(x)
#endif

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

    for (size_t i = 0; i < size; i++) {
        if (mem_u8[i]) {
            return false;
        }
    }

    return true;
}

#define IS_ZERO(x) IsZero(&(x), sizeof(x))

static inline void ActivateBit(const size_t bit_index, t_u8* const bytes, const size_t bit_cnt) {
    assert(bit_index < bit_cnt);
    assert(bytes);
    assert(bit_cnt > 0);

    bytes[bit_index / 8] |= 1 << (bit_index % 8);
}

static inline void DeactivateBit(const size_t bit_index, t_u8* const bytes, const size_t bit_cnt) {
    assert(bit_index < bit_cnt);
    assert(bytes);
    assert(bit_cnt > 0);

    bytes[bit_index / 8] &= ~(1 << (bit_index % 8));
}

static inline bool IsBitActive(const size_t bit_index, const t_u8* const bytes, const size_t bit_cnt) {
    assert(bit_index < bit_cnt);
    assert(bytes);
    assert(bit_cnt > 0);

    return bytes[bit_index / 8] & (1 << (bit_index % 8));
}

static inline t_u8 KeepFirstNBitsOfByte(const t_u8 byte, const size_t n) {
    assert(n <= 8);
    return byte & ((1 << n) - 1);
}

t_s32 FirstInactiveBitIndex(const t_u8* const bytes, const t_s32 bit_cnt); // Returns -1 if an inactive bit is not found.

typedef struct {
    t_u8* buf;
    size_t size;
    size_t offs;
} s_mem_arena;

static inline bool IsMemArenaValid(const s_mem_arena* const arena) {
    assert(arena);
    return arena->buf && arena->size > 0 && arena->offs <= arena->size;
}

bool InitMemArena(s_mem_arena* const arena, const size_t size);
void CleanMemArena(s_mem_arena* const arena);
void* PushToMemArena(s_mem_arena* const arena, const size_t size, const size_t alignment);
void RewindMemArena(s_mem_arena* const arena, const size_t rewind_offs);

#define MEM_ARENA_PUSH_TYPE(arena, type) (type*)PushToMemArena(arena, sizeof(type), ALIGN_OF(type))
#define MEM_ARENA_PUSH_TYPE_CNT(arena, type, cnt) (type*)PushToMemArena(arena, sizeof(type) * (cnt), ALIGN_OF(type))

#endif
