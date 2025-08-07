#include <cu_math.h>

s_rect GenSpanningRect(const s_rect* const rects, const int cnt) {
    assert(rects);
    assert(cnt > 0);

    s_rect_edges span = {
        rects[0].x,
        rects[0].y,
        rects[0].x + rects[0].width,
        rects[0].y + rects[0].height
    };

    for (int i = 1; i < cnt; ++i) {
        const s_rect* const r = &rects[i];

        if (r->x < span.left) {
            span.left = r->x;
        }
        
        if (r->y < span.top) {
            span.top = r->y;
        }
        
        if (r->x + r->width > span.right) {
            span.right = r->x + r->width;
        }
        
        if (r->y + r->height > span.bottom) {
            span.bottom = r->y + r->height;
        }
    }

    return (s_rect){span.left, span.top, span.right - span.left, span.bottom - span.top};
}
