#ifndef _COMMONLIB_H_
#define _COMMONLIB_H_
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>


// typedefs
typedef unsigned int uint;
typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef float  float32;
typedef double float64;

typedef wchar_t wchar;

typedef const char*  cstr;
typedef const wchar* wstr;

#define ASSERT(condition, msg) do {\
        if (!(condition)) {\
            fprintf(stderr, "%s:%d:0 [ASSERTION FAILED] %s: %s", __FILE__, __LINE__, #condition, msg);\
            exit(1);\
        }\
    } while (0)

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof(arr[0]))

#define STRUCT(name) typedef struct name name
#define ENUM(name)   typedef enum name name

// Struct pre-decls

STRUCT(Arena);

//
// OS
//

void os_get_timedate(Arena* a);
bool os_file_exists(cstr filename);

//
// Logging
//

// FLUSH_ON_LOG - define this macro to fflush() on every call to log() not defined by default.

#define log_info(fmt, ...)     printf("%s "fmt"\n", "[INFO]", ##__VA_ARGS__)
#define log_warning(fmt, ...)  fprintf(stderr, "%s "fmt"\n", "[WARNING]", ##__VA_ARGS__)
#define log_error(fmt, ...)    fprintf(stderr, "%s "fmt"\n", "[ERROR]", ##__VA_ARGS__)

//
// File
//

// reads entire file and gives back the string holding the contents. (caller must be responsible for freeing the string!)
const char* slurp_file(const char* filename, bool* success);
// void touch_file_if_doesnt_exist(cstr file);

//
// ### Allocators ###
//

//
// Arena
//

#define ARENA_BUFF_INITIAL_SIZE (1024*4)

struct Arena {
  void* buff;
  uint64 buff_size;
  void* ptr;
};

// pass size 0 to get ARENA_BUFF_INITIAL_SIZE
Arena Arena_make(size_t size);
void* Arena_alloc(Arena* a, size_t size);
void Arena_reset(Arena* a);
void Arena_free(Arena* a);

#define Arena_alloc_str(a, fmt, ...)  Arena_alloc(&(a), sizeof(char)*stbsp_snprintf((a).ptr, (int)((a).buff_size - ((uint8*)(a).ptr - (uint8*)(a).buff)), (fmt), __VA_ARGS__)+1)
#define Arena_alloc_wstr(a, fmt, ...) Arena_alloc(&a, sizeof(char)*wprintf(a.ptr, a.buff_size - ((uint8*)a.ptr - (uint8*)a.buff), (fmt), __VA_ARGS__)+1)

//
// String
//

bool cstr_to_wstr(Arena* warena, cstr str);

//
// String view
//

typedef struct {
  cstr data;
  size_t count;
} String_view;

#define SV_FMT "%.*s"
#define SV_ARG(sv) (int)sv.count, sv.data

#define SV(cstr) (String_view){.data = cstr, strlen(cstr)}

void sv_print_dumb(String_view sv);
String_view sv_from_cstr(const char* cstr); // Actually just use SV(cstr) macro...
String_view sv_lpop(String_view* sv, uint32 n);
String_view sv_lpop_until_predicate(String_view* sv, int(*predicate)(int));
String_view sv_rpop_until_predicate(String_view* sv, int(*predicate)(int));
String_view sv_lpop_until_char(String_view* sv, char ch);
String_view sv_rpop_until_char(String_view* sv, char ch);
void sv_lremove(String_view* sv, size_t n);
void sv_rremove(String_view* sv, size_t n);
void sv_lremove_until_char(String_view* sv, char ch);
void sv_rremove_until_char(String_view* sv, char ch);
void sv_lremove_until_char_after(String_view* sv, char ch);
void sv_rremove_until_char_after(String_view* sv, char ch);
void sv_ltrim(String_view* sv);
void sv_rtrim(String_view* sv);
void sv_trim(String_view* sv);
char* sv_to_cstr(String_view sv);
int32  sv_to_int(String_view sv);
uint64 sv_to_uint64(String_view sv);
uint8 sv_to_uint8_hex(String_view sv);
void*  sv_to_ptr(String_view sv);
float32 sv_to_float(String_view sv);
bool sv_contains_char(String_view sv, char ch);
bool sv_is_hex_numbers(String_view sv);
bool sv_equals(String_view sv1, String_view sv2);

//
// Args
//

cstr shift_args(int* argc, char*** argv);

#endif /* _COMMONLIB_H_ */

//////////////////////////////////////////////////
#ifdef COMMONLIB_IMPLEMENTATION
#include <string.h>
#include <errno.h>
#include <stdlib.h>

// My things implementation:

//
// OS
//

#if defined(_WIN32) || defined(__CYGWIN__)
void os_get_timedate(Arena* a) {
    (void)a;
    ASSERT(false, "Unimplemented!");
}

bool os_file_exists(cstr filename) {
    (void) filename;
    ASSERT(false, "Unimplemented!");
    return false;
}

