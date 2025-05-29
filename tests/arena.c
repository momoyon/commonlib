#define COMMONLIB_IMPLEMENTATION
#include "../commonlib.h"

int main(void) {
    c_Arena a = c_arena_make(0);

    const char *str = c_arena_alloc_str(a, "Foo: %d", 69);

    c_log_info("str: %s", str);

    c_arena_free(&a);

    return 0;
}
