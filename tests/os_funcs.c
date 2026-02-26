#define COMMONLIB_REMOVE_PREFIX
#define COMMONLIB_IMPLEMENTATION
#include "../commonlib.h"

int main(void) {
    log_info("%s", os_file_exists(__FILE__) ? "This file exists!" : "This file doesn't exist!?");

    // String_array files = os_list_files(".");
    //
    // log_debug("%s", "os_list_file(\".\")");
    // for (int i = 0; i < files.count; ++i) {
    //     log_info("FILE: %s", files.items[i]);
    //
    //     free(files.items[i]);
    // }
    //
    // darr_free(files);
    return 0;
}

