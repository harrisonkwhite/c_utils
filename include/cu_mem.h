#ifndef CU_MEM_H
#define CU_MEM_H

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#define KILOBYTES(x) (((size_t)1 << 10) * (x))
#define MEGABYTES(x) (((size_t)1 << 20) * (x))
#define GIGABYTES(x) (((size_t)1 << 30) * (x))
#define TERABYTES(x) (((size_t)1 << 40) * (x))

#define BITS_TO_BYTES(x) (((x) + 7) / 8)
#define BYTES_TO_BITS(x) ((x) * 8)

#define SIZE_IN_BITS(x) BYTES_TO_BITS(sizeof(x))

#ifdef _MSC_VER
#define ALIGN_OF(x) __alignof(x)
#else
#include <stdalign.h>
#define ALIGN_OF(x) alignof(x)
#endif

typedef int8_t t_s8;
typedef uint8_t t_u8;
typedef int16_t t_s16;
typedef uint16_t t_u16;
typedef int32_t t_s32;
typedef uint32_t t_u32;
typedef int64_t t_s64;
typedef uint64_t t_u64;

typedef float t_r32;
static_assert(sizeof(t_r32) == 4, "Type size assumption broken!");

typedef double t_r64;
static_assert(sizeof(t_r64) == 8, "Type size assumption broken!");

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

static inline t_u8 KeepFirstNBitsOfByte(const t_u8 byte, const size_t n) {
    assert(n <= 8);
    return byte & ((1 << n) - 1);
}

static inline size_t IndexFrom2D(const size_t x, const size_t y, const size_t width) {
    assert(x < width);
    return (width * y) + x;
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

typedef struct {
    t_u8* buf;
    size_t size;
    size_t offs;
} s_mem_arena;

bool InitMemArena(s_mem_arena* const arena, const size_t size);
void CleanMemArena(s_mem_arena* const arena);
void* PushToMemArena(s_mem_arena* const arena, const size_t size, const size_t alignment);
void RewindMemArena(s_mem_arena* const arena, const size_t rewind_offs);

#define STATIC_ARRAY_LEN(array) (sizeof(array) / sizeof((array)[0]))

#define STATIC_ARRAY_LEN_CHECK(array, ideal_len) static_assert(STATIC_ARRAY_LEN(array) == (ideal_len), "Invalid static array length!");

#define STATIC_ARRAY_ELEM(array, index) ( \
        assert((index) >= 0 && (index) < STATIC_ARRAY_LEN(array)), \
        &(array)[index] \
    )

#define STATIC_ARRAY_2D_ELEM(array, i, j) ( \
        assert((i) >= 0 && (i) < STATIC_ARRAY_LEN(array)), \
        assert((j) >= 0 && (j) < STATIC_ARRAY_LEN((array)[0])), \
        &(array)[i][j] \
    )

#define STATIC_ARRAY_3D_ELEM(array, i, j, k) ( \
        assert((i) >= 0 && (i) < STATIC_ARRAY_LEN(array)), \
        assert((j) >= 0 && (j) < STATIC_ARRAY_LEN((array)[0])), \
        assert((k) >= 0 && (k) < STATIC_ARRAY_LEN((array)[0][0])), \
        &(array)[i][j][k] \
    )

#define DEF_ARRAY_TYPE(type, name_snake, name_pascal) \
    typedef struct { \
        type* buf_raw; \
        t_s32 elem_cnt; \
    } s_##name_snake##_array; \
    \
    typedef struct { \
        const type* buf_raw; \
        t_s32 elem_cnt; \
    } s_##name_snake##_array_view; \
    \
    static inline s_##name_snake##_array_view name_pascal##ArrayView(const s_##name_snake##_array array) { \
        return (s_##name_snake##_array_view){.buf_raw = array.buf_raw, .elem_cnt = array.elem_cnt}; \
    } \
    \
    static inline type* name_pascal##Elem(const s_##name_snake##_array array, const t_s32 index) { \
        assert(index >= 0 && index < array.elem_cnt); \
        return &array.buf_raw[index]; \
    } \
    static inline const type* name_pascal##ElemView(const s_##name_snake##_array_view array, const t_s32 index) { \
        assert(index >= 0 && index < array.elem_cnt); \
        return &array.buf_raw[index]; \
    } \
    \
    static inline s_##name_snake##_array Push##name_pascal##ArrayToMemArena(s_mem_arena* const arena, const t_s32 elem_cnt) { \
        type* const buf = PushToMemArena(arena, sizeof(type) * (elem_cnt), ALIGN_OF(type)); \
        \
        if (!buf) { \
            return (s_##name_snake##_array){0}; \
        } \
        \
        return (s_##name_snake##_array){ \
            .buf_raw = buf, \
            .elem_cnt = elem_cnt \
        }; \
    }

