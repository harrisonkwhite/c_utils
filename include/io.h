#ifndef IO_H
#define IO_H

#include <stdbool.h>
#include <stdio.h>
#include "mem.h"

// Using macros here instead of functions so that we still get warnings for incorrect arguments, which are very important.
#define LOG(format, ...) printf(format "\n", ##__VA_ARGS__)
#define LOG_ERROR(format, ...) fprintf(stderr, "\x1b[31mError: \x1b[0m" format "\n", ##__VA_ARGS__)

typedef char t_filename_buf[256];

typedef struct {
    const t_filename_buf* buf;
    int cnt;
} s_filenames;

bool DoesFilenameHaveExt(const char* const filename, const char* const ext);
t_u8* PushEntireFileContents(const char* const file_path, s_mem_arena* const mem_arena, const bool incl_terminating_byte);
bool LoadDirectoryFilenames(s_filenames* const filenames, s_mem_arena* const mem_arena, const char* const dir_param);

#endif
