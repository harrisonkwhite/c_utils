#include "cu_io.h"

#if defined(_WIN32)
#include <windows.h>
#else
#include <dirent.h>
#endif

bool DoesFilenameHaveExt(const s_char_array_view filename, const s_char_array_view ext) {
    assert(IsStrTerminated(filename));
    assert(IsStrTerminated(ext));

    const char* const ext_actual = strrchr(filename.buf_raw, '.');

    if (!ext_actual) {
        return false;
    }

    return strcmp(ext.buf_raw, ext_actual) == 0;
}

bool LoadDirFilenames(s_filename_buf_array* const filename_bufs, s_mem_arena* const mem_arena, const s_char_array_view dir_param) {
    assert(IS_ZERO(*filename_bufs));
    assert(IsStrTerminated(dir_param));

    filename_bufs->buf_raw = (t_filename_buf*)(mem_arena->buf + mem_arena->offs);

#if defined(_WIN32)
    char search_path[MAX_PATH];
    snprintf(search_path, MAX_PATH, "%s\\*", dir_param.buf_raw);

    WIN32_FIND_DATAA find_data;
    HANDLE find = FindFirstFileA(search_path, &find_data);

    if (find == INVALID_HANDLE_VALUE) {
        return false;
    }

    do {
        t_filename_buf* const filename = PushToMemArena(mem_arena, sizeof(t_filename_buf), ALIGN_OF(t_filename_buf));

        if (!filename) {
            FindClose(find);
            return false;
        }

        strncpy(*filename, find_data.cFileName, sizeof(*filename));

        filename_bufs->elem_cnt++;
    } while (FindNextFileA(find, &find_data));

    FindClose(find);
#else
    DIR* const dir = opendir(dir_param.buf_raw);

    if (!dir) {
        return false;
    }

    const struct dirent* dir_entry;

    while ((dir_entry = readdir(dir)) != NULL) {
        t_filename_buf* const filename = PushToMemArena(mem_arena, sizeof(t_filename_buf), ALIGN_OF(t_filename_buf));

        if (!filename) {
            closedir(dir);
            return false;
        }

        strncpy(*filename, dir_entry->d_name, sizeof(*filename));

        filename_bufs->elem_cnt++;
    }

    closedir(dir);
#endif

    return true;
}

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
