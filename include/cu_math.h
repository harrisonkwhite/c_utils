#pragma once

#include <cmath>
#include <cassert>
#include "cu_mem.h"

struct s_v2 {
    float x = 0.0f;
    float y = 0.0f;

    constexpr s_v2() = default;
    constexpr s_v2(const float x, const float y) : x(x), y(y) {}

    float Dot(const s_v2 other) const {
        return (x * other.x) + (y * other.y);
    }

    float Mag() const {
        return sqrtf((x * x) + (y * y));
    }

    s_v2 NormalizedOrZero() const {
        const float mag = Mag();

        if (mag == 0.0f) {
            return {};
        }

        return {x / mag, y / mag};
    }
};

struct s_v2_s32 {
    t_s32 x = 0;
    t_s32 y = 0;

    constexpr s_v2_s32() = default;
    constexpr s_v2_s32(const t_s32 x, const t_s32 y) : x(x), y(y) {}

    operator s_v2() const {
        return {static_cast<float>(x), static_cast<float>(y)};
    }
};

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
        union {
            u_v3 xyz;

            struct {
                float x;
                float y;
                float z;
            };
        };

        float w;
    };

    struct {
        union {
            u_v3 rgb;

            struct {
                float r;
                float g;
                float b;
            };
        };

        float a;
    };
} u_v4;

struct s_rect {
    float x;
    float y;
    float width;
    float height;

    s_rect() = default;
    s_rect(const float x, const float y, const float width, const float height) : x(x), y(y), width(width), height(height) {}
    s_rect(const s_v2 pos, const s_v2 size) : x(pos.x), y(pos.y), width(size.x), height(size.y) {}

    s_v2 Pos() const {
        return {x, y};
    }

    s_v2 Size() const {
        return {width, height};
    }

    bool operator==(const s_rect& other) const {
        return x == other.x && y == other.y && width == other.width && height == other.height;
    }
};

struct s_rect_s32 {
    t_s32 x;
    t_s32 y;
    t_s32 width;
    t_s32 height;

    s_rect_s32() = default;
    s_rect_s32(const t_s32 x, const t_s32 y, const t_s32 width, const t_s32 height) : x(x), y(y), width(width), height(height) {}
    s_rect_s32(const s_v2_s32 pos, const s_v2_s32 size) : x(pos.x), y(pos.y), width(size.x), height(size.y) {}

    s_v2_s32 Pos() const {
        return {x, y};
    }

    s_v2_s32 Size() const {
        return {width, height};
    }

    operator s_rect() const {
        return {static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height)};
    }

    bool operator==(const s_rect_s32& other) const {
        return x == other.x && y == other.y && width == other.width && height == other.height;
    }
};

struct s_rect_edges {
    float left;
    float top;
    float right;
    float bottom;
};
