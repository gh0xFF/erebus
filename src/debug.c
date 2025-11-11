#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <execinfo.h>
#include <unistd.h>

typedef struct {
    void*       ptr;
    size_t      size;
    int         line;
    const char* file;
    const char* type;
} alloc_info_t;

typedef struct {
    char        line[100];
    size_t      size;
    uint16_t    allocated_times;
} stats_t;

#define MAX_ALLOCS 100
#define BACKTRACE_SIZE 100

static alloc_info_t allocations[MAX_ALLOCS] = {0};
static size_t alloc_count = 0;

static stats_t alloc_stats[MAX_ALLOCS] = {0};
static int allocs = 0;

void* debug_malloc(size_t size, const char* file, int line);
void* debug_calloc(size_t num, size_t size, const char* file, int line);
void* debug_realloc(void* ptr, size_t size, const char* file, int line);
void debug_free(void* ptr, const char* file, int line);
void update_stats(size_t size, const char* file, int line);
void memory_report(void);

void setup_signal_handlers(void);
void segfault_handler(int sig);

void update_stats(size_t size, const char* file, int line) {
    char str[100];
    sprintf(str, "%s:%d", file, line);

    uint8_t found = 0;
    for (int i = 0; i < MAX_ALLOCS; i++) {
        if (strcmp(alloc_stats[i].line, str) == 0) {
            alloc_stats[i].allocated_times++;
            found = 1;
        }
    }

    if (!found) {
        allocs++;
        alloc_stats[allocs].allocated_times = 0;
        alloc_stats[allocs].size = size;
        strncpy(alloc_stats[allocs].line, str, 100);
    }
}

void* debug_malloc(size_t size, const char* file, int line) {
    void* ptr = malloc(size);
    if(ptr && alloc_count < MAX_ALLOCS) {
        allocations[alloc_count].ptr = ptr;
        allocations[alloc_count].size = size;
        allocations[alloc_count].file = file;
        allocations[alloc_count].line = line;
        allocations[alloc_count].type = "malloc";
        alloc_count++;
    }
    fprintf(stdout, "call malloc: %ld bytes at %s:%d\n", size, file, line);

    update_stats(size, file, line);
    return ptr;
}

void* debug_calloc(size_t num, size_t size, const char* file, int line) {
    void* ptr = calloc(num, size);
    if (ptr && alloc_count < MAX_ALLOCS) {
        allocations[alloc_count].ptr = ptr;
        allocations[alloc_count].size = num * size;
        allocations[alloc_count].file = file;
        allocations[alloc_count].line = line;
        allocations[alloc_count].type = "calloc";
        alloc_count++;
    }
    fprintf(stdout, "call calloc: %ld bytes at %s:%d\n", num * size, file, line);

    update_stats(size, file, line);
    return ptr;
}

void* debug_realloc(void* ptr, size_t size, const char* file, int line) {
    size_t old_size = 0;
    int old_line = 0;
    const char* old_file = NULL;
    const char* old_type = NULL;
    
    for (size_t i = 0; i < alloc_count; i++) {
        if (allocations[i].ptr == ptr) {
            old_size = allocations[i].size;
            old_file = allocations[i].file;
            old_line = allocations[i].line;
            old_type = allocations[i].type;
            allocations[i] = allocations[alloc_count - 1];
            alloc_count--;
            break;
        }
    }
    
    void* new_ptr = realloc(ptr, size);
    
    if (new_ptr) {
        if (alloc_count < MAX_ALLOCS) {
            allocations[alloc_count].ptr = new_ptr;
            allocations[alloc_count].size = size;
            allocations[alloc_count].file = file;
            allocations[alloc_count].line = line;
            allocations[alloc_count].type = "realloc";
            alloc_count++;
        }
    } else {
        if (ptr != NULL && alloc_count < MAX_ALLOCS) {
            allocations[alloc_count].ptr = ptr;
            allocations[alloc_count].size = old_size;
            allocations[alloc_count].file = old_file;
            allocations[alloc_count].line = old_line;
            allocations[alloc_count].type = old_type;
            alloc_count++;
        }
    }
    
    fprintf(stdout, "call realloc: %ld bytes at %s:%d\n", size, file, line);
    
    update_stats(size, file, line);
    return new_ptr;
}

void debug_free(void* ptr, const char* file, int line) {
    if (!ptr) return;

    for(size_t i = 0; i < alloc_count; i++) {
        if(ptr == allocations[i].ptr) {
            fprintf(stdout, "FREE: %p (%s, size: %zu) allocated at %s:%d, freed at %s:%d\n",
            ptr, allocations[i].type, allocations[i].size, allocations[i].file, allocations[i].line, file, line);

            allocations[i] = allocations[alloc_count - 1];
            alloc_count--;
            break;
        }
    }
    free(ptr);
}

void memory_report(void) {
    fprintf(stdout, "\n=== MEMORY REPORT ===\n");
    for(size_t i = 0; i < alloc_count; i++) {
        alloc_info_t ait = allocations[i];

        fprintf(stdout, "ADDR: %p, size: %zu, allocated at %s:%d\n",
            ait.ptr, ait.size, ait.file, ait.line
        );
    }

    if (allocs > 1) {
        for (int i = 0; i < MAX_ALLOCS - 1; i++) {
            for (int j = 0; j < MAX_ALLOCS - i - 1; j++) {
                if (alloc_stats[j].allocated_times < alloc_stats[j + 1].allocated_times) {
                    stats_t temp = alloc_stats[j];
                    alloc_stats[j] = alloc_stats[j + 1];
                    alloc_stats[j + 1] = temp;
                }
            }
        }

        for (int i = 0; i < 10; i++) {
            fprintf(
                stdout,
                "%s, size: %ld, times %d\n", alloc_stats[i].line, alloc_stats[i].size, alloc_stats[i].allocated_times
            );
        }
    }
}


void segfault_handler(int sig) {
    void *array[BACKTRACE_SIZE];
    int size;
    
    fprintf(stderr, "Segmentation fault! Signal %d received.\n", sig);
    
    size = backtrace(array, BACKTRACE_SIZE);
    fprintf(stderr, "Backtrace:\n");
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    
    exit(1);
}

void setup_signal_handlers(void) {
    struct sigaction sa;
    sa.sa_handler = segfault_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    
    sigaction(SIGSEGV, &sa, NULL);
}
