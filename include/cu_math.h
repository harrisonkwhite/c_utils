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
    t_r32 x;
    t_r32 y;
} s_v2;

DEF_ARRAY_TYPE(s_v2, v2, V2);

#define V2_ONE (s_v2){1.0f, 1.0f}

typedef struct {
    t_s32 x;
    t_s32 y;
} s_v2_s32;

DEF_ARRAY_TYPE(s_v2_s32, v2_s32, V2S32);

typedef union {
    struct {
        t_r32 x;
        t_r32 y;
        t_r32 z;
    };

    struct {
        t_r32 r;
        t_r32 g;
        t_r32 b;
    };
} u_v3;

DEF_ARRAY_TYPE(u_v3, v3, V3);

typedef union {
    struct {
        union {
            u_v3 xyz;

            struct {
                t_r32 x;
                t_r32 y;
                t_r32 z;
            };
        };

        t_r32 w;
    };

    struct {
        union {
            u_v3 rgb;

            struct {
                t_r32 r;
                t_r32 g;
                t_r32 b;
            };
        };

        t_r32 a;
    };
} u_v4;

DEF_ARRAY_TYPE(u_v4, v4, V4);

static inline t_r32 Lerp(const t_r32 a, const t_r32 b, const t_r32 t) {
    assert(t >= 0.0f && t <= 1.0f);
    return a + ((b - a) * t);
}

static inline s_v2 V2Sum(const s_v2 a, const s_v2 b) {
    return (s_v2){a.x + b.x, a.y + b.y};
}

static inline s_v2_s32 V2S32Sum(const s_v2_s32 a, const s_v2_s32 b) {
    return (s_v2_s32){a.x + b.x, a.y + b.y};
}

static inline s_v2 V2Scaled(const s_v2 v, const t_r32 scalar) {
    return (s_v2){v.x * scalar, v.y * scalar};
}

