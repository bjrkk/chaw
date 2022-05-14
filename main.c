#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <string.h>

#include <ctype.h>

#include "util.h"
#include "compile.h"

typedef enum {
    CHAW_FILE_UNK,
    CHAW_FILE_CHT,
    CHAW_FILE_CHK,
} chaw_filetype_t;

char const *filetype_names[] = {
    [CHAW_FILE_UNK] = "unknown",
    [CHAW_FILE_CHT] = "cht",
    [CHAW_FILE_CHK] = "chk",
};

char const *chaw_get_file_extension(char const *str) {
    char const *ext = strrchr(str, '.');
    return ext;
}

chaw_filetype_t chaw_get_filetype(char const *path) {
    char const *ext = chaw_get_file_extension(path);
    char const *chk_exts[] = {
        ".chk", ".3cn", ".3th"
    };
    char const *cht_exts[] = {
        ".cht", ".i"
    };

    for (int i = 0; i < CHAW_COUNTOF(chk_exts); i++) {
        if (chaw_strcasecmp(chk_exts[i], ext) == 0) {
            return CHAW_FILE_CHK;
        }
    }

    for (int i = 0; i < CHAW_COUNTOF(cht_exts); i++) {
        if (chaw_strcasecmp(cht_exts[i], ext) == 0) {
            return CHAW_FILE_CHT;
        }
    }

    return CHAW_FILE_UNK;
}

void usage(void) {
    printf("OVERVIEW: chunky file compiler for kauai");
    printf("USAGE: chaw.exe [options] file...\n");
    printf("OPTIONS:\n");
    printf("  -c --compile      Compiles the given chunky source files. Set by default.\n");
    printf("  -d --decompile    Decompiles the given chunky binary files.\n");
    printf("  -o --output       Sets the output of the compilation/decompilation.\n");
    printf("  -h --help         Prints this text.\n");
}

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        printf("chaw: error: no input files\n");
        usage();
        return 1;
    }

    #define IS_ARG(arg, s) ((arg)[1] == s[1] || strcmp((arg) + 1, s) == 0) 

    size_t files_given = 0;

    bool should_compile = true;
    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];
        if (arg[0] == '-' || arg[0] == '/') {
            if (IS_ARG(arg, "-compile")) {
                should_compile = true;
            } else if (IS_ARG(arg, "-decompile")) {
                should_compile = false;
            } else if (IS_ARG(arg, "-help")) {
                usage();
                return 1;
            } else {
                printf("chaw: error: unknown argument");
                return 1;
            }
        } else {
            files_given++;
            FILE *file = fopen(arg, "rb");
            if (file == NULL) {
                printf("chaw: error: failed to open file '%s'!\n", arg);
                continue;
            }

            chaw_filetype_t filetype;
            if (should_compile) {
                filetype = CHAW_FILE_CHT;
            } else {
                filetype = CHAW_FILE_CHK;
            }
            
            chaw_filetype_t assumed_filetype = chaw_get_filetype(arg);
            if (assumed_filetype == CHAW_FILE_UNK) {
                printf("chaw: warning: assuming '%s' is a %s file\n", arg, filetype_names[filetype]);
            } else {
                filetype = assumed_filetype;
            }
            
            char *file_buffer;
            size_t file_length;

            fseek(file, 0, SEEK_END);
            file_length = ftell(file);
            fseek(file, 0, SEEK_SET);
            
            file_buffer = (char*)malloc(file_length + 1);
            fread(file_buffer, file_length, 1, file);
            fclose(file);

            file_buffer[file_length] = '\0';

            if (should_compile) {
                switch (filetype) {
                case CHAW_FILE_CHT:
                    chaw_compile_cht(file_buffer);
                    break;
                case CHAW_FILE_CHK:
                    printf("chaw: error: attempted to compile a compiled binary\n");
                    return 1;
                default:
                    printf("chaw: error: currently unimplemented\n");
                    return 1;
                }
            } else {
                printf("chaw: error: todo, currently unimplemented\n");
                return 1;
            }

            fclose(file);
        }
    }

    #undef IS_ARG

    if (files_given == 0) {
        printf("chaw: error: no input files\n");
        usage();
        return 1;
    }

    return 0;
}
