#define COMMONLIB_IMPLEMENTATION
#define COMMONLIB_REMOVE_PREFIX
#include "../commonlib.h"

// on the stack
#define ARR_CAP (1024)
typedef struct {
    int items[ARR_CAP];
    size_t count;
    size_t capacity;
} Stack_array;
typedef Stack_array Array;

typedef struct {
    int *items;
    size_t count;
    size_t capacity;
} Heap_array;

int main(void) {
    Array arr = {0};
    arr_stack_init(arr);
    log_info("STACK:");
    log_info("arr: %zu, %zu ", arr.count, arr.capacity);
    arr_append(arr, 69);
    arr_append(arr, 420);
    arr_append(arr, 1337);
    log_info("arr: %zu, %zu ", arr.count, arr.capacity);

    Heap_array harr = {0};
    arr_heap_init(harr, 2048);
    log_info("HEAP:");
    log_info("harr: %zu, %zu ", harr.count, harr.capacity);
    arr_append(harr, 69);
    arr_append(harr, 420);
    arr_append(harr, 1337);
    log_info("harr: %zu, %zu ", harr.count, harr.capacity);

    int elm = -1;
    size_t i = 1;
    arr_remove(harr, int, &elm, i);

    ASSERT(elm == 420, "This should be correct!");
    ASSERT(harr.count == 2, "This should be correct!");

    log_info("Element %zu: %d", i, elm);
    log_info("harr.count after remove: %zu", harr.count);

    arr_delete(harr, int, 1);
    log_info("harr.count after delete: %zu", harr.count);
    ASSERT(harr.count == 1, "This should be correct!");



    return 0;
}


