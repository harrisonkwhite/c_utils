#pragma once

#include <cstring>
#include <cassert>
#include <cstdlib>
#include <type_traits>

using t_s8 = char;
using t_u8 = unsigned char;
using t_s16 = short;
using t_u16 = unsigned short;
using t_s32 = int;
using t_u32 = unsigned int;
using t_s64 = long long;
using t_u64 = unsigned long long;

template<typename tp_type>
static constexpr int Min(const tp_type& a, const tp_type& b) {
    return a <= b ? a : b;
}

template<typename tp_type>
static constexpr int Max(const tp_type& a, const tp_type& b) {
    return a >= b ? a : b;
}

constexpr size_t Kilobytes(const size_t x) { return (static_cast<size_t>(1) << 10) * x; }
constexpr size_t Megabytes(const size_t x) { return (static_cast<size_t>(1) << 20) * x; }
constexpr size_t Gigabytes(const size_t x) { return (static_cast<size_t>(1) << 30) * x; }
constexpr size_t Terabytes(const size_t x) { return (static_cast<size_t>(1) << 40) * x; }

constexpr size_t BitsToBytes(size_t x) { return (x + 7) / 8; }
constexpr size_t BytesToBits(size_t x) { return x * 8; }

static constexpr bool IsPowerOfTwo(const size_t n) {
    return n > 0 && (n & (n - 1)) == 0;
}

static constexpr bool IsAlignmentValid(const size_t n) {
    return n > 0 && IsPowerOfTwo(n);
}

static constexpr size_t AlignForward(const size_t n, const size_t alignment) {
    return (n + alignment - 1) & ~(alignment - 1);
}

static inline size_t IndexFrom2D(const size_t x, const size_t y, const size_t width) {
    assert(x < width);
    return (width * y) + x;
}

template<typename tp_type>
class c_array {
public:
    c_array() = default;
    c_array(tp_type* const buf, const int len) : m_buf(buf), m_len(len) {}

    tp_type* Raw() const {
        return m_buf;
    }

    int Len() const {
        return m_len;
    }

    bool IsEmpty() const {
        return m_len == 0;
    }

    tp_type& operator[](const int index) const {
        assert(index >= 0 && index < m_len);
        return m_buf[index];
    }

    c_array Slice(const int beg, const int end) const {
        assert(beg >= 0 && beg <= m_len);
        assert(end >= 0 && end <= m_len);
        assert(beg <= end);
        return {m_buf + beg, end - beg};
    }

private:
    tp_type* m_buf = nullptr;
    int m_len = 0;
};

template<typename tp_type, int tp_len>
class c_static_array {
public:
    c_static_array(const tp_type (&arr)[tp_len]) {
        for (int i = 0; i < tp_len; i++) {
            m_buf[i] = arr[i];
        }
    }

    int Len() const {
        return tp_len;
    }

    tp_type& operator[](const int index) {
        assert(index >= 0 && index < tp_len);
        return m_buf[index];
    }

    const tp_type& operator[](const int index) const {
        assert(index >= 0 && index < tp_len);
        return m_buf[index];
    }

    c_array<tp_type> Array() {
        return {m_buf, tp_len};
    }

    c_array<const tp_type> Array() const {
        return {m_buf, tp_len};
    }

private:
    tp_type m_buf[tp_len] = {};
};

template<typename tp_type>
class c_stack {
public:
    c_stack() = default;
    c_stack(const c_array<tp_type> arr, const int height = 0) : m_arr(arr), m_height(height) {}

    int Capacity() {
        return m_arr.Len();
    }

    int Height() {
        return m_height;
    }

    bool IsEmpty() {
        return m_height == 0;
    }

    bool IsFull() {
        return m_height == Capacity();
    }

    tp_type& operator[](const int index) const {
        assert(index >= 0 && index < m_height);
        return m_arr[index];
    }

    void Push(const tp_type& val) {
        assert(!IsFull());

        m_arr[m_height] = val;
        m_height++;
    }

    tp_type& Pop() {
        assert(!IsEmpty());

        m_height--;
        return m_arr[m_height];
    }

private:
    c_array<tp_type> m_arr;
    int m_height;
};

class c_bitset {
public:
    c_bitset() = default;

    c_bitset(const c_array<t_u8> bytes, const size_t bit_cnt) : m_bytes(bytes), m_bit_cnt(bit_cnt) {
        assert(bit_cnt <= 8 * bytes.Len());
    }

    bool IsBitActive(const size_t index) {
        assert(index < m_bit_cnt);
        return m_bytes[index / 8] & BitMask(index);
    }

    void SetBit(const size_t index) {
        assert(index < m_bit_cnt);
        m_bytes[index / 8] |= BitMask(index);
    }

    void UnsetBit(const size_t index) {
        assert(index < m_bit_cnt);
        m_bytes[index / 8] &= ~BitMask(index);
    }

