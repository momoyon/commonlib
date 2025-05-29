#define DEBUG

#define COMMONLIB_IMPLEMENTATION
#include "../commonlib.h"

int main(void) {
    c_log_info("This is an info log");
    c_log_error("This one is an error log");
    c_log_warning("And this is a warning log");
    c_log_debug("And this is a debug log");

    return 0;
}
