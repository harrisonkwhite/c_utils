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
static_assert(sizeof(t_r32) == 4);

typedef double t_r64;
static_assert(sizeof(t_r64) == 8);

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

#define DECLARE_STATIC_ARRAY_TYPE(type, len, name_snake, name_pascal) \
    typedef struct { \
        type buf_raw[len]; \
    } s_##name_snake; \
    \
    static inline type* name_pascal##Elem(s_##name_snake* const array, const int index) { \
        assert(index >= 0 && index < (len)); \
        return &array->buf_raw[index]; \
    } \
    \
    static inline const type* name_pascal##ElemConst(const s_##name_snake* const array, const int index) { \
        assert(index >= 0 && index < (len)); \
        return &array->buf_raw[index]; \
    }

#define DECLARE_ARRAY_TYPE(type, name_snake, name_pascal) \
    typedef struct { \
        type* buf_raw; \
        int len; \
    } s_##name_snake; \
    \
    typedef struct { \
        const type* buf_raw; \
        int len; \
    } s_##name_snake##_view; \
    \
    static inline s_##name_snake##_view name_pascal##View(const s_##name_snake array) { \
        return (s_##name_snake##_view){.buf_raw = array.buf_raw, .len = array.len}; \
    } \
    \
    static inline type* name_pascal##Elem(const s_##name_snake array, const int index) { \
        assert(index >= 0 && index < array.len); \
        return &array.buf_raw[index]; \
    } \
    static inline const type* name_pascal##ElemView(const s_##name_snake##_view array, const int index) { \
        assert(index >= 0 && index < array.len); \
        return &array.buf_raw[index]; \
    } \
    \
    static inline s_##name_snake Push##name_pascal(s_mem_arena* const mem_arena, const int cnt) { \
        type* const buf = PushToMemArena(mem_arena, sizeof(type) * (cnt), ALIGN_OF(type)); \
        \
        if (!buf) { \
            return (s_##name_snake){0}; \
        } \
        \
        return (s_##name_snake){ \
            .buf_raw = buf, \
            .len = cnt \
        }; \
    }

DECLARE_ARRAY_TYPE(char, str, Str); // TODO: Getting string "length" includes the NT - this is confusing as hell!

static inline bool IsStrTerminated(const s_str_view str) {
    return !*StrElemView(str, str.len - 1);
}

DECLARE_ARRAY_TYPE(s_str, str_array, StrArray);
DECLARE_ARRAY_TYPE(s_str_view, str_view_array, StrViewArray);

DECLARE_ARRAY_TYPE(t_r32, r32_array, R32Array);
DECLARE_ARRAY_TYPE(t_r64, r64_array, R64Array);

DECLARE_ARRAY_TYPE(t_u8, u8_array, U8Array);
DECLARE_ARRAY_TYPE(t_s8, s8_array, S8Array);
DECLARE_ARRAY_TYPE(t_s16, s16_array, S16Array);
DECLARE_ARRAY_TYPE(t_u16, u16_array, U16Array);
DECLARE_ARRAY_TYPE(t_s32, s32_array, S32Array);
DECLARE_ARRAY_TYPE(t_u32, u32_array, U32Array);
DECLARE_ARRAY_TYPE(t_s64, s64_array, S64Array);
DECLARE_ARRAY_TYPE(t_u64, u64_array, U64Array);

typedef struct {
    t_u8* bytes;
    size_t bit_cnt;
} s_bitset;

int Bitset_IndexOfFirstUnsetBit(const s_bitset bitset); // Returns -1 if there is no unset bit.

static inline void Bitset_AssertValidity(const s_bitset bitset) {
    assert(bitset.bytes);
    assert(bitset.bit_cnt > 0);
}

static inline void Bitset_ActivateBit(const s_bitset bitset, const size_t bit_index) {
    Bitset_AssertValidity(bitset);
    assert(bit_index < bitset.bit_cnt);

    bitset.bytes[bit_index / 8] |= 1 << (bit_index % 8);
}

static inline void Bitset_DeactivateBit(const s_bitset bitset, const size_t bit_index) {
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
    static inline void name_pascal##_ActivateBit(t_##name_snake* const bitset, const size_t bit_index) { \
        Bitset_ActivateBit((s_bitset){*bitset, bit_cnt}, bit_index); \
    } \
    \
    static inline void name_pascal##_DeactivateBit(t_##name_snake* const bitset, const size_t bit_index) { \
        Bitset_DeactivateBit((s_bitset){*bitset, bit_cnt}, bit_index); \
    } \
    \
    static inline bool name_pascal##_IsBitActive(const t_##name_snake* const bitset, const size_t bit_index) { \
        return Bitset_IsBitActive((s_bitset){*bitset, bit_cnt}, bit_index); \
    } \
    \
    static inline int name_pascal##_IndexOfFirstInactiveBit(const t_##name_snake* const bitset) { \
        return Bitset_IndexOfFirstInactiveBit((s_bitset){*bitset, bit_cnt}); \
    }

#endif
