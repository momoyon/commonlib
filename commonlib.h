#ifndef _STDLIB_H_
#define _STDLIB_H_
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <windows.h>
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

#define ASSERT(condition) if (!(condition)) panic_assertion(#condition, stderr, __FILE__, __LINE__)

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof(arr[0]))

#define STRUCT(name) typedef struct name name
#define ENUM(name)   typedef enum name name

// Struct pre-decls

STRUCT(Arena);

void panic_assertion(cstr msg, FILE* file, cstr filename, int line);

#define TEMP_BUFF_SIZE (1024*4)
static char tempbuff[TEMP_BUFF_SIZE];

#define temp_sprintf(var, fmt, ...) \
  snprintf(tempbuff, TEMP_BUFF_SIZE, fmt, __VA_ARGS__);	\
  (var) = tempbuff

//
// Winapi
//

#define WINAPI_ERROR_MSG_BUFF_SIZE 1024
static char winapi_error_msg_buff[WINAPI_ERROR_MSG_BUFF_SIZE];
#define WINAPI_OUTPUT_STR_BUFF_SIZE (16*1024)
static CHAR_INFO winapi_output_str_buff[WINAPI_OUTPUT_STR_BUFF_SIZE];

bool output_str(cstr text);
bool output_strn(cstr text, size_t text_len);
cstr winapi_get_last_error_str(void);
cstr winapi_get_current_working_directory(Arena* arena);

//
// logging
//

// FLUSH_ON_LOG - define this macro to fflush() on every call to log() not defined by default.

typedef enum {
  LOG_INFO = 0,
  LOG_ERROR,
  LOG_WARNING,
  LOG_COUNT,
} Log_type;

void log_file(Log_type type, FILE* file, cstr fmt, ...);
#define log_f(type, fmt, ...)  log_file(type, stdout, fmt, __VA_ARGS__)
#define log_info(fmt, ...)     log_f(LOG_INFO, fmt, __VA_ARGS__)
#define log_error(fmt, ...)    log_f(LOG_ERROR, fmt, __VA_ARGS__)
#define log_warning(fmt, ...)  log_f(LOG_WARNING, fmt, __VA_ARGS__)

//
// File
//

// reads entire file and gives back the string holding the contents. (caller must be responsible for freeing the string!)
const char* slurp_file(const char* filename);

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

#define Arena_alloc_str(a, fmt, ...)  Arena_alloc(&(a), sizeof(char)*snprintf((a).ptr, (a).buff_size - ((uint8*)(a).ptr - (uint8*)(a).buff), (fmt), __VA_ARGS__)+1)
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

#endif /* _STDLIB_H_ */

//////////////////////////////////////////////////
#ifdef COMMONLIB_IMPLEMENTATION
#include <string.h>
#include <errno.h>
#include <stdlib.h>

// Winapi
cstr winapi_get_last_error_str(void) {
  DWORD error_code = GetLastError();
  if (!FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, error_code, 0, winapi_error_msg_buff, WINAPI_ERROR_MSG_BUFF_SIZE, NULL)){
    log_f(LOG_ERROR, "FormatMessage() failed with code: %d", GetLastError());
    return NULL;
  }
  return winapi_error_msg_buff;
}

cstr winapi_get_current_working_directory(Arena* arena) {
  if (arena->buff_size < MAX_PATH) {
    log_error("Arena size should be at least of size MAX_PATH(%u)", MAX_PATH);
    return NULL;
  }

  size_t n = GetCurrentDirectoryA((DWORD)arena->buff_size, arena->buff);
  if (n == 0) {
    log_error("%s -> %s", __func__, winapi_get_last_error_str());
    return NULL;
  }
  return arena->buff;
}

bool output_str(cstr text) {
  return output_strn(text, strlen(text));
}

