#define COMMONLIB_IMPLEMENTATION
#define COMMONLIB_REMOVE_PREFIX
#include "../commonlib.h"

int main(void) {

	ASSERT(c_str_starts_with("Hello, World", "Hello"), "THIS SHOULD BE TRUE!");

	log_info("OK");

	return 0;
}
