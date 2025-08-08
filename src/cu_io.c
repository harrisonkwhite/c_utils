#include "cu_io.h"

#if defined(_WIN32)
#include <windows.h>
#else
#include <dirent.h>
#endif

#if 0
bool DoesFilenameHaveExt(const char* const filename, const char* const ext) {
    const char* const ext_actual = strrchr(filename, '.');

    if (!ext_actual) {
        return false;
    }

    return strcmp(ext_actual, ext) == 0;
}
#endif

s_u8_array LoadFileContents(const s_str_view file_path, s_mem_arena* const mem_arena, const bool include_terminating_byte) {
    FILE* const fs = fopen(file_path.buf_raw, "rb");

    if (!fs) {
        LOG_ERROR("Failed to open \"%s\"!", file_path.buf_raw);
        return (s_u8_array){0};
    }

    fseek(fs, 0, SEEK_END);
    const size_t file_size = ftell(fs);
    fseek(fs, 0, SEEK_SET);

    const s_u8_array contents = PushU8Array(mem_arena, include_terminating_byte ? file_size + 1 : file_size);

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

#if 0
bool LoadDirectoryFilenames(s_filenames* const filenames, s_mem_arena* const mem_arena, const char* const dir_param) {
    assert(filenames && IS_ZERO(*filenames));
    MemArena_AssertValidity(mem_arena);
    assert(dir_param);

    const size_t mem_arena_init_offs = mem_arena->offs;

#if defined(_WIN32)
    char search_path[MAX_PATH];
    snprintf(search_path, MAX_PATH, "%s\\*", dir_param);

    WIN32_FIND_DATAA find_data;
    HANDLE find = FindFirstFileA(search_path, &find_data);

    if (find == INVALID_HANDLE_VALUE) {
        return false;
    }

    do {
        t_filename_buf* const buf = MEM_ARENA_PUSH_TYPE(mem_arena, t_filename_buf);

        if (!buf) {
            FindClose(ha);
            ZERO_OUT(*filenames);
            RewindMemArena(mem_arena, mem_arena_init_offs);
            return false;
        }

        strncpy((char*)buf, find_data.cFileName, sizeof(*buf));

        if (!filenames->buf) {
            filenames->buf = buf;
        }

        filenames->cnt++;
    } while (FindNextFileA(find, &find_data));

    FindClose(find);
#else
    DIR* const dir = opendir(dir_param);

    if (!dir) {
        return false;
    }

    const struct dirent* dir_entry;

    while ((dir_entry = readdir(dir)) != NULL) {
        t_filename_buf* const buf = MEM_ARENA_PUSH_TYPE(mem_arena, t_filename_buf);

        if (!buf) {
            closedir(dir);
            ZERO_OUT(*filenames);
            MemArena_Rewind(mem_arena, mem_arena_init_offs);
            return false;
        }

        strncpy((char*)buf, dir_entry->d_name, sizeof(*buf));

        if (!filenames->buf)
            filenames->buf = buf;

        filenames->cnt++;
    }

    closedir(dir);
#endif

    return true;
}
#endif
