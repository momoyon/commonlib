#define NOB_IMPLEMENTATION
#include "nob.h"

#define CC "gcc"
#define CFLAGS "-Wextra", "-Wall", "-Wno-char-subscripts"

#define TESTS_DIR "./tests/"

int main(int argc, char *argv[]) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    Nob_Cmd cmd = {0};

    Nob_File_Paths files = {0};
    if (!nob_read_entire_dir(TESTS_DIR, &files)) return 1;

    for (int i = 0; i < files.count; ++i) {
        const char *file = files.items[i];

        // Ignore files starting with '.'
        if (file[0] == '.') continue;

        cmd.count = 0;

        Nob_String_Builder file_without_suffix = {0};
        nob_sb_append_cstr(&file_without_suffix, file);

        // Ignore non *.c files
        if (!nob_sv_end_with(nob_sb_to_sv(file_without_suffix), ".c")) {
            nob_sb_free(file_without_suffix);
            continue;
        }

        file_without_suffix.count -= 2;
        nob_sb_append_null(&file_without_suffix);

        Nob_String_Builder sb = {0};
        nob_sb_append_cstr(&sb, TESTS_DIR);
        nob_sb_append_cstr(&sb, "/");
        nob_sb_append_cstr(&sb, file_without_suffix.items);
        nob_sb_append_null(&sb);


        Nob_String_Builder sb2 = {0};
        nob_sb_append_cstr(&sb2, TESTS_DIR);
        nob_sb_append_cstr(&sb2, "/");
        nob_sb_append_cstr(&sb2, file);
        nob_sb_append_null(&sb2);

        nob_cmd_append(&cmd, CC, CFLAGS, "-o", sb.items, sb2.items);

        if (!nob_cmd_run_sync(cmd)) return 1;
        nob_sb_free(sb);
        nob_sb_free(sb2);
    }
    return 0;
}
