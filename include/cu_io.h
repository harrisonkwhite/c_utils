#ifndef CU_IO_H
#define CU_IO_H

#include <stdbool.h>
#include <stdio.h>
#include "cu_mem.h"

#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define MKDIR(path) mkdir(path, 0755)
#endif

#define ANSI_ESC "\x1b"

#define ANSI_RESET ANSI_ESC "[0m"

#define ANSI_BOLD ANSI_ESC "[1m"
#define ANSI_DIM ANSI_ESC "[2m"
#define ANSI_UNDERLINE ANSI_ESC "[4m"
#define ANSI_REVERSED ANSI_ESC "[7m"

#define ANSI_FG_BLACK ANSI_ESC "[30m"
#define ANSI_FG_BBLACK ANSI_ESC "[90m"
#define ANSI_FG_RED ANSI_ESC "[31m"
#define ANSI_FG_BRED ANSI_ESC "[91m"
#define ANSI_FG_GREEN ANSI_ESC "[32m"
#define ANSI_FG_BGREEN ANSI_ESC "[92m"
#define ANSI_FG_YELLOW ANSI_ESC "[33m"
#define ANSI_FG_BYELLOW ANSI_ESC "[93m"
#define ANSI_FG_BLUE ANSI_ESC "[34m"
#define ANSI_FG_BBLUE ANSI_ESC "[94m"
#define ANSI_FG_MAGENTA ANSI_ESC "[35m"
#define ANSI_FG_BMAGENTA ANSI_ESC "[95m"
#define ANSI_FG_CYAN ANSI_ESC "[36m"
#define ANSI_FG_BCYAN ANSI_ESC "[96m"
#define ANSI_FG_WHITE ANSI_ESC "[37m"
#define ANSI_FG_BWHITE ANSI_ESC "[97m"

#define ANSI_BG_BLACK ANSI_ESC "[40m"
#define ANSI_BG_BBLACK ANSI_ESC "[100m"
#define ANSI_BG_RED ANSI_ESC "[41m"
#define ANSI_BG_BRED ANSI_ESC "[101m"
#define ANSI_BG_GREEN ANSI_ESC "[42m"
#define ANSI_BG_BGREEN ANSI_ESC "[102m"
#define ANSI_BG_YELLOW ANSI_ESC "[43m"
#define ANSI_BG_BYELLOW ANSI_ESC "[103m"
#define ANSI_BG_BLUE ANSI_ESC "[44m"
#define ANSI_BG_BBLUE ANSI_ESC "[104m"
#define ANSI_BG_MAGENTA ANSI_ESC "[45m"
#define ANSI_BG_BMAGENTA ANSI_ESC "[105m"
#define ANSI_BG_CYAN ANSI_ESC "[46m"
#define ANSI_BG_BCYAN ANSI_ESC "[106m"
#define ANSI_BG_WHITE ANSI_ESC "[47m"
#define ANSI_BG_BWHITE ANSI_ESC "[107m"

// NOTE: Using macros here instead of functions so that there are still warnings for incorrect arguments.
#define LOG(format, ...) printf(format "\n", ##__VA_ARGS__)
#define LOG_WARNING(format, ...) fprintf(stderr, ANSI_BOLD ANSI_FG_YELLOW "Warning: " ANSI_RESET format "\n", ##__VA_ARGS__)
#define LOG_ERROR(format, ...) fprintf(stderr, ANSI_BOLD ANSI_FG_RED "Error: " ANSI_RESET format "\n", ##__VA_ARGS__)
#define LOG_ERROR_SPECIAL(prefix, format, ...) fprintf(stderr, ANSI_BOLD ANSI_FG_BRED prefix " Error: " ANSI_RESET); \
    fprintf(stderr, format "\n", ##__VA_ARGS__)
#define LOG_SUCCESS(format, ...) fprintf(stderr, ANSI_BOLD ANSI_FG_GREEN "Success: " ANSI_RESET format "\n", ##__VA_ARGS__)

typedef char t_filename_buf[256];

DEF_ARRAY_TYPE(t_filename_buf, filename_buf, FilenameBuf);

bool DoesFilenameHaveExt(const s_char_array_view filename, const s_char_array_view ext);
bool LoadDirFilenames(s_filename_buf_array* const filename_bufs, s_mem_arena* const mem_arena, const s_char_array_view dir_param);

s_u8_array LoadFileContents(const s_char_array_view file_path, s_mem_arena* const mem_arena, const bool include_terminating_byte);

static inline s_char_array LoadFileContentsAsStr(const s_char_array_view file_path, s_mem_arena* const mem_arena) {
    const s_u8_array contents = LoadFileContents(file_path, mem_arena, true);
    return (s_char_array){.buf_raw = (char*)contents.buf_raw, .elem_cnt = contents.elem_cnt};
}

#endif
