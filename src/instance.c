#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <sqlite3.h>
#include "sqlite3_storage.h"
#include "template.h"
#include "chacha20.h"
#include "adler32.h"

#define OPTION_DECRYPT 1
#define OPTION_ENCRYPT 2

#define CLI_MODE 0
#define INTERACTIVE_MODE 1

#define MAX_FILE_SIZE 4096
#define MAX_KEY_LENGTH 256

#define CLEAR_STDOUT "\\e[3J" //"\033[2J"

int run_app( char *template,  char *msg,  uint16_t offset,  uint8_t option, uint8_t mode);
int extract_key_from_templatefile(const char* template, char* key);
int read_message_from_file(const char *msg, char *msg_data);
int writeout_message(char *fname, char *msg);
void garbage_generator(char *msg, uint16_t len);
void add_garbage_to_msg(char *msg);
char* transform_filename(const char* input);
int run_interactive_mode(sqlite3 *db);
void chacha20_generator(char* message_buffer, char* key, uint16_t offset);
int run_cli_mode(sqlite3 *db, char* template, char* msg, uint16_t offset, uint8_t option);

int run_app(
    char* template,  
    char* msg,  
    uint16_t offset,  
    uint8_t option,
    uint8_t mode
) {

    sqlite3 *db = NULL;
    if (init_storage(&db) != 0) {
        fprintf(stderr, "can't open storage\n");
        return -1;
    }

    if (ping_storage(db) != 0) {
        fprintf(stderr, "error while ping storage\n");
        return -1;
    }
    
    int status = 0;
    switch (mode) {
        case CLI_MODE:
            status = run_cli_mode(db, template, msg, offset, option);
            break;
        case INTERACTIVE_MODE:
            status = run_interactive_mode(db);
            break;
        default:
            fprintf(stderr, "unsupported mode\n");
            status = -1;
    }

    return status;
}
int run_cli_mode(
    sqlite3 *db __attribute__((unused)),
    char* template,  
    char* msg,  
    uint16_t offset,  
    uint8_t option
) {
    char *key = (char*)calloc(MAX_KEY_LENGTH, 1);
    if(key == NULL) {
        return -1;
    }

    if (extract_key_from_templatefile(template, key) == -1) {
        free(key);
        fprintf(stderr, "error while getting key from template file\n");
        return -1;
    }

    char *message_buffer = (char*)calloc(MAX_FILE_SIZE, 1);
    if(message_buffer == NULL) {
        free(key);
        return -1;
    }

    if (read_message_from_file(msg, message_buffer) == -1) {
        free(message_buffer);
        free(key);
        fprintf(stderr, "error while getting message from file\n");
        return -1;
    }

    if (option == OPTION_DECRYPT) {
        /* for now do nothing; todo add extractors to print message without garbage */
    } 
    
    if (option == OPTION_ENCRYPT) {
        garbage_generator(message_buffer, (uint16_t)strlen(message_buffer));
    }

    chacha20_generator(message_buffer, key, offset);

    if (writeout_message(msg, message_buffer) == -1) {
        fprintf(stderr, "error while writing message file to disk\n");
        return -1;
    }

    free(key);
    free(message_buffer);
    return 0;
}

int run_interactive_mode(sqlite3 *db) {
    /*
       There's an inconsistent state issue; according to the mutated fields in the database, they don't correspond to the memory access.
        We need to synchronize them.
        One option is to rework the database access methods ourselves.
        The best option is to monitor mutagen calls from the upper layers and update the memory in this case.
    */
    int status = 0;
    char *command_buffer = NULL;
    size_t size = 0;
    ssize_t len = 0;
    storage *st = (storage*)calloc(1, sizeof(storage));

    if (get_dialogs(db, st) != 0) {
        goto __exit;
    }

    while (1) {
        fprintf(stdout, 
            "%s\n"
            "press 'q' to exit or choose dialog for option:\n"
            "press 'a' to add new dialog:\n"
            "press 's' to show dialogs, you have %d:\n",
            CLEAR_STDOUT, st->count
        );
       
        if ((len = getline(&command_buffer, &size, stdin)) == -1) {
            status = -1;
            goto __exit;
        }
    
        command_buffer[strcspn(command_buffer, "\n")] = '\0';

        printf("[DEBUG] pressed [%s]\n", command_buffer);

    
        if (strcmp(command_buffer, "q") == 0) {
            status = 0;
            goto __exit;
        }

        if (strcmp(command_buffer, "a") == 0) {
            fprintf(stdout, "enter usrsname: ");

            char *username = NULL;
            if ((len = getline(&username, &size, stdin)) == -1) {
                fprintf(stderr, "input error\n");
                status = -1;
                goto __exit;
            }

            if (size == 0) {
                fprintf(stdout, "empty arg\n");
                break;
            }

            fprintf(stdout, "enter key: ");

            char *key = NULL;
            if ((len = getline(&key, &size, stdin)) == -1) {
                fprintf(stderr, "input error\n");
                status = -1;
                goto __exit;
            }

            if (size == 0) {
                fprintf(stdout, "empty arg\n");
                break;
            }

            if (insert_dialog(db, username, key) == -1) {
                fprintf(stderr, "error while inserting dialog\n");
                status = -1;
                goto __exit;
            }
        }

        if (strcmp(command_buffer, "s") == 0) {
            if (st->count == 0) {
                fprintf(stdout, "no available dialogs");
            } else {
                for(int i = 0; i < st->count; i++) {
                    fprintf(stdout, "[%d] %s\n", i, st->data[i].username);
                }
            }
        }
    }

__exit:
    free(command_buffer);
    return status;
}


