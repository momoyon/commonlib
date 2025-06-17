#define COMMONLIB_IMPLEMENTATION
#define COMMONLIB_REMOVE_PREFIX
#include "../commonlib.h"

int main(void) {
	String_builder sb = {0};

	sb_append(&sb, "Hello");
	sb_append(&sb, " ");
	sb_append(&sb, "World");

	sb_append_null(&sb);

	log_info("%s", sb.items);
	return 0;
}
