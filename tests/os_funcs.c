#define COMMONLIB_IMPLEMENTATION
#define COMMONLIB_REMOVE_PREFIX
#include "../commonlib.h"

int main(void) {
    log_info("%s", os_file_exists(__FILE__) ? "This file exists!" : "This file doesn't exist!?");
    return 0;
}

