#define COMMONLIB_IMPLEMENTATION
#include "../commonlib.h"

typedef struct {
    int *items;
    size_t count;
    size_t capacity;
} Dynamic_Array;

int main(void) {
    Dynamic_Array da = {0};

    c_da_append(da, 6);

    for (int i = 0; i < 10; ++i) {
        if (i % 2 == 0) c_da_append(da, i);
    }
    // NOTE: da_shift will make the da loose its ptr, so store the ptr elsewhere if you want to free it later!!!
    int a = c_da_shift(da);

    C_ASSERT(a == 6, "da_shift did not return correct value");

    c_log_info("Even numbers:");
    for (int i = 0; i < da.count; ++i) {
        c_log_info("%d", da.items[i]);
    }

    int third = 0;
    c_log_info("da count before remove: %zu", da.count);
    c_da_remove(da, int, &third, 2);

    c_log_info("Third element: %d", third);
    c_log_info("da count after remove: %zu", da.count);

    int outofbound_value = -1;
    c_da_remove(da, int, &outofbound_value, 50);
    c_log_info("outofbound_value : %d", outofbound_value);
    c_log_info("da count after trying to remove outofbounds_value: %zu", da.count);

    return 0;
}
