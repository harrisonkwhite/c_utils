#ifndef CU_MEM_H
#define CU_MEM_H

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

#ifdef _MSC_VER
#define ALIGN_OF(x) __alignof(x)
#else
#include <stdalign.h>
#define ALIGN_OF(x) alignof(x)
#endif

typedef unsigned char t_byte;

typedef struct {
    t_byte* buf;
    size_t size;
    size_t offs;
} s_mem_arena;

static inline bool IsMemArenaValid(const s_mem_arena* const arena) {
    assert(arena);
    return arena->buf && arena->size > 0 && arena->offs <= arena->size;
}

int FirstInactiveBitIndex(const t_byte* const bytes, const size_t bit_cnt); // Returns -1 if an inactive bit is not found.

bool InitMemArena(s_mem_arena* const arena, const size_t size);
void CleanMemArena(s_mem_arena* const arena);
void* PushToMemArena(s_mem_arena* const arena, const size_t size, const size_t alignment);
void RewindMemArena(s_mem_arena* const arena, const size_t rewind_offs);

#define MEM_ARENA_PUSH_TYPE(arena, type) (type*)PushToMemArena(arena, sizeof(type), ALIGN_OF(type))
#define MEM_ARENA_PUSH_TYPE_CNT(arena, type, cnt) (type*)PushToMemArena(arena, sizeof(type) * (cnt), ALIGN_OF(type))

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

    const t_byte* const mem_u8 = mem;

    for (size_t i = 0; i < size; i++) {
        if (mem_u8[i]) {
            return false;
        }
    }

    return true;
}

#define IS_ZERO(x) IsZero(&(x), sizeof(x))

static inline size_t IndexFrom2D(const size_t x, const size_t y, const size_t width) {
    assert(x < width);
    return (width * y) + x;
}

#define DECLARE_STATIC_ARRAY_TYPE(type, len, name_snake, name_pascal) \
    typedef type t_##name_snake[len]; \
    \
    static inline int* name_pascal##_Get(t_##name_snake* const array, const int index) { \
        assert(index >= 0 && index < len); \
        return &(*array)[index]; \
    }

#define DECLARE_ARRAY_TYPE(type, name_snake, name_pascal) \
    typedef struct { \
        type* buf; \
        int len; \
    } s_##name_snake##_array; \
    \
    static inline void name_pascal##Array_AssertValidity(const s_##name_snake##_array* const array) { \
        assert(array); \
        assert(array->buf); \
        assert(array->len >= 0); \
    } \
    \
    static inline type* name_pascal##Array_Get(const s_##name_snake##_array* const array, const int index) { \
        name_pascal##Array_AssertValidity(array); \
        assert(index >= 0 && index < array->len); \
        \
        return &array->buf[index]; \
    } \
    \
    static inline s_##name_snake##_array Push##name_pascal##s(s_mem_arena* const mem_arena, const int cnt) { \
        type* const buf = MEM_ARENA_PUSH_TYPE_CNT(mem_arena, type, cnt); \
        \
        if (!buf) { \
            return (s_##name_snake##_array){0}; \
        } \
        \
        return (s_##name_snake##_array){ \
            .buf = buf, \
            .len = cnt \
        }; \
    }

DECLARE_ARRAY_TYPE(int, int, Int);
DECLARE_ARRAY_TYPE(float, float, Float);
DECLARE_ARRAY_TYPE(double, double, Double);

static inline void ActivateBit(t_byte* const bytes, const size_t bit_index, const size_t bit_cnt) {
    assert(bytes);
    assert(bit_index < bit_cnt);
    assert(bit_cnt > 0);

    bytes[bit_index / 8] |= 1 << (bit_index % 8);
}

static inline void DeactivateBit(t_byte* const bytes, const size_t bit_index, const size_t bit_cnt) {
    assert(bytes);
    assert(bit_index < bit_cnt);
    assert(bit_cnt > 0);

    bytes[bit_index / 8] &= ~(1 << (bit_index % 8));
}

static inline bool IsBitActive(const t_byte* const bytes, const size_t bit_index, const size_t bit_cnt) {
    assert(bytes);
    assert(bit_index < bit_cnt);
    assert(bit_cnt > 0);

    return bytes[bit_index / 8] & (1 << (bit_index % 8));
}

static inline t_byte KeepFirstNBitsOfByte(const t_byte byte, const size_t n) {
    assert(n <= 8);
    return byte & ((1 << n) - 1);
}

#endif
