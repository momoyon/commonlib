#define COMMONLIB_IMPLEMENTATION
#include "../commonlib.h"

typedef struct {
    int *items;
    size_t count;
    size_t capacity;
} Dynamic_Array;

void log_da(Dynamic_Array da) {
    for (int i = 0; i < da.count; ++i) {
        printf("%d ", da.items[i]);
    }
	printf("\n");
}

int main(void) {
    Dynamic_Array da = {0};

    c_darr_append(da, 6);

    for (int i = 0; i < 10; ++i) {
        if (i % 2 == 0) c_darr_append(da, i);
    }
    // NOTE: darr_shift will make the da loose its ptr, so store the ptr elsewhere if you want to free it later!!!
    int a = c_darr_shift(da);

    C_ASSERT(a == 6, "darr_shift did not return correct value");

    c_log_info("Even numbers:");
    for (int i = 0; i < da.count; ++i) {
        c_log_info("%d", da.items[i]);
    }

    c_log_info(">>UNORDERED:");
    c_log_info("da count before remove: %zu", da.count);
    c_darr_remove_unordered(da, 2);
    c_log_info("da count after remove: %zu", da.count);


	Dynamic_Array da2 = {0};

    for (int i = 0; i < 10; ++i) {
    	c_darr_append(da2, i);
	}

	c_log_info("Before ordered remove:");
	log_da(da2);

	int removing_i = 4;
	// memmove(da2.items + removing_i, da2.items + removing_i+1, (da2.count - removing_i+1) * sizeof(int));
	// da2.count--;
	c_darr_remove(da2, removing_i);

	c_log_info("After ordered element at index %d removed:", removing_i);
	log_da(da2);

    return 0;
}
