#ifndef DEBUG_H
#define DEBUG_H

/*
    YES, its looks stupid for my rust-style makefile, but this file helpes me
    to explore alloc/free stats to improve it in the future
*/

#include <stdlib.h>

void* debug_malloc(size_t size, const char* file, int line);
void* debug_calloc(size_t num, size_t size, const char* file, int line);
void* debug_realloc(void* ptr, size_t size, const char* file, int line);
void debug_free(void* ptr, const char* file, int line);
void memory_report(void);

void setup_signal_handlers(void);

#define malloc(size) debug_malloc(size, __FILE__, __LINE__);
#define calloc(num, size) debug_calloc(num, size, __FILE__, __LINE__);
#define realloc(ptr, size) debug_realloc(ptr, size, __FILE__, __LINE__);
#define free(ptr) debug_free(ptr, __FILE__, __LINE__);

#endif