    void ShiftLeft(int amount, const bool rot = false) {
        assert(amount >= 0);

        while (amount >= 0) {
            const int iter_shift_amount = Min(amount, 8);

            t_u8 carry = 0;

            for (int i = 0; i < m_bytes.Len(); i++) {
                t_u8& byte = m_bytes[i];
                const size_t byte_bit_cnt = m_bit_cnt - (8 * i);
                const t_u8 lower_mask = (1 << iter_shift_amount) - 1;
                const t_u8 upper_mask = lower_mask << (byte_bit_cnt - iter_shift_amount);
                const t_u8 carry_old = carry;
                carry = (byte & upper_mask) >> (byte_bit_cnt - iter_shift_amount);
                byte = (byte << iter_shift_amount) | carry_old;
            }

            if (rot) {
                m_bytes[0] |= carry;
            }

            amount -= iter_shift_amount;
        }
    }

    void WriteBitStr(const c_array<char> str_chrs) {
        assert(str_chrs.Len() >= m_bit_cnt + 1);

        for (size_t i = 0; i < m_bit_cnt; i++) {
            str_chrs[i] = IsBitActive(i) ? '1' : '0';
        }
    }

private:
    t_u8 BitMask(const size_t index) {
        return 1 << (index % 8);
    }

    c_array<t_u8> m_bytes;
    size_t m_bit_cnt;
};

/*template<typename tp_type>
struct s_binary_tree {
    s_binary_tree(const c_array<tp_type> node_vals) : m_node_vals(node_vals) {
        assert(IsPowerOfTwo(node_vals.Len()));
    }

    bool IsLeaf() {
        return m_node_vals.Len() == 1;
    }

    s_binary_tree<tp_type> Left() {
        assert(!IsLeaf());
        return {
            m_node_vals.Slice(1, (m_node_vals.Len() - 1) / 2)
        };
    }

    s_binary_tree<tp_type> Right() {
        assert(!IsLeaf());
        return {
            m_node_vals.Slice(1 + ((m_node_vals.Len() - 1) / 2), m_node_vals.Len())
        };
    }

    bool Contains(const tp_type& val) const {
        if (m_node_vals[0] == val) {
            return true;
        }

        if (IsLeaf()) {
            return false;
        }

        return val < m_node_vals[0] ? Left().Contains(val) : Right().Contains(val);
    }

private:
    c_array<tp_type> m_node_vals;
};*/

class c_mem_arena {
public:
    bool Init(const size_t size) {
        assert(!m_buf);

        m_buf = static_cast<t_u8*>(calloc(size, 1));

        if (!m_buf) {
            //LOG_ERROR("Failed to initialise memory arena of size %zu bytes!", size);
            return false;
        }

        m_size = size;

        return true;
    }

    void Clean() {
        assert(m_buf);

        free(m_buf);
        m_buf = nullptr;
        m_size = 0;
        m_offs = 0;
    }

    void* PushRaw(const size_t size, const size_t alignment) {
        const size_t offs_aligned = AlignForward(m_offs, alignment);
        const size_t offs_next = offs_aligned + size;

        if (offs_next > m_size) {
            //LOG_ERROR("Failed to push %zu bytes to memory arena!", size);
            return nullptr;
        }

        m_offs = offs_next;

        return m_buf + offs_aligned;
    }

    template<typename tp_type>
    tp_type* Push() {
        static_assert(std::is_trivially_destructible_v<tp_type>);

        tp_type* const ptr = static_cast<tp_type*>(PushRaw(sizeof(tp_type), alignof(tp_type)));

        if (ptr) {
            new (ptr) tp_type();
        }

        return ptr;
    } 

private:
    t_u8* m_buf = nullptr;
    size_t m_size = 0;
    size_t m_offs = 0;
};

template<typename tp_type>
c_array<tp_type> PushArrayToMemArena(c_mem_arena& arena, const int cnt) {
    static_assert(std::is_trivially_destructible_v<tp_type>);

    void* const mem = arena.PushRaw(sizeof(tp_type) * cnt, alignof(tp_type));

    if (!mem) {
        return {};
    }

    tp_type* const arr_raw = reinterpret_cast<tp_type*>(mem);

    for (size_t i = 0; i < cnt; i++) {
        new (&arr_raw[i]) tp_type();
    }

    return {arr_raw, cnt};
}

template<typename tp_type>
bool IsSorted(const c_array<const tp_type> arr) {
    for (int i = 0; i < arr.Len() - 1; i++) {
        if (arr[i] >= arr[i + 1]) {
            return false;
        }
    }

    return true;
}

template<typename tp_type>
void BubbleSort(const c_array<tp_type> arr) {
    bool sorted;

    do {
        sorted = true;

        for (int i = 0; i < arr.Len() - 1; i++) {
            if (arr[i] >= arr[i + 1]) {
                const tp_type next_old = arr[i + 1];
                arr[i + 1] = arr[i];
                arr[i] = next_old;

                sorted = false;
                break;
            }
        }
    } while (!sorted);
}

template<typename tp_type>
bool BinarySearch(const c_array<const tp_type> arr, const tp_type& elem) {
    assert(IsSorted(arr));

    if (arr.Len() == 0) {
        return false;
    }

    const tp_type& mid = elem[arr.Len() / 2];

    if (mid == elem) {
        return true;
    }

    return elem < mid ? BinarySearch(arr.Slice(0, arr.Len() / 2), elem) : BinarySearch(arr.Slice((arr.Len() / 2) + 1, arr.Len()), elem);
}
