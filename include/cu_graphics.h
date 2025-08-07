#ifndef CU_GRAPHICS_H
#define CU_GRAPHICS_H

#include <glad/glad.h>
#include "cu_mem.h"
#include "cu_math.h"

#define GL_VERSION_MAJOR 4
#define GL_VERSION_MINOR 3

#define ASCII_PRINTABLE_MIN ' '
#define ASCII_PRINTABLE_MAX '~'
#define ASCII_PRINTABLE_RANGE_LEN (ASCII_PRINTABLE_MAX - ASCII_PRINTABLE_MIN + 1)

#define WHITE (u_v4){1.0f, 1.0f, 1.0f, 1.0f}
#define RED (u_v4){1.0f, 0.0f, 0.0f, 1.0f}
#define GREEN (u_v4){0.0f, 1.0f, 0.0f, 1.0f}
#define BLUE (u_v4){0.0f, 0.0f, 1.0f, 1.0f}
#define BLACK (u_v4){0.0f, 0.0f, 0.0f, 1.0f}
#define YELLOW (u_v4){1.0f, 1.0f, 0.0f, 1.0f}
#define CYAN (u_v4){0.0f, 1.0f, 1.0f, 1.0f}
#define MAGENTA (u_v4){1.0f, 0.0f, 1.0f, 1.0f}
#define GRAY (u_v4){0.5f, 0.5f, 0.5f, 1.0f}

#define ALIGNMENT_TOP_LEFT (s_v2){0.0f, 0.0f}
#define ALIGNMENT_TOP_CENTER (s_v2){0.5f, 0.0f}
#define ALIGNMENT_TOP_RIGHT (s_v2){1.0f, 0.0f}
#define ALIGNMENT_CENTER_LEFT (s_v2){0.0f, 0.5f}
#define ALIGNMENT_CENTER (s_v2){0.5f, 0.5f}
#define ALIGNMENT_CENTER_RIGHT (s_v2){1.0f, 0.5f}
#define ALIGNMENT_BOTTOM_LEFT (s_v2){0.0f, 1.0f}
#define ALIGNMENT_BOTTOM_CENTER (s_v2){0.5f, 1.0f}
#define ALIGNMENT_BOTTOM_RIGHT (s_v2){1.0f, 1.0f}

typedef GLuint t_gl_id;

DECLARE_ARRAY_TYPE(t_gl_id, gl_id, GLID);

typedef enum {
    ek_gl_resource_type_texture,
    ek_gl_resource_type_shader_prog,
    ek_gl_resource_type_vert_array,
    ek_gl_resource_type_vert_buf,
    ek_gl_resource_type_elem_buf,
    ek_gl_resource_type_framebuffer,

    eks_gl_resource_type_cnt
} e_gl_resource_type;

DECLARE_ARRAY_TYPE(e_gl_resource_type, gl_resource_type, GLResourceType);

typedef struct {
    s_gl_id_array ids;
    s_gl_resource_type_array res_types;

    int res_used;
    int res_limit;
} s_gl_resource_arena;

static inline void AssertGLResourceArenaValidity(const s_gl_resource_arena* const res_arena) {
    assert(res_arena);

    GLIDArray_AssertValidity(&res_arena->ids);
    assert(res_arena->ids.len == res_arena->res_limit);

    GLResourceTypeArray_AssertValidity(&res_arena->res_types);
    assert(res_arena->res_types.len == res_arena->res_limit);

    assert(res_arena->res_used >= 0);
    assert(res_arena->res_limit > 0);
    assert(res_arena->res_used <= res_arena->res_limit);
}

typedef struct {
    s_gl_id_array gl_ids;
    s_v2_int_array sizes;

    int cnt;
} s_texture_group;

static inline void AssertTextureGroupValidity(const s_texture_group* const tex_group) {
    assert(tex_group);

    GLIDArray_AssertValidity(&tex_group->gl_ids);
    assert(tex_group->gl_ids.len == tex_group->cnt);

    V2IntArray_AssertValidity(&tex_group->sizes);
    assert(tex_group->sizes.len == tex_group->cnt);

    assert(tex_group->cnt > 0);

    for (int i = 0; i < tex_group->cnt; i++) {
        assert(glIsTexture(*GLIDArray_Get(&tex_group->gl_ids, i)));

        const s_v2_int size = *V2IntArray_Get(&tex_group->sizes, i);
        assert(size.x > 0 && size.y > 0);
    }
}

typedef struct {
    const t_byte* rgba_px_data;
    s_v2_int tex_size;
} s_texture_info;

static inline void AssertTextureInfoValidity(const s_texture_info* const tex_info) {
    assert(tex_info);

    assert(tex_info->rgba_px_data);
    assert(tex_info->tex_size.x > 0 && tex_info->tex_size.y > 0);
}

typedef s_texture_info (*t_gen_texture_info_func)(const int tex_index, s_mem_arena* const mem_arena);

