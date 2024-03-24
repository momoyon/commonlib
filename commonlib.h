#ifndef _STDLIB_H_
#define _STDLIB_H_
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>

// typedefs
typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef float  real32;
typedef double real64;

typedef const char* cstr;

#define ASSERT(condition) if (!(condition)) fprintf(stderr, "%s:%d:0: Assertion failed: %s\n", __FILE__, __LINE__, #condition); exit(1)

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof(arr[0]))

// FLUSH_ON_LOG - define this macro to fflush() on every call to log() not defined by default.

// logging
typedef enum {
  LOG_INFO = 0,
  LOG_ERROR,
  LOG_WARNING,
  LOG_COUNT,
} Log_type;

void log_f(Log_type type, const char* fmt, ...);

// file

// reads entire file and gives back the string holding the contents. (caller must be responsible for freeing the string!)
const char* slurp_file(const char* filename);

//
// String view
//

typedef struct {
  cstr data;
  int32 count;
} String_view;

#define SV_FMT "%.*s"
#define SV_ARG(sv) sv.count, sv.data

#define SV(cstr) (String_view){.data = cstr, strlen(cstr)}

void sv_print_dumb(String_view sv);
String_view sv_from_cstr(const char* cstr);
String_view sv_lpop_until_predicate(String_view* sv, int(*predicate)(int));
String_view sv_rpop_until_predicate(String_view* sv, int(*predicate)(int));
String_view sv_lpop_until_char(String_view* sv, char ch);
String_view sv_rpop_until_char(String_view* sv, char ch);
void sv_lremove(String_view* sv, int n);
void sv_rremove(String_view* sv, int n);
void sv_lremove_until_char(String_view* sv, char ch);
void sv_rremove_until_char(String_view* sv, char ch);
void sv_lremove_until_char_after(String_view* sv, char ch);
void sv_rremove_until_char_after(String_view* sv, char ch);
void sv_ltrim(String_view* sv);
void sv_rtrim(String_view* sv);
void sv_trim(String_view* sv);
char* sv_to_cstr(String_view sv);
int32   sv_to_int(String_view sv);
real32 sv_to_float(String_view sv);
bool sv_contains_char(String_view sv, char ch);

#endif /* _STDLIB_H_ */

//////////////////////////////////////////////////
#ifdef COMMONLIB_IMPLEMENTATION
#include <string.h>
#include <errno.h>
#include <stdlib.h>

void log_f(Log_type type, const char* fmt, ...){
  va_list args;
  va_start(args, fmt);

  switch (type){
  case LOG_INFO: printf("INFO: "); break;
  case LOG_ERROR: printf("ERROR: "); break;
  case LOG_WARNING: printf("WARNING: "); break;
  default: ASSERT(0 && "Unreachable");
  }

  while (*fmt != '\0'){
    if (*fmt == '%'){
      fmt++;
      switch (*fmt){
      case 's': {
	const char* str = va_arg(args, const char*);
	printf("%s", str);
      } break;
      case 'i':
      case 'd': {
        int i = va_arg(args, int);
	printf("%d", i);
      } break;
      case 'o': {
	int i = va_arg(args, int);
	printf("%o", i);
      } break;
      case 'u': {
	unsigned int i = va_arg(args, unsigned int);
	printf("%u", i);
      } break;
      case 'f':
      case 'F': {
	double i = va_arg(args, double);
	printf("%f", i);
      } break;
      case 'p': {
        void* i = va_arg(args, void*);
	printf("%p", i);
      } break;
      case '%': {
	printf("%%");
      }
      case 'c':{
        char i = va_arg(args, char);
	printf("%c", i);
      }
      }
    } else {
      putc(*fmt, stdout);
    }
    fmt++;
  }

  putc('\n', stdout);
#ifdef FLUSH_ON_LOG
  fflush(stdout);
#endif
  va_end(args);
}

// simple and dirty way to have defering in C (not recommended to use!)
#define defer(ret_val) \
  result = ret_val;\
  goto defer

const char* slurp_file(const char* filename) {
  FILE* f = fopen(filename, "rb");
  char* result = NULL;

#define TMPBUFF_SIZE (1024)
  char tmpbuff[TMPBUFF_SIZE] = {0}; // for strerror_s()

  if (f == NULL){
    log_f(LOG_ERROR, "slurp_file::fopen(\"%s\", \"rb\") -> %s\n", filename, strerror_s(tmpbuff, TMPBUFF_SIZE, errno));
    defer(NULL);
  }

  if (fseek(f, 0, SEEK_END) < 0) {
    log_f(LOG_ERROR, "slurp_file::fseek(f, 0, SEEK_END) -> %s\n", filename, strerror_s(tmpbuff, TMPBUFF_SIZE, errno));
    defer(NULL);
  }

  size_t fsize = ftell(f);

  if (fsize == -1){
    log_f(LOG_ERROR, "slurp_file::ftell(f) -> %s\n", filename, strerror_s(tmpbuff, TMPBUFF_SIZE, errno));
    defer(NULL);
  }

  result = malloc(sizeof(char)*(fsize+1));

  if (result == NULL){
    log_f(LOG_ERROR, "slurp_file::malloc(%zu) -> %s\n", sizeof(char)*fsize, strerror_s(tmpbuff, TMPBUFF_SIZE, errno));
    defer(NULL);
  }

  if (fseek(f, 0, SEEK_SET) < 0) {
    log_f(LOG_ERROR, "slurp_file::fseek(f, 0, SEEK_SET) -> %s\n", filename, strerror_s(tmpbuff, TMPBUFF_SIZE, errno));
    defer(NULL);
  }

  if (fread((char*)result, sizeof(char), fsize, f) != fsize){
    log_f(LOG_ERROR, "slurp_file::fread(result, %d, 1, f) -> %s\n", fsize, strerror_s(tmpbuff, TMPBUFF_SIZE, errno));
    defer(NULL);
  }

  // set null-terminator
  result[fsize] = '\0';

 defer:
  if (f) fclose(f);
  return result;
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
  int old_sv_count = sv->count;
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
  int old_sv_count = sv->count;
  while (sv->count > 0 && *(sv->data+sv->count-1) != ch){
    sv->count--;
  }

  return (String_view){
    .data = sv->data + sv->count,
    .count = old_sv_count - sv->count,
  };
}

void sv_lremove(String_view* sv, int n){
  if (n > sv->count) n = sv->count;

  sv->data += n;
  sv->count -= n;
}

void sv_rremove(String_view* sv, int n){
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

int32 sv_to_int(String_view sv) {
  char* str = sv_to_cstr(sv);
  int32 res = atoi(str);
  free(str);
  return res;
}

real32 sv_to_float(String_view sv) {
  char* str = sv_to_cstr(sv);
  real32 res = atof(str);
  free(str);
  return res;
}

bool sv_contains_char(String_view sv, char ch){
  for (size_t i = 0; i < sv.count; ++i){
    if (sv.data[i] == ch) return true;
  }
  return false;
}

#endif