#elif defined(__linux__)
void os_get_timedate(Arena* a) {
    (void)a;
    ASSERT(false, "Unimplemented!");
}

bool os_file_exists(cstr filename) {
    struct stat buf;
    ASSERT(false, "Unimplemented!");
    return stat(filename, &buf) == 0;
}
#endif

// simple and dirty way to have defering in C (not recommended to use!)
#define defer(ret_val) \
  result = ret_val;\
  goto defer

const char* slurp_file(const char* filename, bool* success) {
  FILE* f = fopen(filename, "rb");
  char* result = NULL;

  if (f == NULL){
    log_error("slurp_file::fopen(\"%s\", \"rb\") -> %s\n", filename, strerror(errno));
    defer(NULL);
  }

  if (fseek(f, 0, SEEK_END) < 0) {
    log_error("slurp_file::fseek(f, 0, SEEK_END) -> %s\n", filename, strerror(errno));
    defer(NULL);
  }

  size_t fsize = ftell(f);

  if (fsize == (size_t)-1){
    log_error("slurp_file::ftell(f) -> %s\n", filename, strerror(errno));
    defer(NULL);
  }

  result = malloc(sizeof(char)*(fsize+1));

  if (result == NULL){
    log_error("slurp_file::malloc(%zu) -> %s\n", sizeof(char)*fsize, strerror(errno));
    defer(NULL);
  }

  if (fseek(f, 0, SEEK_SET) < 0) {
    log_error("slurp_file::fseek(f, 0, SEEK_SET) -> %s\n", filename, strerror(errno));
    defer(NULL);
  }

  if (fread((char*)result, sizeof(char), fsize, f) != fsize){
    log_error("slurp_file::fread(result, %d, 1, f) -> %s\n", fsize, strerror(errno));
    defer(NULL);
  }

  // set null-terminator
  result[fsize] = '\0';

 defer:
  if (f) fclose(f);
  *success = result != NULL;
  return result;
}

// void touch_file_if_doesnt_exist(cstr filename) {
//     if (os_file_exists(filename)) return;
//     FILE* file = fopen(filename, "w");
//     if (file)
//         fclose(file);
// }

//
// ### Allocators ###
//

// Arena

Arena Arena_make(size_t size) {
  Arena res = {0};
  res.buff_size = size == 0 ? ARENA_BUFF_INITIAL_SIZE : size;
  res.buff = malloc(res.buff_size);
  res.ptr = res.buff;

  ASSERT(res.buff, "Malloc failed?");

  return res;
}

void* Arena_alloc(Arena* a, size_t size) {
  ASSERT(a->buff, "Bro pass an initialized arena!");

  void* res = a->ptr;
  a->ptr = (uint8*)a->ptr + size;

  // TODO: realloc buff with greater buff_size
  size_t diff = (size_t)((uint8*)a->ptr - (uint8*)a->buff);
  if (diff > a->buff_size) {
    log_info("Arena resized from %u to %u", a->buff_size, a->buff_size*2);
    a->buff_size *= 2;
    a->buff = realloc(a->buff, a->buff_size);
    a->ptr = (uint8*)a->buff + diff;
    res = a->ptr;
    a->ptr = (uint8*)a->ptr + size;
  }
  /* ASSERT((size_t)((uint8*)a->ptr - (uint8*)a->buff) <= a->buff_size); */

  return res;
}

void Arena_reset(Arena* a) {
  a->ptr = a->buff;
}

void Arena_free(Arena* a) {
  free(a->buff);
}

//
// String
//

bool cstr_to_wstr(Arena* warena, cstr str) {
  if (warena->buff_size <= 0) {
    log_error("Arena size is 0!!!");
    return false;
  }

  size_t count = strlen(str)+1;
  size_t ret = mbstowcs((wchar*)warena->buff,
		  str,
		  count);

  if (ret == (uint)-1) {
    log_error("Encountered an invalid multibyte character!");
    return false;
  } else if (ret == count) {
    log_error("The wide-character string isn't null-terminated!");
    return false;
  }
  return true;
}

//
// String view
//

void sv_print_dumb(String_view sv){
  for (size_t i = 0; i < (size_t)sv.count; ++i){
    putc(*(sv.data+i), stdout);
  }
}

String_view sv_from_cstr(const char* cstr){
  return (String_view){
    .data = cstr,
    .count = strlen(cstr),
  };
}

String_view sv_lpop(String_view* sv, uint32 n) {
  String_view res = {0};
  if (sv->count < n) return res;
  res.data = sv->data;
  res.count = n;

  sv->data += n;
  sv->count -= n;

  return res;
}

String_view sv_lpop_until_predicate(String_view* sv, int(*predicate)(int)){
  const char* old_sv_data = sv->data;
  while (sv->count > 0 && !predicate(*sv->data)){
    sv->data++;
    sv->count--;
  }

  return (String_view){
    .data = sv->data - (sv->data - old_sv_data),
    .count = (sv->data - old_sv_data),
  };
}

