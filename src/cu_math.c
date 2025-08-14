#include <cu_math.h>

s_rect GenSpanningRect(const s_rect_array_view rects) {
    s_rect_edges span = {
        RectElemView(rects, 0)->x,
        RectElemView(rects, 0)->y,
        RectElemView(rects, 0)->x + RectElemView(rects, 0)->width,
        RectElemView(rects, 0)->y + RectElemView(rects, 0)->height
    };

    for (t_s32 i = 1; i < rects.elem_cnt; ++i) {
        const s_rect* const rect = RectElemView(rects, i);

        if (rect->x < span.left) {
            span.left = rect->x;
        }
        
        if (rect->y < span.top) {
            span.top = rect->y;
        }
        
        if (rect->x + rect->width > span.right) {
            span.right = rect->x + rect->width;
        }
        
        if (rect->y + rect->height > span.bottom) {
            span.bottom = rect->y + rect->height;
        }
    }

    return (s_rect){
        span.left,
        span.top,
        span.right - span.left,
        span.bottom - span.top
    };
}
