#ifndef INSTANCE_H
#define INSTANCE_H

#define OPTION_DECRYPT 1
#define OPTION_ENCRYPT 2

#include <stdint.h>

int run_app(char *template,  char *msg,  uint16_t offset,  uint8_t option);

#endif
