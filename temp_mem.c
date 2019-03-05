#define TEMPORARY_MEMORY_NAMESPACE T
#include "temp_mem.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_TMEM_CAPACITY 10000

int main() {
  char* hello = T.alloc(sizeof(char*) * 20);
  strcpy(hello, "");
  tstr.println("%d", T.size());
  tstr.println("%d", sizeof(char*) * 20);
  return 0;
}

static void* alloc(size_t);
static void mem_free(void);
static void* root(void);
static void* loc(void);
static void set_loc(void* loc);
static size_t size(void);
static size_t capacity(void);
static void set_capacity(size_t);
static char* format(char* fmt, ...);
static char* va_format(char* fmt, va_list args);
static void print(char* fmt, ...);
static void println(char* fmt, ...);

static char* location;
static char* mem_buffer = NULL;
static int mem_buffer_capacity = DEFAULT_TMEM_CAPACITY;

static void*
alloc(size_t size) {
  if (mem_buffer == NULL) {
    mem_buffer = malloc(sizeof(char*) * DEFAULT_TMEM_CAPACITY);
    location = mem_buffer;
  }
  void* current = location;
  location += size / sizeof(char*);
  if (size % sizeof(char*) != 0)
    location += 1;
  return current;
}

static
void
mem_free(void)
{
  free(mem_buffer);
  mem_buffer = NULL;
  location = NULL;
  mem_buffer_capacity = 0;
}

static
void*
root(void)
{
  return mem_buffer;
}

static
void*
loc(void)
{
  return location;
}

static
void
set_loc(void* loc)
{
  if (loc == NULL) {
    location = mem_buffer;
  } else if ((char*) loc < mem_buffer
      || (char*) loc > mem_buffer + capacity())
  { // If loc is outside our current buffer
    location = mem_buffer = loc;
    mem_buffer_capacity = 0;
  } else {
    location = loc;
  }
}

static
size_t
size(void)
{
  return (location - mem_buffer) * sizeof(char*);
}

static
size_t
capacity(void)
{
  return mem_buffer_capacity * sizeof(char*);
}

static
void
set_capacity(size_t size)
{
  if (mem_buffer_capacity == 0) {
    mem_buffer_capacity = size / sizeof(char*);
  } else if (size <= capacity() && size > capacity() * .75 ) {
    location = mem_buffer;
  } else {
    free(mem_buffer);
    location = mem_buffer = malloc(size);
    mem_buffer_capacity = size / sizeof(char*);
  }
}

static char* str_buffer;

static
char*
format(char* fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  char* out = va_format(fmt, args);
  va_end(args);
  return out;
}

static
char*
va_format(char* fmt, va_list args)
{
  if (str_buffer == NULL)
    str_buffer = malloc(sizeof(char) * 1000);
  vsprintf(str_buffer, fmt, args);
  char* out = alloc(sizeof(char) * (strlen(str_buffer) + 1));
  strcpy(out, str_buffer);
  return out;
}

static
void
print(char* fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  printf("%s",va_format(fmt, args));
  va_end(args);
}

static
void
println(char* fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  printf("%s\n",va_format(fmt, args));
  va_end(args);
}

const TemporaryMemory TEMPORARY_MEMORY_NAMESPACE = {
  alloc, mem_free, root,
  loc, set_loc,
  size, capacity, set_capacity };
const TemporaryString TEMPORARY_STRING_NAMESPACE = {format,va_format,print,println};

