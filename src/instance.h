#ifndef INSTANCE_H
#define INSTANCE_H

#define OPTION_DECRYPT 1
#define OPTION_ENCRYPT 2

#define CLI_MODE 0
#define INTERACTIVE_MODE 1

#include <stdint.h>

int run_app(
    char *template,
    char *msg,  
    uint16_t offset,  
    uint8_t option,
    uint8_t mode
);

#endif
