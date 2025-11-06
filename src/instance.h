#ifndef INSTANCE_H
#define INSTANCE_H

#define OPTION_DECRYPT 1
#define OPTION_ENCRYPT 2

#define CLI_MODE 0
#define INTERACTIVE_MODE 1

#include <stdint.h>

int run_app(
    char *template,
    const char *msg,  
    const uint16_t offset,  
    const uint8_t option,
    const uint8_t mode
);

#endif
