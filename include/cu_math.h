#ifndef CU_MATH_H
#define CU_MATH_H

#include <math.h>
#include <assert.h>
#include "cu_mem.h"

#define PI 3.14159265358979323846f
#define TAU 6.28318530717958647692f

#define MIN(x, y) ((x) <= (y) ? (x) : (y))
#define MAX(x, y) ((x) >= (y) ? (x) : (y))

#define ABS(x) (x) < 0 ? -(x) : (x)

#define CLAMP(x, min, max) (((x) < (min)) ? (min) : ((x) > (max) ? (max) : (x)))
#define SIGN(x) ((x) == 0 ? (x) : ((x) > 0 ? 1 : -1));

typedef struct {
    float x;
    float y;
} s_v2;

DECLARE_ARRAY_TYPE(s_v2, v2, V2);

typedef struct {
    int x;
    int y;
} s_v2_int;

DECLARE_ARRAY_TYPE(s_v2_int, v2_int, V2Int);

typedef union {
    struct {
        float x;
        float y;
        float z;
    };

    struct {
        float r;
        float g;
        float b;
    };
} u_v3;

DECLARE_ARRAY_TYPE(u_v3, v3, V3);

typedef union {
    struct {
        float x;
        float y;
        float z;
        float w;
    };

    struct {
        float r;
        float g;
        float b;
        float a;
    };
} u_v4;

DECLARE_ARRAY_TYPE(u_v4, v4, V4);

static inline float Lerp(const float a, const float b, const float t) {
    assert(t >= 0.0f && t <= 1.0f);
    return a + ((b - a) * t);
}

static inline s_v2 V2Sum(const s_v2 a, const s_v2 b) {
    return (s_v2){a.x + b.x, a.y + b.y};
}

static inline s_v2_int V2IntSum(const s_v2_int a, const s_v2_int b) {
    return (s_v2_int){a.x + b.x, a.y + b.y};
}

static inline s_v2 V2Scaled(const s_v2 v, const float scalar) {
    return (s_v2){v.x * scalar, v.y * scalar};
}