#define ARRAY_FROM_STATIC(static_array) {.buf_raw = static_array, .elem_cnt = STATIC_ARRAY_LEN(static_array)}

DEF_ARRAY_TYPE(char, char, Char);

static inline s_char_array StrFromRawTerminated(char* const raw) {
    return (s_char_array){
        .buf_raw = raw, .elem_cnt = strlen(raw) + 1
    };
}

static inline s_char_array_view StrViewFromRawTerminated(const char* const raw) {
    return (s_char_array_view){
        .buf_raw = raw, .elem_cnt = strlen(raw) + 1
    };
}

static inline bool IsStrTerminated(const s_char_array_view str) {
    for (int i = 0; i < str.elem_cnt; i++) {
        if (!*CharElemView(str, i)) {
            return true;
        }
    }

    return false;
}

DEF_ARRAY_TYPE(s_char_array, char_array, CharArray);
DEF_ARRAY_TYPE(s_char_array_view, char_array_view, CharArrayView);

DEF_ARRAY_TYPE(t_r32, r32, R32);
DEF_ARRAY_TYPE(t_r64, r64, R64);

DEF_ARRAY_TYPE(t_u8, u8, U8);
DEF_ARRAY_TYPE(t_s8, s8, S8);
DEF_ARRAY_TYPE(t_s16, s16, S16);
DEF_ARRAY_TYPE(t_u16, u16, U16);
DEF_ARRAY_TYPE(t_s32, s32, S32);
DEF_ARRAY_TYPE(t_u32, u32, U32);
DEF_ARRAY_TYPE(t_s64, s64, S64);
DEF_ARRAY_TYPE(t_u64, u64, U64);

typedef struct {
    s_u8_array bytes;
    size_t bit_cnt;
} s_bitset;

typedef struct {
    s_u8_array_view bytes;
    size_t bit_cnt;
} s_bitset_view;

t_s32 IndexOfFirstUnsetBit(const s_bitset_view bitset); // Returns -1 if there is no inactive bit.

static inline s_bitset_view BitsetView(const s_bitset bitset) {
    return (s_bitset_view){.bytes = U8ArrayView(bitset.bytes), .bit_cnt = bitset.bit_cnt};
}

static inline void SetBit(const s_bitset bitset, const size_t bit_index) {
    assert(bit_index < bitset.bit_cnt);
    *U8Elem(bitset.bytes, bit_index / 8) |= (1 << (bit_index % 8));
}

static inline void UnsetBit(const s_bitset bitset, const size_t bit_index) {
    assert(bit_index < bitset.bit_cnt);
    *U8Elem(bitset.bytes, bit_index / 8) &= ~(1 << (bit_index % 8));
}

static inline bool IsBitSet(const s_bitset_view bitset, const size_t bit_index) {
    assert(bit_index < bitset.bit_cnt);
    return *U8ElemView(bitset.bytes, bit_index / 8) & (1 << (bit_index % 8));
}

#define DEF_STATIC_BITSET_TYPE(name_snake, name_pascal, _bit_cnt) \
    typedef t_u8 t_##name_snake[BITS_TO_BYTES(_bit_cnt)]; \
    \
    static inline void Set##name_pascal##Bit(t_##name_snake* const bitset, const size_t bit_index) { \
        SetBit((s_bitset){.bytes = ARRAY_FROM_STATIC(*bitset), .bit_cnt = _bit_cnt}, bit_index); \
    } \
    \
    static inline void Unset##name_pascal##Bit(t_##name_snake* const bitset, const size_t bit_index) { \
        UnsetBit((s_bitset){.bytes = ARRAY_FROM_STATIC(*bitset), .bit_cnt = _bit_cnt}, bit_index); \
    } \
    \
    static inline bool Is##name_pascal##BitSet(const t_##name_snake* const bitset, const size_t bit_index) { \
        return IsBitSet((s_bitset_view){.bytes = ARRAY_FROM_STATIC(*bitset), .bit_cnt = _bit_cnt}, bit_index); \
    } \
    \
    static inline t_s32 FirstUnset##name_pascal##BitIndex(const t_##name_snake* const bitset) { \
        return IndexOfFirstUnsetBit((s_bitset_view){.bytes = ARRAY_FROM_STATIC(*bitset), .bit_cnt = _bit_cnt}); \
    }

#endif
