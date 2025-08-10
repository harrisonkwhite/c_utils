#include "cu_io.h"

s_u8_array LoadFileContents(const s_char_array_view file_path, s_mem_arena* const mem_arena, const bool include_terminating_byte) {
    FILE* const fs = fopen(file_path.buf_raw, "rb");

    if (!fs) {
        LOG_ERROR("Failed to open \"%s\"!", file_path.buf_raw);
        return (s_u8_array){0};
    }

    fseek(fs, 0, SEEK_END);
    const size_t file_size = ftell(fs);
    fseek(fs, 0, SEEK_SET);

    const s_u8_array contents = PushU8ArrayToMemArena(mem_arena, include_terminating_byte ? file_size + 1 : file_size);

    if (IS_ZERO(contents)) {
        LOG_ERROR("Failed to reserve memory for the contents of file \"%s\"!", file_path.buf_raw);
        return (s_u8_array){0};
    }

    if (fread(contents.buf_raw, 1, file_size, fs) < file_size) {
        LOG_ERROR("Failed to read the contents of \"%s\"!", file_path.buf_raw);
        return (s_u8_array){0};
    }

    return contents;
}
