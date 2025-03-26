#define COMMONLIB_IMPLEMENTATION
#include "../commonlib.h"

int main(void) {
    c_Arena a = c_arena_make(0);

    c_arena_free(&a);

    return 0;
}
