#ifndef TEMPORARY_MEMORY_ALLOCATOR
#define TEMPORARY_MEMORY_ALLOCATOR
#include <ctype.h>
#include <stdarg.h>

#ifndef TEMPORARY_MEMORY_NAMESPACE
#define TEMPORARY_MEMORY_NAMESPACE t_mem
#endif

#ifndef TEMPORARY_STRING_NAMESPACE
#define TEMPORARY_STRING_NAMESPACE t_str
#endif

#ifndef TEMPORARY_MEMORY_NAMESPACE
#define TEMPORARY_MEMORY_UTILITIES_NAMESPACE t
#endif

typedef struct {
  void* (*alloc)(size_t);
  void (*release)(void);
  void* (*root)(void);
  void* (*loc)(void);
  void (*free)(void*);
  size_t (*size)(void);
  size_t (*capacity)(void);
  void (*set_capacity)(size_t);
} TemporaryMemory;

extern const TemporaryMemory TEMPORARY_MEMORY_NAMESPACE;

typedef struct {
  char* (*format)(char* fmt, ...);
  char* (*va_format)(char* fmt, va_list args);
  void (*print)(char* fmt, ...);
  void (*println)(char* fmt, ...);
} TemporaryString;

extern const TemporaryString TEMPORARY_STRING_NAMESPACE;

typedef struct {
  void* (*alloc)(size_t);
  void (*release)(void);
  void* (*root)(void);
  void* (*loc)(void);
  void (*free)(void*);
  size_t (*size)(void);
  size_t (*capacity)(void);
  void (*set_capacity)(size_t);
  char* (*format)(char* fmt, ...);
  char* (*va_format)(char* fmt, va_list args);
  void (*print)(char* fmt, ...);
  void (*println)(char* fmt, ...);
} TemporaryMemoryUtilities;

extern const TemporaryMemoryUtilities TEMPORARY_MEMORY_UTILITIES_NAMESPACE;

#endif
