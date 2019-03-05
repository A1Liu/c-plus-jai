# C+Jai
This repository holds C utility functions that are heavily inspired by [Jonathan
Blow's language, Jai][jai-programming-language].

[jai-programming-language]: https://www.youtube.com/watch?v=uZgbKrDEzAs

## `/temp_mem` Linear Allocator and Associated Functions
This folder holds functions that essentially plagiarize [Jai's temporary storage system.][temp-mem-system]

[temp-mem-system]: https://youtu.be/SSVHWrYG974?t=130

Functions are all contained to user-defined namespaces that default to `tm`,
`ts`, and `t` by default. These namespaces are structs of function pointers:

### `tm` API
The struct `tm` is of type `TemporaryMemory` and its namespace is
defined by the preprocessor variable `TEMPORARY_MEMORY_NAMESPACE`.
It defines the following functions as members:

* `void* alloc(size_t size)` -
Allocates `size` bytes of memory in the temporary storage buffer. If the buffer
is no longer allocated, it creates a buffer of size `DEFAULT_TMEM_CAPACITY`
* `void release(void)` -
Frees all temporary memory. Note that this is usually not the best.
* `void* root(void)` -
Returns a pointer to the beginning of the temporary memory buffer
* `void change_buffer(void* new_root, size_t new_capacity)` -
Frees all temporary memory, and sets the root to `new_root`, with
capacity `new_capacity`.
* `void* loc(void)` -
Returns a pointer to the current location of temporary memory
* `void free(void* ptr)` -
Sets the current index into the buffer to `ptr`, or resets temporary
memory to beginning of the buffer if `ptr` is `NULL`
* `size_t size(void)` -
Returns the current amount of memory being used
* `size_t capacity(void)` -
Returns the current capacity of temporary memory
* `void set_capacity(size_t size)` -
Sets the capacity of the temporary buffer to `size`, using `malloc`.

