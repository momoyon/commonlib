#define COMMONLIB_IMPLEMENTATION
#include "../commonlib.h"

int main(int argc, char **argv) {
    const char *program = c_shift_args(argv, argc);

    printf("program: %s\n", program);
    printf("After:\n");
    printf("argc: %d\n", argc);
    printf("argv: %s\n", argv[0]);
    return 0;
}