static inline float Mag(const s_v2 v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

static inline float Dot(const s_v2 a, const s_v2 b) {
    return (a.x * b.x) + (a.y * b.y);
}

static inline s_v2 NormalOrZero(const s_v2 v) {
    const float mag = Mag(v);

    if (mag == 0.0f) {
        return (s_v2){0};
    }

    return (s_v2){v.x / mag, v.y / mag};
}

static inline s_v2 V2Dir(const s_v2 a, const s_v2 b) {
    return NormalOrZero((s_v2){b.x - a.x, b.y - a.y});
}

static inline float Dist(const s_v2 a, const s_v2 b) {
    s_v2 d = {a.x - b.x, a.y - b.y};
    return Mag(d);
}

static inline float Dir(const s_v2 v) {
    return atan2f(-v.y, v.x);
}

static inline float DirAToB(const s_v2 a, const s_v2 b) {
    return Dir((s_v2){b.x - a.x, b.y - a.y});
}

static inline s_v2 LenDir(const float len, const float dir) {
    return (s_v2){cosf(dir) * len, -sinf(dir) * len};
}

typedef struct {
    float x;
    float y;
    float width;
    float height;
} s_rect;

typedef struct {
    int x;
    int y;
    int width;
    int height;
} s_rect_int;

typedef struct {
    float left;
    float top;
    float right;
    float bottom;
} s_rect_edges;

typedef struct {
    int left;
    int top;
    int right;
    int bottom;
} s_rect_edges_int;

s_rect GenSpanningRect(const s_rect* const rects, const int cnt);

static inline s_v2 RectPos(const s_rect rect) {
    return (s_v2){rect.x, rect.y};
}

static inline s_v2_int RectIntPos(const s_rect_int rect) {
    return (s_v2_int){rect.x, rect.y};
}

static inline s_v2 RectSize(const s_rect rect) {
    return (s_v2){rect.width, rect.height};
}

static inline s_v2_int RectIntSize(const s_rect_int rect) {
    return (s_v2_int){rect.width, rect.height};
}

static inline s_v2 RectCenter(const s_rect rect) {
    return (s_v2){rect.x + (rect.width / 2.0f), rect.y + (rect.height / 2.0f)};
}

static inline s_rect RectTranslated(const s_rect rect, const s_v2 trans) {
    return (s_rect){rect.x + trans.x, rect.y + trans.y, rect.width, rect.height};
}

static inline s_rect_int RectTranslatedInt(const s_rect_int rect, const s_v2_int trans) {
    return (s_rect_int){rect.x + trans.x, rect.y + trans.y, rect.width, rect.height};
}

static inline bool IsPointInRect(const s_v2 pt, const s_rect rect) {
    return pt.x >= rect.x && pt.y >= rect.y && pt.x < rect.x + rect.width && pt.y < rect.y + rect.height;
}

static inline bool DoRectsInters(const s_rect a, const s_rect b) {
    return a.x < b.x + b.width && a.y < b.y + b.height && a.x + a.width > b.x && a.y + a.height > b.y;
}

static inline s_rect_edges RectEdgesClamped(const s_rect_edges edges, const s_rect_edges clamp_edges) {
    return (s_rect_edges){
        MAX(edges.left, clamp_edges.left),
        MAX(edges.top, clamp_edges.top),
        MIN(edges.right, clamp_edges.right),
        MIN(edges.bottom, clamp_edges.bottom)
    };
}

static inline s_rect_edges_int RectEdgesIntClamped(const s_rect_edges_int edges, const s_rect_edges_int clamp_edges) {
    return (s_rect_edges_int){
        MAX(edges.left, clamp_edges.left),
        MAX(edges.top, clamp_edges.top),
        MIN(edges.right, clamp_edges.right),
        MIN(edges.bottom, clamp_edges.bottom)
    };
}

static inline bool IsRangeValid(const s_rect_edges range, const s_v2 size) {
    assert(size.x > 0.0f && size.y > 0.0f);

    return range.left >= 0.0f && range.left < size.x
        && range.right >= 0.0f && range.right <= size.x
        && range.top >= 0.0f && range.top < size.y
        && range.bottom >= 0.0f && range.bottom <= size.y
        && range.left <= range.right
        && range.top <= range.bottom;
}

static inline bool IsRangeIntValid(const s_rect_edges_int range, const s_v2_int size) {
    assert(size.x > 0 && size.y > 0);

    return range.left >= 0 && range.left < size.x
        && range.right >= 0 && range.right <= size.x
        && range.top >= 0 && range.top < size.y
        && range.bottom >= 0 && range.bottom <= size.y
        && range.left <= range.right
        && range.top <= range.bottom;
}

typedef struct {
    float elems[4][4];
} s_matrix_4x4;

static inline s_matrix_4x4 IdentityMatrix4x4() {
    s_matrix_4x4 mat = {0};
    mat.elems[0][0] = 1.0f;
    mat.elems[1][1] = 1.0f;
    mat.elems[2][2] = 1.0f;
    mat.elems[3][3] = 1.0f;

    return mat;
}

static inline s_matrix_4x4 OrthographicMatrix(const float left, const float right, const float bottom, const float top, const float near, const float far) {
    assert(right > left);
    assert(top < bottom);
    assert(far > near);
    assert(near < far);

    s_matrix_4x4 mat = {0};
    mat.elems[0][0] = 2.0f / (right - left);
    mat.elems[1][1] = 2.0f / (top - bottom);
    mat.elems[2][2] = -2.0f / (far - near);
    mat.elems[3][0] = -(right + left) / (right - left);
    mat.elems[3][1] = -(top + bottom) / (top - bottom);
    mat.elems[3][2] = -(far + near) / (far - near);
    mat.elems[3][3] = 1.0f;

    return mat;
}

static inline void TranslateMatrix4x4(s_matrix_4x4* const mat, const s_v2 trans) {
    assert(mat);

    mat->elems[3][0] += trans.x;
    mat->elems[3][1] += trans.y;
}

static inline void ScaleMatrix4x4(s_matrix_4x4* const mat, const float scalar) {
    assert(mat);

    mat->elems[0][0] *= scalar;
    mat->elems[1][1] *= scalar;
    mat->elems[2][2] *= scalar;
}

#endif