static inline t_r32 Mag(const s_v2 v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

static inline t_r32 Dot(const s_v2 a, const s_v2 b) {
    return (a.x * b.x) + (a.y * b.y);
}

static inline s_v2 NormalOrZero(const s_v2 v) {
    const t_r32 mag = Mag(v);

    if (mag == 0.0f) {
        return (s_v2){0};
    }

    return (s_v2){v.x / mag, v.y / mag};
}

static inline s_v2 V2Dir(const s_v2 a, const s_v2 b) {
    return NormalOrZero((s_v2){b.x - a.x, b.y - a.y});
}

static inline t_r32 Dist(const s_v2 a, const s_v2 b) {
    s_v2 d = {a.x - b.x, a.y - b.y};
    return Mag(d);
}

static inline t_r32 Dir(const s_v2 v) {
    return atan2f(-v.y, v.x);
}

static inline t_r32 DirAToB(const s_v2 a, const s_v2 b) {
    return Dir((s_v2){b.x - a.x, b.y - a.y});
}

static inline s_v2 LenDir(const t_r32 len, const t_r32 dir) {
    return (s_v2){cosf(dir) * len, -sinf(dir) * len};
}

typedef struct {
    t_r32 x;
    t_r32 y;
    t_r32 width;
    t_r32 height;
} s_rect;

DEF_ARRAY_TYPE(s_rect, rect, Rect);

typedef struct {
    t_s32 x;
    t_s32 y;
    t_s32 width;
    t_s32 height;
} s_rect_s32;

DEF_ARRAY_TYPE(s_rect_s32, rect_s32, RectS32);

typedef struct {
    t_r32 left;
    t_r32 top;
    t_r32 right;
    t_r32 bottom;
} s_rect_edges;

DEF_ARRAY_TYPE(s_rect_edges, rect_edges, RectEdges);

typedef struct {
    t_s32 left;
    t_s32 top;
    t_s32 right;
    t_s32 bottom;
} s_rect_edges_s32;

DEF_ARRAY_TYPE(s_rect_edges_s32, rect_edges_s32, RectEdgesS32);

s_rect GenSpanningRect(const s_rect_array_view rects);

static inline s_v2 RectPos(const s_rect rect) {
    return (s_v2){rect.x, rect.y};
}

static inline s_v2_s32 RectS32Pos(const s_rect_s32 rect) {
    return (s_v2_s32){rect.x, rect.y};
}

static inline s_v2 RectSize(const s_rect rect) {
    return (s_v2){rect.width, rect.height};
}

static inline s_v2_s32 RectS32Size(const s_rect_s32 rect) {
    return (s_v2_s32){rect.width, rect.height};
}

static inline s_v2 RectCenter(const s_rect rect) {
    return (s_v2){rect.x + (rect.width / 2.0f), rect.y + (rect.height / 2.0f)};
}

static inline s_rect RectTranslated(const s_rect rect, const s_v2 trans) {
    return (s_rect){rect.x + trans.x, rect.y + trans.y, rect.width, rect.height};
}

static inline s_rect_s32 RectTranslatedS32(const s_rect_s32 rect, const s_v2_s32 trans) {
    return (s_rect_s32){rect.x + trans.x, rect.y + trans.y, rect.width, rect.height};
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

static inline s_rect_edges_s32 RectEdgesS32Clamped(const s_rect_edges_s32 edges, const s_rect_edges_s32 clamp_edges) {
    return (s_rect_edges_s32){
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

static inline bool IsRangeS32Valid(const s_rect_edges_s32 range, const s_v2_s32 size) {
    assert(size.x > 0 && size.y > 0);

    return range.left >= 0 && range.left < size.x
        && range.right >= 0 && range.right <= size.x
        && range.top >= 0 && range.top < size.y
        && range.bottom >= 0 && range.bottom <= size.y
        && range.left <= range.right
        && range.top <= range.bottom;
}

typedef struct {
    t_r32 elems[4][4];
} s_matrix_4x4;

static inline s_matrix_4x4 IdentityMatrix4x4() {
    s_matrix_4x4 mat = {0};

    *STATIC_ARRAY_2D_ELEM(mat.elems, 0, 0) = 1.0f;
    *STATIC_ARRAY_2D_ELEM(mat.elems, 1, 1) = 1.0f;
    *STATIC_ARRAY_2D_ELEM(mat.elems, 2, 2) = 1.0f;
    *STATIC_ARRAY_2D_ELEM(mat.elems, 3, 3) = 1.0f;

    return mat;
}

static inline s_matrix_4x4 OrthographicMatrix(const t_r32 left, const t_r32 right, const t_r32 bottom, const t_r32 top, const t_r32 near, const t_r32 far) {
    assert(right > left);
    assert(top < bottom);
    assert(far > near);
    assert(near < far);

    s_matrix_4x4 mat = {0};
    *STATIC_ARRAY_2D_ELEM(mat.elems, 0, 0) = 2.0f / (right - left);
    *STATIC_ARRAY_2D_ELEM(mat.elems, 1, 1) = 2.0f / (top - bottom);
    *STATIC_ARRAY_2D_ELEM(mat.elems, 2, 2) = -2.0f / (far - near);
    *STATIC_ARRAY_2D_ELEM(mat.elems, 3, 0) = -(right + left) / (right - left);
    *STATIC_ARRAY_2D_ELEM(mat.elems, 3, 1) = -(top + bottom) / (top - bottom);
    *STATIC_ARRAY_2D_ELEM(mat.elems, 3, 2) = -(far + near) / (far - near);
    *STATIC_ARRAY_2D_ELEM(mat.elems, 3, 3) = 1.0f;

    return mat;
}

static inline void TranslateMatrix4x4(s_matrix_4x4* const mat, const s_v2 trans) {
    assert(mat);

    *STATIC_ARRAY_2D_ELEM(mat->elems, 3, 0) += trans.x;
    *STATIC_ARRAY_2D_ELEM(mat->elems, 3, 1) += trans.y;
}

static inline void ScaleMatrix4x4(s_matrix_4x4* const mat, const t_r32 scalar) {
    assert(mat);

    *STATIC_ARRAY_2D_ELEM(mat->elems, 0, 0) *= scalar;
    *STATIC_ARRAY_2D_ELEM(mat->elems, 1, 1) *= scalar;
    *STATIC_ARRAY_2D_ELEM(mat->elems, 2, 2) *= scalar;
}

#endif
