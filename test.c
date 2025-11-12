#define COMMONLIB_IMPLEMENTATION
#define COMMONLIB_REMOVE_PREFIX
#include "commonlib.h"

typedef struct {
    int *items;
    size_t count;
    size_t capacity;
} Darray;

int main(void) {
    Darray da = {0};
    da_append(da, 69);
    da_append(da, 420);
    da_append(da, 1337);

    int second = -1;
    da_remove(da, int, &second, 1);
    log_info("Second element: %d", second);

    da_delete(da, int, 1);
    log_info("Da count: %zu", da.count);
    return 0;
}
