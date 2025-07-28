#ifndef IO_H
#define IO_H

#include <stdbool.h>
#include "mem.h"

typedef char t_filename_buf[256];

typedef struct {
    const t_filename_buf* buf;
    int cnt;
} s_filenames;

void Log(const char* const format, ...);
void LogError(const char* const format, ...);

bool DoesFilenameHaveExt(const char* const filename, const char* const ext);
t_u8* PushEntireFileContents(const char* const file_path, s_mem_arena* const mem_arena, const bool incl_terminating_byte);
bool LoadDirectoryFilenames(s_filenames* const filenames, s_mem_arena* const mem_arena, const char* const dir_param);

#endif