typedef struct {
    int line_height;

    s_v2_int chr_offsets[ASCII_PRINTABLE_RANGE_LEN];
    s_v2_int chr_sizes[ASCII_PRINTABLE_RANGE_LEN];
    int chr_advances[ASCII_PRINTABLE_RANGE_LEN];
} s_font_arrangement_info;

static inline void AssertFontArrangementInfoValidity(const s_font_arrangement_info* const arrangement_info) {
    assert(arrangement_info);

    assert(arrangement_info->line_height > 0);

    for (int i = 1; i < ASCII_PRINTABLE_RANGE_LEN; i++) {
        assert(arrangement_info->chr_sizes[i].x > 0 && arrangement_info->chr_sizes[i].y > 0);
        assert(arrangement_info->chr_advances[i] >= 0);
    }
}

typedef s_v2_int t_tex_chr_positions[ASCII_PRINTABLE_RANGE_LEN];

typedef struct {
    const s_font_arrangement_info* arrangement_infos;
    const t_gl_id* tex_gl_ids;
    const s_v2_int* tex_sizes;
    const t_tex_chr_positions* tex_chr_positions;

    int cnt;
} s_font_group;

static inline void AssertFontGroupValidity(const s_font_group* const font_group) {
    assert(font_group);

    assert(font_group->arrangement_infos);
    assert(font_group->tex_gl_ids);
    assert(font_group->tex_sizes);
    assert(font_group->tex_chr_positions);
    assert(font_group->cnt > 0);

    for (int i = 0; i < font_group->cnt; i++) {
        assert(glIsTexture(font_group->tex_gl_ids[i]));
        assert(font_group->tex_sizes[i].x > 0 && font_group->tex_sizes[i].y > 0);
        AssertFontArrangementInfoValidity(&font_group->arrangement_infos[i]);
    }
}

typedef struct {
    const char* file_path;
    int height;
} s_font_info;

static inline void AssertFontInfoValidity(const s_font_info* const info) {
    assert(info);

    assert(info->file_path);
    assert(info->height > 0);
}

typedef struct {
    s_gl_id_array gl_ids;
} s_shader_prog_group;

static inline void AssertShaderProgGroupValidity(const s_shader_prog_group* const prog_group) {
    assert(prog_group);

    GLIDArray_AssertValidity(&prog_group->gl_ids);

    for (int i = 0; i < prog_group->gl_ids.len; i++) {
        assert(glIsProgram(*GLIDArray_Get(&prog_group->gl_ids, i)));
    }
}

typedef struct {
    bool is_srcs;

    union {
        struct {
            const char* vs_src;
            const char* fs_src;
        };

        struct {
            const char* vs_file_path;
            const char* fs_file_path;
        };
    };
} s_shader_prog_gen_info;

static inline void AssertShaderProgGenInfoValidity(const s_shader_prog_gen_info* const gen_info) {
    assert(gen_info);

    assert(gen_info->vs_src);
    assert(gen_info->fs_src);
}

typedef enum {
    ek_shader_prog_uniform_value_type_int,
    ek_shader_prog_uniform_value_type_float,
    ek_shader_prog_uniform_value_type_v2,
    ek_shader_prog_uniform_value_type_v3,
    ek_shader_prog_uniform_value_type_v4,
    ek_shader_prog_uniform_value_type_mat4x4,
} e_shader_prog_uniform_value_type;

typedef struct {
    e_shader_prog_uniform_value_type type;

    union {
        int as_int;
        float as_float;
        s_v2 as_v2;
        u_v3 as_v3;
        u_v4 as_v4;
        s_matrix_4x4 as_mat4x4;
    };
} s_shader_prog_uniform_value;

typedef struct {
    s_gl_id_array vert_array_gl_ids;
    s_gl_id_array vert_buf_gl_ids;
    s_gl_id_array elem_buf_gl_ids;

    int cnt;
} s_renderables;

static inline void AssertRenderablesValidity(const s_renderables* const renderables) {
    assert(renderables);

    GLIDArray_AssertValidity(&renderables->vert_array_gl_ids);
    assert(renderables->vert_array_gl_ids.len == renderables->cnt);

    GLIDArray_AssertValidity(&renderables->vert_buf_gl_ids);
    assert(renderables->vert_buf_gl_ids.len == renderables->cnt);

    GLIDArray_AssertValidity(&renderables->elem_buf_gl_ids);
    assert(renderables->elem_buf_gl_ids.len == renderables->cnt);

    assert(renderables->cnt > 0);

    for (int i = 0; i < renderables->cnt; i++) {
        const t_gl_id va_gl_id = *GLIDArray_Get(&renderables->vert_array_gl_ids, i);
        assert(glIsVertexArray(va_gl_id));

        const t_gl_id vb_gl_id = *GLIDArray_Get(&renderables->vert_buf_gl_ids, i);
        assert(glIsBuffer(vb_gl_id));

        const t_gl_id eb_gl_id = *GLIDArray_Get(&renderables->elem_buf_gl_ids, i);
        assert(glIsBuffer(eb_gl_id));
    }
}

