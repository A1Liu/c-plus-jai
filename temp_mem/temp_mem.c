#include "temp_mem.h"
#include <stdlib.h>

#define xstr(arg) #arg
#define str(arg) xstr(arg)

static void* alloc(size_t);
static void mem_free(void);
static void* root(void);
static void change_buffer(void*, size_t);
static void* location(void);
static void set_location(void* loc);
static size_t size(void);
static size_t capacity(void);
static void set_capacity(size_t);
static char* format(char* fmt, ...);
static char* va_format(char* fmt, va_list args);

static char* current_buffer_location;
static char* mem_buffer = NULL;
static uint32_t mem_buffer_capacity = 0;
static int hide_debug = 0;

const TemporaryMemory TEMPORARY_MEMORY_NAMESPACE = {
  alloc, mem_free, root, change_buffer,
  location, set_location,
  size, capacity, set_capacity };

const TemporaryString TEMPORARY_STRING_NAMESPACE =
  { format, va_format };

const TemporaryMemoryUtilities TEMPORARY_MEMORY_UTILITIES_NAMESPACE =
{
  { alloc, mem_free, root, change_buffer,
  location, set_location,
  size, capacity, set_capacity },
  { format, va_format }
};

static void*
alloc(size_t size)
{
  static char* current;
  if (mem_buffer == NULL) {
    #if TEMPORARY_MEMORY_DEBUG_MESSAGES
    printf("[DEBUG] " str(TEMPORARY_MEMORY_NAMESPACE)
        ".alloc(): Buffer allocation of size %d\n",
        DEFAULT_TMEM_CAPACITY);
    #endif
    set_capacity(DEFAULT_TMEM_CAPACITY);
  }
  current = current_buffer_location;
  current_buffer_location += size / sizeof(char*);
  if (size % sizeof(char*) != 0)
    current_buffer_location += 1;
  #if TEMPORARY_MEMORY_DEBUG_MESSAGES
  if (current_buffer_location > mem_buffer + mem_buffer_capacity)
    printf("[DEBUG] " str(TEMPORARY_MEMORY_NAMESPACE)
        ".alloc(): Buffer overflow on next call\n");
  #endif
  return current;
}

static
void
mem_free(void)
{
    if (mem_buffer == NULL) {
    return;
  } else {
    #if TEMPORARY_MEMORY_DEBUG_MESSAGES
    printf("[DEBUG] " str(TEMPORARY_MEMORY_NAMESPACE)
        ".release(): Freeing buffer at %p\n", mem_buffer);
    #endif
    free(mem_buffer);
    mem_buffer = NULL;
    current_buffer_location = NULL;
    mem_buffer_capacity = 0;
  }
}

static
void*
root(void)
{
  return mem_buffer;
}

static
void
change_buffer(void* new_root, size_t new_capacity)
{
  #if TEMPORARY_MEMORY_DEBUG_MESSAGES
  printf("[DEBUG] " str(TEMPORARY_MEMORY_NAMESPACE)
      ".change_buffer(): Moving to %p\n", new_root);
  #endif
  mem_free();
  mem_buffer = new_root;
  set_capacity(new_capacity);
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
    #if TEMPORARY_MEMORY_DEBUG_MESSAGES
    if(!hide_debug)
      printf("[DEBUG] " str(TEMPORARY_MEMORY_NAMESPACE)
          ".free(): Moving location back to root at %p\n", mem_buffer);
    #endif
    current_buffer_location = mem_buffer;
  } else {
    #if TEMPORARY_MEMORY_DEBUG_MESSAGES
    if (!hide_debug)
      printf("[DEBUG] " str(TEMPORARY_MEMORY_NAMESPACE)
          ".free(): Moving to %p, bounded by (%p, %p]\n",
          loc, mem_buffer, mem_buffer + mem_buffer_capacity);
    #endif
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
  static size_t true_capacity;
  if (size == 0) {
    #if TEMPORARY_MEMORY_DEBUG_MESSAGES
    printf("[DEBUG] " str(TEMPORARY_MEMORY_NAMESPACE)
        ".set_capacity(): Got a size of 0; doing nothing.\n");
    #endif
    return;
  } else if (mem_buffer != NULL && mem_buffer_capacity == 0) {
    #if TEMPORARY_MEMORY_DEBUG_MESSAGES
    printf("[DEBUG] " str(TEMPORARY_MEMORY_NAMESPACE)
        ".set_capacity(): Setting capacity value to %lu\n", size);
    #endif
    mem_buffer_capacity = size / sizeof(char*);
    true_capacity = mem_buffer_capacity * sizeof(char*);
  } else if (size <= true_capacity && size > true_capacity * .75 ) {
    #if TEMPORARY_MEMORY_DEBUG_MESSAGES
    printf("[DEBUG] " str(TEMPORARY_MEMORY_NAMESPACE)
        ".set_capacity(): Setting capacity value to %lu\n", size);
    #endif
    mem_buffer_capacity = size / sizeof(char*);
    current_buffer_location = mem_buffer;
  } else {
    #if TEMPORARY_MEMORY_DEBUG_MESSAGES
    printf("[DEBUG] " str(TEMPORARY_MEMORY_NAMESPACE)
        ".set_capacity(): Allocating buffer with size %lu\n", size);
    #endif
    mem_free();
    mem_buffer_capacity = size / sizeof(char*);
    if (size % sizeof(char*) != 0)
      mem_buffer_capacity += 1;
    true_capacity = mem_buffer_capacity * sizeof(char*);
    current_buffer_location = mem_buffer = malloc(true_capacity);
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
  hide_debug = 1;
  static const int predicted_len = 25;
  static va_list copy;
  // Create and use buffer w/ predicted length
  char* buffer = alloc(sizeof(char) * predicted_len);
  va_copy(copy, args);
  int len = vsnprintf(buffer, predicted_len + 1, fmt, copy);
  va_end(copy);
  if (len == predicted_len) {
    return buffer;
  } else if (len < predicted_len) {
    // Set location back to not waste space
    set_location(buffer);
    alloc((len + 1) * sizeof(char*));
    hide_debug = 0;
    return buffer;
  } else {
    size_t size_diff = (len - predicted_len) * sizeof(char);
    size_diff = size_diff - len * sizeof(char) % sizeof(char*);
    alloc(size_diff); // Can waste up to 14 bytes
    vsprintf(buffer, fmt, args);
    hide_debug = 0;
    return buffer;
  }
}