String_view sv_rpop_until_predicate(String_view* sv, int(*predicate)(int)){
  size_t old_sv_count = sv->count;
  while (sv->count > 0 && !predicate(*(sv->data+sv->count-1))){
    sv->count--;
  }

  return (String_view){
    .data = sv->data + sv->count,
    .count = old_sv_count - sv->count,
  };
}

String_view sv_lpop_until_char(String_view* sv, char ch){
  const char* old_sv_data = sv->data;
  while (sv->count > 0 && *sv->data != ch){
    sv->data++;
    sv->count--;
  }

  return (String_view){
    .data = sv->data - (sv->data - old_sv_data),
    .count = (sv->data - old_sv_data),
  };
}

String_view sv_rpop_until_char(String_view* sv, char ch){
  size_t old_sv_count = sv->count;
  while (sv->count > 0 && *(sv->data+sv->count-1) != ch){
    sv->count--;
  }

  return (String_view){
    .data = sv->data + sv->count,
    .count = old_sv_count - sv->count,
  };
}

void sv_lremove(String_view* sv, size_t n){
  if (n > sv->count) n = sv->count;

  sv->data += n;
  sv->count -= n;
}

void sv_rremove(String_view* sv, size_t n){
  if (n > sv->count)
    sv->count = 0;
  else
    sv->count -= n;
}

void sv_lremove_until_char(String_view* sv, char ch){
  while (sv->count > 0 && *sv->data != ch){
    sv->data++;
    sv->count--;
  }
}

void sv_rremove_until_char(String_view* sv, char ch){
  while (sv->count > 0 && *(sv->data+sv->count-1) != ch){
    sv->count--;
  }
}

void sv_lremove_until_char_after(String_view* sv, char ch){
  while (sv->count > 0 && *(sv->data-1) != ch){
    sv->data++;
    sv->count--;
  }
}

void sv_rremove_until_char_after(String_view* sv, char ch){
  while (sv->count > 0 && *(sv->data+sv->count) != ch){
    sv->count--;
  }
}

void sv_ltrim(String_view* sv){
  while (sv->count > 0 && isspace(*sv->data)){
    sv->data++;
    sv->count--;
  }
}

void sv_rtrim(String_view* sv){
  while (sv->count > 0 && isspace(*(sv->data+sv->count-1))){
    sv->count--;
  }
}

void sv_trim(String_view* sv){
  sv_ltrim(sv);
  sv_rtrim(sv);
}

char* sv_to_cstr(String_view sv){
  char* res = (char*)calloc(1, sizeof(char)*sv.count);
  memcpy(res, sv.data, sv.count);
  return res;
}

// TODO: check for failure of conversion. returns 0/0.0 on failure, so just check if the str contains zero.
int32 sv_to_int(String_view sv) {
  char* str = sv_to_cstr(sv);
  int32 res = atoi(str);
  free(str);
  return res;
}

uint64 sv_to_uint64(String_view sv) {
  char* str = sv_to_cstr(sv);
  uint64 res = (uint64)atoll(str);
  free(str);
  return res;
}

uint8 sv_to_uint8_hex(String_view sv) {
  char* str = sv_to_cstr(sv);
  char* end = str + sv.count;
  uint8 res = (uint8)strtol(str, &end, 16);
  free(str);
  return res;
}

float32 sv_to_float(String_view sv) {
  char* str = sv_to_cstr(sv);
  float32 res = (float32)atof(str);
  free(str);
  return res;
}

void*  sv_to_ptr(String_view sv) {
  char* str = sv_to_cstr(sv);
  char* end = NULL;
  void* res = (void*)strtoull(str, &end, 16);
  free(str);
  return res;
}

bool sv_contains_char(String_view sv, char ch){
  for (size_t i = 0; i < sv.count; ++i){
    if (sv.data[i] == ch) return true;
  }
  return false;
}

bool sv_is_hex_numbers(String_view sv) {
  char hex[] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '0', 'a', 'b', 'c', 'd', 'e', 'f',
    'A', 'B', 'C', 'D', 'E', 'F'
  };
  bool found = false;
  for (size_t i = 0; i < sv.count; ++i) {
    char c = sv.data[i];
    for (size_t j = 0; j < ARRAY_LEN(hex); ++j) {
      if (hex[j] == c) {
	found = true;
      }
    }
  }

  return found;
}

bool sv_equals(String_view sv1, String_view sv2) {
  if (sv1.count != sv2.count) return false;
  for (size_t i = 0; i < sv1.count; ++i) {
    if (sv1.data[i] != sv2.data[i]) {
      return false;
    }
  }

  return true;
}


//
// Args
//

cstr shift_args(int* argc, char*** argv) {
  if (*argc <= 0) return NULL;

  cstr res = *(argv)[0];
  *argv = (*argv) + 1;
  *argc = (*argc) - 1;
  return res;
}

#endif
