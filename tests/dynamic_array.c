#define COMMONLIB_IMPLEMENTATION
#include "../commonlib.h"

typedef struct {
    int *items;
    size_t count;
    size_t capacity;
} Dynamic_Array;

int main(void) {
    Dynamic_Array da = {0};

    for (int i = 0; i < 100; ++i) {
        if (i % 2 == 0) c_da_append(da, i);
    }

    c_log_info("Even numbers:");
    for (int i = 0; i < da.count; ++i) {
        c_log_info("%d", da.items[i]);
    }

    return 0;
}
