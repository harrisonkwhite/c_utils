#ifndef CU_MATH_H
#define CU_MATH_H

#include <assert.h>

#define PI 3.14159265358979323846f
#define TAU 6.28318530717958647692f

#define MIN(x, y) ((x) <= (y) ? (x) : (y))
#define MAX(x, y) ((x) >= (y) ? (x) : (y))

#define ABS(x) (x) < 0 ? -(x) : (x)

#define CLAMP(x, min, max) (((x) < (min)) ? (min) : ((x) > (max) ? (max) : (x)))
#define SIGN(x) ((x) == 0 ? (x) : ((x) > 0 ? 1 : -1));

static inline float Lerp(const float a, const float b, const float t) {
    assert(t >= 0.0f && t <= 1.0f);
    return a + ((b - a) * t);
}

#endif
