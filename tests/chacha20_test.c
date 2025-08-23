#include "../src/chacha20.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

void chacha20_test(void);
void xor(uint64_t c, uint8_t *payload);
void run_test(int i);

void chacha20_test(void) {
    fprintf(stdout, "\tTEST chacha20 double xor..............................................");
    {
        for(int i = 0; i < INT_MAX / 1024 / 8; i+=8) {
            run_test(i);
            run_test(i+1);
            run_test(i+2);            
            run_test(i+3);            
            run_test(i+4);            
            run_test(i+5);            
            run_test(i+6);        
            run_test(i+7);            
        }
    }    
    
    fprintf(stdout, "OK\n");
}

void run_test(int i) {
    int buf_size = 4096;
    uint8_t *payload = (uint8_t*)calloc((size_t)buf_size, sizeof(char));
    uint8_t *result = (uint8_t*)calloc((size_t)buf_size, sizeof(char));
    
    assert(payload != NULL);
    assert(result != NULL);

    for(size_t idx = 0; idx < 4096; idx+=8) {
        payload[idx+0] = (uint8_t)(idx+0 % 256);
        payload[idx+1] = (uint8_t)(idx+1 % 256);
        payload[idx+2] = (uint8_t)(idx+2 % 256);
        payload[idx+3] = (uint8_t)(idx+3 % 256);
        payload[idx+4] = (uint8_t)(idx+4 % 256);
        payload[idx+5] = (uint8_t)(idx+5 % 256);
        payload[idx+6] = (uint8_t)(idx+6 % 256);
        payload[idx+7] = (uint8_t)(idx+7 % 256);
    }

    memcpy((char*)result, (char*)payload, (size_t)buf_size);

    xor((uint64_t)i, payload);
    assert(memcmp(result, payload, (size_t)buf_size) != 0);
    xor((uint64_t)i, payload);
    assert(memcmp(result, payload, (size_t)buf_size) == 0);

    free(payload);
    free(result);
}

inline void xor(uint64_t c, uint8_t *payload) {
    uint8_t chacha_key[32] = "12345678901234567890123456789012";
    uint8_t nonce[12] = "0123456789ab";

    struct chacha20_context ctx;
    chacha20_init_context(&ctx, chacha_key, nonce, c);
    chacha20_xor(&ctx, payload, 4096);
}