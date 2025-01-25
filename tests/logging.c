#define COMMONLIB_IMPLEMENTATION
#include "../commonlib.h"

int main(void) {
    c_log_info("This is an info log");
    c_log_error("This one is an error log");
    c_log_warning("And this is a warning log");

    return 0;
}
