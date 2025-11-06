#include <stdio.h>
#include <stdlib.h>

#define MAX_FILE_SIZE 4096
#define MAX_KEY_LENGTH 256

int extract_key_from_template(const char *template, char *key, int *key_size);

int extract_key_from_template(const char *template, char *key, int *key_size) {
    int8_t opened_bracket = 0;
    int8_t can_collect = 0;
    
    // change limit to strlen?
    for (int i = 0; i < MAX_FILE_SIZE; i++) {
        if (template[i] == '{') {
            if (opened_bracket == 1) {return -1;} // twice opened
            can_collect = 1;
            opened_bracket++;
            continue;
        }

        if (template[i] == '}') {
            if (opened_bracket == -1) {return -1;} // twice closed
            can_collect = 0;
            opened_bracket--;
            continue;
        }
       
        if (can_collect && *key_size < MAX_KEY_LENGTH) {
            key[*key_size] = template[i];
            (*key_size)++;
        }
    }

    if (*key_size == 0 || opened_bracket != 0) {
        return -1;
    }

    return 0;
}