bool output_strn(cstr text, size_t text_len) {
  SECURITY_ATTRIBUTES sa = {0};
  sa.nLength = sizeof(sa);
  sa.bInheritHandle = TRUE;

  HANDLE console = CreateFileA("CONOUT$",
			       GENERIC_READ|GENERIC_WRITE,
			       FILE_SHARE_WRITE,
			       &sa,
			       OPEN_EXISTING,
			       0,
			       NULL);

  if (console == INVALID_HANDLE_VALUE) {
    log_f(LOG_INFO, "Could not get a handle to the active console screen buffer: %s", winapi_get_last_error_str());
    return 1;
  }

  if (text_len == 0) return false;

  ASSERT(text_len < WINAPI_OUTPUT_STR_BUFF_SIZE);
  CHAR_INFO* buff = winapi_output_str_buff;

  CONSOLE_SCREEN_BUFFER_INFO csbi = {0};

  if (!GetConsoleScreenBufferInfo(console, &csbi)) {
    log_f(LOG_ERROR, "Could not get console screen buffer info: %s", winapi_get_last_error_str());
    return false;
  }

  COORD buff_size = {0, 1};
  COORD new_cursor = {
    .X = csbi.dwCursorPosition.X,
    .Y = csbi.dwCursorPosition.Y
  };

  bool overflown = false;

  char* next_text = NULL;

  for (size_t i = 0; i < text_len; ++i) {
    if (text[i] == '\n') {
      overflown = true;
      next_text = (char*)text+buff_size.X+1;
      text_len = (text+text_len)-text;
      break;
    } else {
      buff[i].Char.AsciiChar = text[i];
      buff_size.X++;
      new_cursor.X++;
      if (new_cursor.X >= csbi.dwMaximumWindowSize.X) {
	overflown = true;
	next_text = (char*)text+buff_size.X;
	text_len = (text+text_len)-text;
	break;
      }
    }

    buff[i].Attributes = FOREGROUND_RED|FOREGROUND_INTENSITY;
  }

  COORD write_coord = {0, 0};
  SMALL_RECT rect = {
    .Left   = csbi.dwCursorPosition.X,
    .Top    = csbi.dwCursorPosition.Y,
    .Right  = new_cursor.X,
    .Bottom = new_cursor.Y,
  };

  if(!WriteConsoleOutput(console,
			 buff,
			 buff_size,
			 write_coord,
			 &rect)) {
    log_f(LOG_ERROR, "[1] Could not write to console output: [%d] %s", GetLastError(), winapi_get_last_error_str());
    return false;
  }

  if (!overflown) {
    // advance cursor
    if (!SetConsoleCursorPosition(console, new_cursor)) {
      log_f(LOG_ERROR, "[1] Could not set new cursor pos: %s", winapi_get_last_error_str());
      return false;
    }
  }

  if (overflown) {
    new_cursor.X = 0;
    new_cursor.Y++;
    // advance cursor
    if (!SetConsoleCursorPosition(console, new_cursor)) {
      log_f(LOG_ERROR, "[2] Could not set new cursor pos: %s", winapi_get_last_error_str());
      return false;
    }
    output_strn(next_text, text_len);
  }

  return true;
}

void panic_assertion(cstr msg, FILE* file, cstr filename, int line) {
  fprintf(file, "%s:%d:0: ASSERTION FAILED: %s\n", filename, line, msg);
  exit(1);
}

void log_file(Log_type type, FILE* file, cstr fmt, ...) {
  va_list args;
  va_start(args, fmt);

  SYSTEMTIME sys_time = {0};
  GetLocalTime(&sys_time);

  fprintf(file, "[%02d:%02d:%02d] ", sys_time.wHour, sys_time.wMinute, sys_time.wSecond);

  switch (type){
  case LOG_INFO: fprintf(file, "INFO: "); break;
  case LOG_ERROR: fprintf(file, "ERROR: "); break;
  case LOG_WARNING: fprintf(file, "WARNING: "); break;
  default: ASSERT(0 && "Unreachable");
  }

  while (*fmt != '\0'){
    if (*fmt == '%'){
      fmt++;
      switch (*fmt){
      case 's': {
	const char* str = va_arg(args, const char*);
	fprintf(file, "%s", str);
      } break;
      case 'i':
      case 'd': {
        int i = va_arg(args, int);
	fprintf(file, "%d", i);
      } break;
      case 'o': {
	int i = va_arg(args, int);
	fprintf(file, "%o", i);
      } break;
      case 'u': {
	unsigned int i = va_arg(args, unsigned int);
	fprintf(file, "%u", i);
      } break;
      case 'f':
      case 'F': {
	double i = va_arg(args, double);
	fprintf(file, "%f", i);
      } break;
      case 'p': {
        void* i = va_arg(args, void*);
	fprintf(file, "%p", i);
      } break;
      case '%': {
	fprintf(file, "%%");
      }
      case 'c':{
        char i = va_arg(args, char);
	fprintf(file, "%c", i);
      }
      }
    } else {
      fputc(*fmt, file);
    }
    fmt++;
  }

  fputc('\n', file);
#ifdef FLUSH_ON_LOG
  fflush(file);
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
// ### Allocators ###
//

// Arena

Arena Arena_make(size_t size) {
  Arena res = {0};
  res.buff_size = size == 0 ? ARENA_BUFF_INITIAL_SIZE : size;
  res.buff = malloc(res.buff_size);
  res.ptr = res.buff;

  ASSERT(res.buff);

  return res;
}

void* Arena_alloc(Arena* a, size_t size) {
  ASSERT(a->buff);

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

  if (ret == -1) {
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
