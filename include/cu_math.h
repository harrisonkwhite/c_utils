#pragma once

#include <cmath>
#include <cassert>
#include "cu_mem.h"

struct s_v2 {
    float x;
    float y;

    s_v2() = default;
    s_v2(const float x, const float y) : x(x), y(y) {}

    float Mag() {
        return sqrtf((x * x) + (y * y));
    }

    s_v2 NormalizedOrZero() {
        const float mag = Mag();

        if (mag == 0.0f) {
            return {};
        }

        return {x / mag, y / mag};
    }
};

struct s_rect {
    float x;
    float y;
    float width;
    float height;
};

struct s_rect_edges {
    float left;
    float top;
    float right;
    float bottom;
};
