#define COMMONLIB_IMPLEMENTATION
#include "../commonlib.h"

int main(void) {
    int file_size = -1;
    const char *buff = c_read_file(__FILE__, &file_size);
    C_ASSERT(file_size != -1, "We are trying to read this file...");

    c_log_info("`%s` (%d bytes)", buff, file_size);

    return 0;
}
