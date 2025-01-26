
#define COMMONLIB_IMPLEMENTATION
#include "../commonlib.h"

int main(void) {
    bool success = false;
    const char *buff = c_slurp_file(__FILE__, &success);
    
    if (!success) { return 1; }

    c_log_info("buff: '%s'", buff);
    
    C_FREE(buff);

    return 0;
}
