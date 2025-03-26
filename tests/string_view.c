#define COMMONLIB_IMPLEMENTATION
#define COMMONLIB_REMOVE_PREFIX
#include "../commonlib.h"

int main(void) {

    String_view main = SV("Hello, World!");

    String_view multiline_comment = SV("/*Blah Blah\nBlah Blah2\nTHis is the end*/THIS SHOULD NOT BE VISIBLE");

    String_view mc = sv_lpop_until_string(&multiline_comment, "*/");

    String_view a = sv_lpop_until_char(&main, ' ');

    sv_lremove(&main, 1);

    String_view b = main;

    log_info("main: '"SV_FMT"'", SV_ARG(main));
    log_info("a: '"SV_FMT"'", SV_ARG(a));
    log_info("b: '"SV_FMT"'", SV_ARG(b));

    log_info("mc: '"SV_FMT"'", SV_ARG(mc));


    String_view foo = SV("Hello Mister");

    char *cstr = sv_to_cstr(foo);

    log_info("cstr from sv: "SV_FMT" -> %s", SV_ARG(foo), cstr);

    C_FREE(cstr);

    return 0;
}
