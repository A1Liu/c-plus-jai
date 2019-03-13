#ifndef TEMPORARY_MEMORY_ALLOCATOR
#define TEMPORARY_MEMORY_ALLOCATOR
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>

#ifndef TEMPORARY_MEMORY_NAMESPACE
#define TEMPORARY_MEMORY_NAMESPACE tm
#endif

#ifndef TEMPORARY_STRING_NAMESPACE
#define TEMPORARY_STRING_NAMESPACE ts
#endif

#ifndef TEMPORARY_MEMORY_UTILITIES_NAMESPACE
#define TEMPORARY_MEMORY_UTILITIES_NAMESPACE t
#endif

#ifndef DEFAULT_TMEM_CAPACITY
#define DEFAULT_TMEM_CAPACITY 10000
#endif

#ifndef TEMPORARY_MEMORY_DEBUG_MESSAGES
#define TEMPORARY_MEMORY_DEBUG_MESSAGES 0
#endif

typedef struct {
  void* (*alloc)(size_t);
  void (*release)(void);
  void* (*root)(void);
  void (*change_buffer)(void* new_root, size_t new_capacity);
  void* (*loc)(void);
  void (*free)(void*);
  size_t (*size)(void);
  size_t (*capacity)(void);
  void (*set_capacity)(size_t);
} TemporaryMemory;

extern const TemporaryMemory TEMPORARY_MEMORY_NAMESPACE;

typedef struct {
  char* (*format)(char* fmt, ...);
  char* (*vformat)(char* fmt, va_list args);
//  void (*print)(char* fmt, ...);
//  void (*println)(char* fmt, ...);
//  void (*fprint)(FILE* buf, char* fmt, ...);
} TemporaryString;

extern const TemporaryString TEMPORARY_STRING_NAMESPACE;

typedef struct {
  TemporaryMemory mem;
  TemporaryString str;
} TemporaryMemoryUtilities;

extern const TemporaryMemoryUtilities TEMPORARY_MEMORY_UTILITIES_NAMESPACE;

#endif
