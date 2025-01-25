#define COMMONLIB_IMPLEMENTATION
#include "../commonlib.h"

int main(void) {
    c_Arena a = c_Arena_make(0);

    c_Arena_free(&a);

    return 0;
}
