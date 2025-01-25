#define NOB_IMPLEMENTATION
#include "nob.h"

#define CC "gcc"
#define CFLAGS "-Wextra", "-Wall", "-Wno-char-subscripts"

#define TESTS_DIR "./tests/"

Nob_Fd open_fd_formatted_for_write(Nob_String_Builder *sb, const char *prefix, Nob_String_View name_sv, const char *suffix) {
    sb->count = 0;
    nob_sb_append_cstr(sb, prefix);
    nob_sb_append_cstr(sb, nob_temp_sv_to_cstr(name_sv));
    nob_sb_append_cstr(sb, ".");
    nob_sb_append_cstr(sb, suffix);
    return nob_fd_open_for_write(sb->items);
}

Nob_Fd open_fd_formatted_for_read(Nob_String_Builder *sb, const char *prefix, Nob_String_View name_sv, const char *suffix) {
    sb->count = 0;
    nob_sb_append_cstr(sb, prefix);
    nob_sb_append_cstr(sb, nob_temp_sv_to_cstr(name_sv));
    nob_sb_append_cstr(sb, ".");
    nob_sb_append_cstr(sb, suffix);
    return nob_fd_open_for_read(sb->items);
}

// TODO: Implement
bool check_test_outputs(Nob_String_View test_name_sv) {
    NOB_ASSERT(0 && "Unimplemented!");
}

void usage(int log_level, const char *program) {
    nob_log(log_level, "Usage: %s [subcmd]", program);
}

void help(int log_level, const char *program) {
    nob_log(log_level, "");
    nob_log(log_level, "Subcommands:");
    nob_log(log_level, "    help - Prints this help message.");
    nob_log(log_level, "    build - Build the tests.");
    nob_log(log_level, "    run - Runs the tests.");
}

int main(int argc, char *argv[]) {
    // NOB_GO_REBUILD_URSELF(argc, argv);

    const char *program = nob_shift(argv, argc);

    if (argc <= 0) {
        nob_log(NOB_ERROR, "Please provide a subcommand!");
        usage(NOB_ERROR, program);
        help(NOB_INFO, program);
        return 1;
    }

    const char *subcmd = nob_shift(argv, argc);

    Nob_Cmd cmd = {0};
    Nob_File_Paths all_files = {0};
    if (!nob_read_entire_dir(TESTS_DIR, &all_files)) return 1;

    Nob_File_Paths c_files = {0};

    for (int i = 0; i < all_files.count; ++i) {
        if (nob_sv_end_with(nob_sv_from_cstr(all_files.items[i]), ".c")) {
            nob_da_append(&c_files, all_files.items[i]);
        }
    }

    // TODO: Detect if there isnt a corresponding *.exe file for *.c
    Nob_File_Paths exe_files = {0};
    for (int i = 0; i < all_files.count; ++i) {
        if (nob_sv_end_with(nob_sv_from_cstr(all_files.items[i]), ".exe")) {
            nob_da_append(&exe_files, all_files.items[i]);
        }
    }

    if (strcmp(subcmd, "help") == 0) {
        help(NOB_INFO, program);
        return 0;
    } else if (strcmp(subcmd, "build") == 0) {
        nob_log(NOB_INFO, "Building tests...");
        for (int i = 0; i < c_files.count; ++i) {
            const char *file = c_files.items[i];

            cmd.count = 0;

            Nob_String_Builder file_without_suffix = {0};
            nob_sb_append_cstr(&file_without_suffix, file);

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
    } else if (strcmp(subcmd, "run") == 0) {
        nob_log(NOB_INFO, "Running tests...");
        for (int i = 0; i < exe_files.count; ++i) {
            const char *file = exe_files.items[i];

            Nob_String_View base_name_sv = nob_sv_from_cstr(file);

            base_name_sv = nob_sv_chop_by_delim(&base_name_sv, '.');

            cmd.count = 0;

            Nob_String_Builder sb = {0};
            nob_sb_append_cstr(&sb, TESTS_DIR);
            nob_sb_append_cstr(&sb, "/");
            nob_sb_append_cstr(&sb, file);
            // TODO: Pass arguments to program
            nob_cmd_append(&cmd, sb.items);

            Nob_String_Builder fd_sb = {0};
            Nob_Fd fdin  = open_fd_formatted_for_read(&fd_sb,  TESTS_DIR, base_name_sv, "in");
            if (fdin == NOB_INVALID_FD) return 1;
            Nob_Fd fdout = open_fd_formatted_for_write(&fd_sb, TESTS_DIR, base_name_sv, "out");
            if (fdout == NOB_INVALID_FD) return 1;
            Nob_Fd fderr = open_fd_formatted_for_write(&fd_sb, TESTS_DIR, base_name_sv, "err");
            if (fderr == NOB_INVALID_FD) return 1;

            nob_cmd_run_sync_redirect(cmd, (Nob_Cmd_Redirect) {
                    .fdin =  &fdin,
                    .fdout = &fdout,
                    .fderr = &fderr,
            });

            check_test_outputs(base_name_sv);

            nob_sb_free(sb);
            // Closing all the files
            nob_fd_close(fdin);
            nob_fd_close(fdout);
            nob_fd_close(fderr);
        }
    } else {
        nob_log(NOB_ERROR, "Invalid subcommand '%s'", subcmd);
        usage(NOB_ERROR, program);
        help(NOB_ERROR, program);
        return 1;
    }


    return 0;
}
