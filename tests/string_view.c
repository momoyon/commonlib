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


    /// Conversions

    // STRING
    String_view foo = SV("Hello Mister");

    char *cstr = sv_to_cstr(foo);

    log_info("cstr from sv: "SV_FMT" -> %s", SV_ARG(foo), cstr);

    C_FREE(cstr);

    // INT
    int i_count = -1;
    String_view int_sv = SV("-39");
    int64 i = sv_to_int(int_sv, &i_count, 10);
    ASSERT(i_count != -1, "We know `int_sv` contains an int!");

    log_info("int from sv: "SV_FMT" -> %d (len of int str: %d)", SV_ARG(int_sv), (int)i, i_count);

    int_sv = SV("1337  junk");
    i = sv_to_int(int_sv, &i_count, 10);
    ASSERT(i_count != -1, "We know `int_sv` contains an int!");

    log_info("int from sv: "SV_FMT" -> %d (len of int str: %d)", SV_ARG(int_sv), (int)i, i_count);

    int_sv = SV("baka1337  junk");
    i = sv_to_int(int_sv, &i_count, 10);
    ASSERT(i_count == -1, "We know `int_sv` does not contain an int!");

    log_info("int from sv: "SV_FMT" -> %d (len of int str: %d)", SV_ARG(int_sv), (int)i, i_count);

    // UINT
    int u_count = -1;
    String_view uint_sv = SV("1240");
    uint64 u = sv_to_uint(uint_sv, &u_count, 10);
    ASSERT(u_count != -1, "We know `uint_sv` contains an uint!");

    log_info("uint from sv: "SV_FMT" -> %d (len of uint str: %d)", SV_ARG(uint_sv), (int)u, u_count);

    uint_sv = SV("00000012312strin");
    u = sv_to_uint(uint_sv, &u_count, 10);
    ASSERT(u_count != -1, "We know `uint_sv` contains an uint!");

    log_info("uint from sv: "SV_FMT" -> %d (len of uint str: %d)", SV_ARG(uint_sv), (int)u, u_count);

    uint_sv = SV("burger 123123.23423  junk");
    u = sv_to_uint(uint_sv, &u_count, 10);
    ASSERT(u_count == -1, "We know `uint_sv` does not contain an uint!");

    log_info("uint from sv: "SV_FMT" -> %d (len of uint str: %d)", SV_ARG(uint_sv), (int)u, u_count);

    // FLOAT
    int f_count = -1;
    String_view float_sv = SV("-2342.045BLAH345");
    float f = sv_to_float(float_sv, &f_count);
    ASSERT(f_count != -1, "We know `float_sv` contains an float!");

    log_info("float from sv: "SV_FMT" -> %f (len of float str: %d)", SV_ARG(float_sv), f, f_count);

    float_sv = SV("1.013012312310f");
    f = sv_to_float(float_sv, &f_count);
    ASSERT(f_count != -1, "We know `float_sv` contains an float!");
    log_info("float from sv: "SV_FMT" -> %f (len of float str: %d)", SV_ARG(float_sv), f, f_count);

    float_sv = SV("T.013012312310f");
    f = sv_to_float(float_sv, &f_count);
    ASSERT(f_count == -1, "We know `float_sv` does not contain an float!");
    log_info("float from sv: "SV_FMT" -> %f (len of float str: %d)", SV_ARG(float_sv), f, f_count);

    return 0;
}
