#ifndef CU_MATH_H
#define CU_MATH_H

#include <math.h>
#include <assert.h>

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

typedef struct {
    int x;
    int y;
} s_v2_int;

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

#endif