typedef struct {
    s_gl_id_array fb_gl_ids;
    s_gl_id_array fb_tex_gl_ids;
    s_v2_int* sizes;

    int cnt;
} s_surface_group;

static inline void AssertSurfaceGroupValidity(const s_surface_group* const surfs) {
    assert(surfs);

    GLIDArray_AssertValidity(&surfs->fb_gl_ids);
    assert(surfs->fb_gl_ids.len == surfs->cnt);

    GLIDArray_AssertValidity(&surfs->fb_tex_gl_ids);
    assert(surfs->fb_tex_gl_ids.len == surfs->cnt);

    assert(surfs->sizes);

    assert(surfs->cnt > 0);

    for (int i = 0; i < surfs->cnt; i++) {
        assert(glIsFramebuffer(*GLIDArray_Get(&surfs->fb_gl_ids, i)));
        assert(glIsTexture(*GLIDArray_Get(&surfs->fb_tex_gl_ids, i)));
        assert(surfs->sizes[i].x > 0 && surfs->sizes[i].y > 0);
    }
}

bool InitGLResourceArena(s_gl_resource_arena* const res_arena, s_mem_arena* const mem_arena, const int res_limit);
void CleanGLResourceArena(s_gl_resource_arena* const res_arena);
s_gl_id_array ReserveGLIDs(s_gl_resource_arena* const res_arena, const int cnt, const e_gl_resource_type res_type);

s_texture_info GenTextureInfoFromFile(const char* const file_path, s_mem_arena* const mem_arena);
s_texture_group GenTextures(const int tex_cnt, const t_gen_texture_info_func gen_tex_info_func, s_gl_resource_arena* const gl_res_arena, s_mem_arena* const mem_arena, s_mem_arena* const temp_mem_arena);
s_rect_edges TextureCoords(const s_rect_int src_rect, const s_v2_int tex_size);

s_font_group GenFonts(const int font_cnt, const s_font_info* const font_infos, s_gl_resource_arena* const gl_res_arena, s_mem_arena* const mem_arena, s_mem_arena* const temp_mem_arena);
s_v2* PushStrChrRenderPositions(s_mem_arena* const mem_arena, const char* const str, const s_font_group* const fonts, const int font_index, const s_v2 pos, const s_v2 alignment);
bool LoadStrCollider(s_rect* const rect, const char* const str, const s_font_group* const fonts, const int font_index, const s_v2 pos, const s_v2 alignment, s_mem_arena* const temp_mem_arena);

s_shader_prog_group GenShaderProgs(const int prog_cnt, const s_shader_prog_gen_info* const prog_gen_infos, s_gl_resource_arena* const gl_res_arena, s_mem_arena* const temp_mem_arena);

void GenRenderable(t_gl_id* const va_gl_id, t_gl_id* const vb_gl_id, t_gl_id* const eb_gl_id, const float* const vert_buf, const size_t vert_buf_size, const unsigned short* const elem_buf, const size_t elem_buf_size, const int* const vert_attr_lens, const int vert_attr_cnt);

s_surface_group GenSurfaces(const int surf_cnt, const s_v2_int* const surf_sizes, s_gl_resource_arena* const gl_res_arena, s_mem_arena* const mem_arena);
bool ResizeSurface(s_surface_group* const surfs, const int surf_index, const s_v2_int size);

static inline bool IsOriginValid(const s_v2 orig) {
    return orig.x >= 0.0f && orig.x <= 1.0f && orig.y >= 0.0f && orig.y <= 1.0f;
}

static inline bool IsTextureCoordsValid(const s_rect_edges coords) {
    return coords.left >= 0.0f && coords.top >= 0.0f && coords.right <= 1.0f && coords.bottom <= 1.0f;
}

static bool IsSrcRectValid(const s_rect_int src_rect, const s_v2_int tex_size) {
    assert(tex_size.x > 0 && tex_size.y > 0);
    return src_rect.x >= 0 && src_rect.width > 0 && src_rect.x + src_rect.width <= tex_size.x
        && src_rect.y >= 0 && src_rect.height > 0 && src_rect.y + src_rect.height <= tex_size.y;
}

static inline bool IsColorValid(const u_v4 col) {
    return col.r >= 0.0f && col.r <= 1.0f
        && col.g >= 0.0f && col.g <= 1.0f
        && col.b >= 0.0f && col.b <= 1.0f
        && col.a >= 0.0f && col.a <= 1.0f;
}

static inline bool IsColorRGBValid(const u_v3 col) {
    return col.r >= 0.0f && col.r <= 1.0f
        && col.g >= 0.0f && col.g <= 1.0f
        && col.b >= 0.0f && col.b <= 1.0f;
}

static inline bool IsStrAlignmentValid(const s_v2 alignment) {
    return alignment.x >= 0.0f && alignment.x <= 1.0f
        && alignment.y >= 0.0f && alignment.y <= 1.0f;
}

#endif
