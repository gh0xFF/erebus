#include "../src/adler32.h"
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <stddef.h>

#define MEMORY_LIMIT 1

void adler32_test(void);

void adler32_test(void) {

    // assert(adler32(NULL, -1)); // SEGFAULT (out of index)
    // assert(adler32(NULL, 1));  // SEGFAULT (out of index)
    // assert(adler32("", 1));    // SEGFAULT (out of index)

    char *buffer = "1234567890";

    fprintf(stdout, "\tTEST adler32 with empty params........................................");

    assert(adler32(NULL, 0) == 1);
    assert(adler32("", 0)   == 1);

    fprintf(stdout, "OK\n");

    fprintf(stdout, "\tTEST adler32 with buffer..............................................");
    for (size_t i = 0; i < 10; i++) {
        assert(adler32(buffer, i));
    }
    
    fprintf(stdout, "OK\n");

    #ifdef MEMORY_LIMIT
    fprintf(stdout, "\tTEST adler32 determinism..............................................");
    #pragma message "will be alloceted 128 MB"
    for (uint32_t i = 1; i < UINT32_MAX/32; i*=2) { // 128 MB
        char *test_buffer = (char*)malloc(i);

        assert(adler32(test_buffer, i) == adler32(test_buffer, i));
        free(test_buffer);
    }
    fprintf(stdout, "OK\n");
    #endif

    fprintf(stdout, "\tTEST adler32 determinism X10k.........................................");
    for (int i = 0; i < 10000; i++) {
        assert(adler32(buffer, 10) == adler32(buffer, 10));
    }
    fprintf(stdout, "OK\n");
}