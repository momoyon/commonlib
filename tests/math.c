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


    log_info("MAX(1, 2): %d", MAX(1, 2));
    log_info("MIN(231, 123): %d", MIN(231, 123));

	return 0;
}
