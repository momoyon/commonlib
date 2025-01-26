#define COMMONLIB_IMPLEMENTATION
#include "../commonlib.h"

int main(void) {
    bool success = false;
    const char *buff = c_slurp_file(__FILE__, &success);
    
    if (!success) { return 1; }

    c_log_info("buff: '%s'", buff);
    
    // We don't care about leaking memory since we are just exiting right away!
    return 0;
}
