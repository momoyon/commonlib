#define COMMONLIB_IMPLEMENTATION
#define COMMONLIB_REMOVE_PREFIX
#include "../commonlib.h"

int main(void) {
    Arena a = arena_make(0);

    int *integer_ptr = (int *)arena_alloc(&a, sizeof(int)+2); // 6-byte allocated

    ASSERT(a.alloced_blocks.count == 1, "RAH");

    *integer_ptr = 69;

    const char *str = arena_alloc_str(a, "Foo: %d", 1337);

    ASSERT(a.alloced_blocks.count == 2, "RAH");
    ASSERT(a.free_blocks.count == 0, "RAH");

    arena_dealloc(&a, integer_ptr);
    ASSERT(a.alloced_blocks.count == 1, "RAH");
    ASSERT(a.free_blocks.count == 1, "RAH");

    int *another_integer_ptr = (int *)arena_alloc(&a, sizeof(int));

    ASSERT(a.alloced_blocks.count == 2, "RAH");
    ASSERT(a.free_blocks.count == 1, "RAH");

    *another_integer_ptr = 100;

    arena_free(&a);

    return 0;
}
