#include "io.h"

#include <stdio.h>
#include <stdarg.h>
#if defined(_WIN32)
#include <windows.h>
#else
#include <dirent.h>
#endif

void Log(const char* const format, ...) {
    va_list args;
    va_start(args, format);

    vprintf(format, args);
    printf("\n");

    va_end(args);
}

void LogError(const char* const format, ...) {
    va_list args;
    va_start(args, format);

    fprintf(stderr, "Error: ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");

    va_end(args);
}

bool DoesFilenameHaveExt(const char* const filename, const char* const ext) {
    assert(filename);
    assert(ext);

    const char* const ext_actual = strrchr(filename, '.');

    if (!ext_actual) {
        return false;
    }

    return strcmp(ext_actual, ext) == 0;
}

t_u8* PushEntireFileContents(const char* const file_path, s_mem_arena* const mem_arena, const bool incl_terminating_byte) {
    assert(file_path);
    assert(mem_arena && IsMemArenaValid(mem_arena));

    FILE* const fs = fopen(file_path, "rb");

    if (!fs) {
        fprintf(stderr, "Failed to open \"%s\"!\n", file_path);
        return NULL;
    }

    fseek(fs, 0, SEEK_END);
    const size_t file_size = ftell(fs);
    fseek(fs, 0, SEEK_SET);

    t_u8* const contents = MEM_ARENA_PUSH_TYPE_CNT(mem_arena, t_u8, incl_terminating_byte ? (file_size + 1) : file_size);

    if (!contents) {
        return NULL;
    }

    const size_t read_cnt = fread(contents, 1, file_size, fs);
    
    if (read_cnt != file_size) {
        fprintf(stderr, "Failed to read the contents of \"%s\"!\n", file_path);
        return NULL;
    }

    return contents;
}

bool LoadDirectoryFilenames(s_filenames* const filenames, s_mem_arena* const mem_arena, const char* const dir_param) {
    assert(filenames && IS_ZERO(*filenames));
    assert(mem_arena && IsMemArenaValid(mem_arena));
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
            RewindMemArena(mem_arena, mem_arena_init_offs);
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
