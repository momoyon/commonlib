#define COMMONLIB_IMPLEMENTATION
#include "../commonlib.h"

int main(void) {
    int file_size = 0;
    const char *buff = c_slurp_file(__FILE__, &file_size);
    
    if (file_size < 0) { return 1; }

    c_log_info("buff: '%s' (%d bytes)", buff, file_size);
    
    // We don't care about leaking memory since we are just exiting right away!
    return 0;
}
