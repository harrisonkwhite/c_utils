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
    typedef struct { \
        type buf_raw[len]; \
    } s_##name_snake; \
    \
    static inline type* name_pascal##_Get(s_##name_snake* const array, const int index) { \
        assert(index >= 0 && index < (len)); \
        return &array->buf_raw[index]; \
    } \
    \
    static inline const type* name_pascal##_GetConst(const s_##name_snake* const array, const int index) { \
        assert(index >= 0 && index < (len)); \
        return &array->buf_raw[index]; \
    }

#define DECLARE_ARRAY_TYPE(type, name_snake, name_pascal) \
    typedef struct { \
        type* buf_raw; \
        int len; \
    } s_##name_snake##_array; \
    \
    static inline void name_pascal##Array_AssertValidity(const s_##name_snake##_array array) { \
        assert(array.buf_raw); \
        assert(array.len >= 0); \
    } \
    \
    static inline type* name_pascal##Array_Get(const s_##name_snake##_array array, const int index) { \
        name_pascal##Array_AssertValidity(array); \
        assert(index >= 0 && index < array.len); \
        \
        return &array.buf_raw[index]; \
    } \
    \
    static inline s_##name_snake##_array Push##name_pascal##s(s_mem_arena* const mem_arena, const int cnt) { \
        type* const buf = PushToMemArena(mem_arena, sizeof(type) * (cnt), ALIGN_OF(type)); \
        \
        if (!buf) { \
            return (s_##name_snake##_array){0}; \
        } \
        \
        return (s_##name_snake##_array){ \
            .buf_raw = buf, \
            .len = cnt \
        }; \
    }

DECLARE_ARRAY_TYPE(t_byte, byte, Byte);
DECLARE_ARRAY_TYPE(char, char, Char);
DECLARE_ARRAY_TYPE(short, short, Short);
DECLARE_ARRAY_TYPE(unsigned short, ushort, UShort);
DECLARE_ARRAY_TYPE(int, int, Int);
DECLARE_ARRAY_TYPE(unsigned int, uint, UInt);
DECLARE_ARRAY_TYPE(float, float, Float);
DECLARE_ARRAY_TYPE(double, double, Double);

typedef struct {
    t_byte* bytes;
    size_t bit_cnt;
} s_bitset;

int Bitset_IndexOfFirstUnsetBit(const s_bitset bitset); // Returns -1 is there is no unset bit.

static inline void Bitset_AssertValidity(const s_bitset bitset) {
    assert(bitset.bytes);
    assert(bitset.bit_cnt > 0);
}

static inline void Bitset_SetBit(const s_bitset bitset, const size_t bit_index) {
    Bitset_AssertValidity(bitset);
    assert(bit_index < bitset.bit_cnt);

    bitset.bytes[bit_index / 8] |= 1 << (bit_index % 8);
}

static inline void Bitset_UnsetBit(const s_bitset bitset, const size_t bit_index) {
    Bitset_AssertValidity(bitset);
    assert(bit_index < bitset.bit_cnt);

    bitset.bytes[bit_index / 8] &= ~(1 << (bit_index % 8));
}

static inline bool Bitset_IsBitActive(const s_bitset bitset, const size_t bit_index) {
    Bitset_AssertValidity(bitset);
    assert(bit_index < bitset.bit_cnt);

    return bitset.bytes[bit_index / 8] & (1 << (bit_index % 8));
}

#define DECLARE_STATIC_BITSET_TYPE(bit_cnt, name_snake, name_pascal) \
    typedef t_byte t_##name_snake[BITS_TO_BYTES(bit_cnt)]; \
    \
    static inline void name_pascal##_SetBit(t_##name_snake* const bitset, const size_t bit_index) { \
        Bitset_SetBit((s_bitset){*bitset, bit_cnt}, bit_index); \
    } \
    \
    static inline void name_pascal##_UnsetBit(t_##name_snake* const bitset, const size_t bit_index) { \
        Bitset_UnsetBit((s_bitset){*bitset, bit_cnt}, bit_index); \
    } \
    \
    static inline bool name_pascal##_IsBitActive(const t_##name_snake* const bitset, const size_t bit_index) { \
        return Bitset_IsBitActive((s_bitset){*bitset, bit_cnt}, bit_index); \
    } \
    \
    static inline int name_pascal##_IndexOfFirstUnsetBit(const t_##name_snake* const bitset) { \
        return Bitset_IndexOfFirstUnsetBit((s_bitset){*bitset, bit_cnt}); \
    }

static inline t_byte KeepFirstNBitsOfByte(const t_byte byte, const size_t n) {
    assert(n <= 8);
    return byte & ((1 << n) - 1);
}

#endif