char* transform_filename(const char* input) {
    const char* last_slash = strrchr(input, '/');
    const char* filename_start = (last_slash != NULL) ? last_slash + 1 : input;
    const char* dot = strrchr(filename_start, '.');
    const char* encrypted_prefix = strstr(filename_start, "encrypted_");
    
    size_t prefix_len = (size_t)(filename_start - input);
    size_t result_len;
    char* result;
    
    if (encrypted_prefix != NULL && encrypted_prefix == filename_start) {
        result_len = strlen(input) - strlen("encrypted_") + strlen("decrypted_") + 1;
        result = malloc(result_len);
        if (result == NULL) {
            return NULL;
        }
        
        if (prefix_len > 0) {
            strncpy(result, input, prefix_len);
            result[prefix_len] = '\0';
        } else {
            result[0] = '\0';
        }
        
        strcat(result, "decrypted_");
        strcat(result, encrypted_prefix + strlen("encrypted_"));
    }
    else if (dot != NULL) {
        result_len = strlen(input) + strlen("encrypted_") + 1;
        result = malloc(result_len);
        if (result == NULL) {
            return NULL;
        }
        
        if (prefix_len > 0) {
            strncpy(result, input, prefix_len);
            result[prefix_len] = '\0';
        } else {
            result[0] = '\0';
        }
        
        strcat(result, "encrypted_");
        strcat(result, filename_start);
    } else {
        result_len = strlen(input) + strlen("encrypted_") + 1;
        result = malloc(result_len);
        if (result == NULL) {
            return NULL;
        }
        
        strcpy(result, input);
        strcat(result, "encrypted_");
    }
    
    return result;
}
int writeout_message(char *path, char *msg) {
    char *fname = transform_filename(path);
    if (fname == NULL) {
        return -1;
    }

    FILE* outfile = fopen(fname, "wb+");
    if(!outfile) {
        fprintf(stderr, "can't write outfile\n");
        free(fname);
        return -1;
    }

    size_t wrote = fwrite(msg, 1, strlen(msg), outfile);

    if (wrote != (strlen(msg))) {
        fprintf(stderr, "error while trying to write out file, wrote: %ld, len of data %ld\n", wrote, sizeof(msg));
        fclose(outfile); // todo
        free(fname);
        return -1;
    }

    if (fclose(outfile) == -1) {
        free(fname);
        fprintf(stderr, "can't close message file correctly\n");
        return -1;
    }

    free(fname);
    return 0;
}

int read_message_from_file(const char *msg, char *msg_buffer) {
    if(msg_buffer == NULL) {
        return -1;
    }

    FILE* message_file = fopen(msg, "r");
    if(!message_file) {
        fprintf(stderr, "failed to open file\n");
        return -1;
    }

    fseek(message_file, 0, SEEK_END);
    long real_size = ftell(message_file);
    fseek(message_file, 0, SEEK_SET);

    char *tmp = (char*)calloc(MAX_FILE_SIZE, 1);
    if(tmp == NULL) {
        fclose(message_file);
        return -1;
    }

    size_t bytes_read = fread(tmp, 1, (size_t)real_size, message_file);
    if (bytes_read != strlen(tmp)) {
        fprintf(stderr, "broken file, diff in bytes %d\n", abs(((int)(strlen(tmp)-bytes_read))));
        fclose(message_file);
        free(tmp);
        return -1;
    }

    strncpy(msg_buffer, tmp, (size_t)real_size);

    if (fclose(message_file) == -1) {
        free(tmp);
        fprintf(stderr, "can't close message file correctly\n");
        return -1;
    }

    free(tmp);
    return 0;
}

int extract_key_from_templatefile(const char* template, char* key) {
    if (template == NULL) {
        return -1;
    }

    FILE* template_file = fopen(template, "r");
    if (!template_file) {
        fprintf(stderr, "Failed to open file\n");
        fclose(template_file);
        return -1;
    }

    int key_len = 0;
    char *buffer = (char*)calloc(MAX_FILE_SIZE, 1);
    if (buffer == NULL) {
        fclose(template_file);
        return -1;
    }

    size_t bytes_read = fread(buffer, 1, MAX_FILE_SIZE, template_file);
    if (bytes_read == 0) {
        fprintf(stderr, "0 bytes read");
        free(buffer);
        fclose(template_file);
        return -1;
    }

    if (extract_key_from_template(buffer, key, &key_len) == -1) {
        free(buffer);

        fprintf(stderr, "can't read key from template\n");
        fclose(template_file);
        return -1;
    }

    if (fclose(template_file) == -1) {
        free(buffer);

        fprintf(stderr, "can't close template file correctly\n");
        return -1;
    }

    #ifdef PARANOIC
    if (key_len < 64) {
        free(buffer);

        fprintf(stderr, "key from template must be at least 64 symbols\n");
        return -1;
    }
    #else
    if (key_len < 16) {
        free(buffer);

        fprintf(stderr, "key from template must be at least 16 symbols\n");
        return -1;
    }
    #endif
        free(buffer);

    return 0;
}

void garbage_generator(char *msg, uint16_t len) {
    char *new_msg = (char *)calloc(MAX_FILE_SIZE, 1);
    if (new_msg == NULL) {
        return;
    }
    
    srand((unsigned int)time(NULL)+(unsigned int)getpid());

    for (uint16_t i = 0; i < MAX_FILE_SIZE; i++) {
        new_msg[i] = (char)(rand() % (127 - 32 + 1) + 32);
    }

    memcpy(new_msg, msg, (size_t)len);
    memcpy(msg, new_msg, MAX_FILE_SIZE);

    free(new_msg);
}
