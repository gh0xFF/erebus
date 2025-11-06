#ifndef CHACHA20_H
#define CHACHA20_H

#include <stdint.h>
#include <stddef.h>

typedef struct chacha20_context {
	uint64_t counter;
	size_t position;
	uint32_t keystream32[16];
	uint32_t state[16];
	uint8_t key[32];
	uint8_t nonce[12];
} chacha20_ctx;

void chacha20_generator(char* message_buffer, char* key, uint16_t offset);
void chacha20_init_context(chacha20_ctx *ctx, uint8_t key[], uint8_t nounc[], uint64_t counter);
void chacha20_xor(chacha20_ctx *ctx, uint8_t *bytes, size_t n_bytes);

#endif