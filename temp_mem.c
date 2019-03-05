#define TEMPORARY_MEMORY_NAMESPACE T
#include "temp_mem.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_TMEM_CAPACITY 10000

static void* alloc(size_t);
static void mem_free(void);
static void* root(void);
static void* location(void);
static void set_location(void* loc);
static size_t size(void);
static size_t capacity(void);
static void set_capacity(size_t);
static char* format(char* fmt, ...);
static char* va_format(char* fmt, va_list args);
static void print(char* fmt, ...);
static void println(char* fmt, ...);

static char* current_buffer_location;
static char* mem_buffer = NULL;
static int mem_buffer_capacity = DEFAULT_TMEM_CAPACITY;

static void*
alloc(size_t size) {
  if (mem_buffer == NULL) {
    mem_buffer = malloc(sizeof(char*) * DEFAULT_TMEM_CAPACITY);
    current_buffer_location = mem_buffer;
  }
  void* current = current_buffer_location;
  current_buffer_location += size / sizeof(char*);
  if (size % sizeof(char*) != 0)
    current_buffer_location += 1;
  return current;
}

static
void
mem_free(void)
{
  free(mem_buffer);
  mem_buffer = NULL;
  current_buffer_location = NULL;
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
location(void)
{
  return current_buffer_location;
}

static
void
set_location(void* loc)
{
  if (loc == NULL) {
    current_buffer_location = mem_buffer;
  } else if ((char*) loc < mem_buffer
      || (char*) loc > mem_buffer + capacity())
  { // If loc is outside our current buffer
    current_buffer_location = mem_buffer = loc;
    mem_buffer_capacity = 0;
  } else {
    current_buffer_location = loc;
  }
}

static
size_t
size(void)
{
  return (current_buffer_location - mem_buffer) * sizeof(char*);
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
    current_buffer_location = mem_buffer;
  } else {
    free(mem_buffer);
    current_buffer_location = mem_buffer = malloc(size);
    mem_buffer_capacity = size / sizeof(char*);
  }
}

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
  // Weights that determine learning rate
  static const float NEW_WEIGHT = 1 / 4.0;
  static const float OLD_WEIGHT = 1 - NEW_WEIGHT;

  // Prediction for difference in output and format length
  static int prediction = 5;

  // temporary vars
  static int fmt_len, predicted_len, len, actual;

  // Get data about format string
  fmt_len = strlen(fmt);
  predicted_len = fmt_len + prediction;

  // Create and use buffer w/ predicted length
  char* buffer = alloc(sizeof(char) * predicted_len);
  len = vsnprintf(buffer, predicted_len + 1, fmt, args);
  actual = len - fmt_len;
  prediction = actual * NEW_WEIGHT + prediction * OLD_WEIGHT;
  if (len <= predicted_len) {
    // Set location back to not waste space
    set_location(buffer - (predicted_len - len) * sizeof(char) / sizeof(char*));
    return buffer;
  } else {
    alloc(sizeof(char) * (len - predicted_len));
    vsprintf(buffer, fmt, args);
    return buffer;
  }
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
  location, set_location,
  size, capacity, set_capacity };
const TemporaryString TEMPORARY_STRING_NAMESPACE = {format,va_format,print,println};

