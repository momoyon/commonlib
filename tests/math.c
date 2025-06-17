#define COMMONLIB_IMPLEMENTATION
#define COMMONLIB_REMOVE_PREFIX
#include "../commonlib.h"

int main(void) {

	int i = 11;
	log_info("i: %d", i);
	i = clampi(i, 0, 10);

	log_info("clamped to 0 ~ 10: %d", i);

	float f = -0.5f;
	log_info("f: %f", f);
	f = clampi(f, 0.f, 1.f);

	log_info("clamped to 0.0 ~ 1.0: %f", f);

	return 0;
}
