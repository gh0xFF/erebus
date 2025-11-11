#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "instance.h"
#include "security.h"

#ifdef NDEBUG 

#include "debug.h"

#endif

void print_help(void);

int main(int argc, char *argv[]) {

    #ifdef NDEBUG
    setup_signal_handlers();
    #endif

    int status = lock_process_memory();
    if (status == -1) {
        #ifdef PARANOIC
            status = 1;
            goto __exit;
        #endif

        // ok lets continue
    }

    char *template = NULL;
    char *msg = NULL;

    // 256 - usual len for filename on modern fs
    // may be a problem if we are using full filepath from root to file
    template = (char*)calloc(256, sizeof(char));
    if (!template) {
        status = 1;
        goto __exit;
    }

    msg = (char*)calloc(256, sizeof(char));
    if (!msg) {
        status = 1;
        goto __exit;
    }
    
    uint16_t offset = 0;
    uint8_t option = 0;
    uint8_t mode = CLI_MODE;
    int opt;

    // ./erebus -t ./tests/data/instance_template.txt -m ./tests/data/encrypted_instance_msg.txt -o 1 -d
    while ((opt = getopt(argc, argv, "t:m:o:dei")) != -1) {
        switch (opt) {
            case 'i':
                mode = INTERACTIVE_MODE;
                break;
            case 't':
                strcpy(template, optarg);
                break;
            case 'm':
                strcpy(msg, optarg);
                break;
            case 'o':
                offset = (uint16_t)atol(optarg);
                break;
            case 'd':
                option = OPTION_DECRYPT;
                break;
            case 'e':
                option = OPTION_ENCRYPT;
                break;
            default:
                print_help();
                goto __exit;
        }
    }

    if (mode == CLI_MODE) {
        if (template == NULL || sizeof(template) == 0 || msg == NULL || sizeof(msg) == 0) {
            print_help();
            status = 1;
            goto __exit;
        }

        if (option != OPTION_DECRYPT && option != OPTION_ENCRYPT) {
            fprintf(stderr, "ivalid option value\n");
            status = 1;
            goto __exit;
        }

        #ifdef PARANOIC
        if (offset >= 16) {
            fprintf(stderr, "paranoic mode in action, you are already used same key 16 times. Create new template and send it\n");
            status = 1;
            goto __exit;
        }
        #endif
    }
    

    status = run_app(template, msg, offset, option, mode);

__exit:
    if (template != NULL) free(template);
    if (msg != NULL)      free(msg);

    #ifdef NDEBUG
    memory_report();
    #endif

    exit(status);
}

void print_help(void) {
    fprintf(
        stdout,
        "version 0.0.4\n\n"
        "\033[1;31mNO WARRANTIES UNTIL VERSION 0.1.0\033[0m\n\n"
        "usage: erebus -t <template> -m <message> -o <offset> -d|e\n"
        "example: erebus -t template.txt -m msg.txt -o 1 -d\n\n"
        "-i to run app in interactive mode\n"
        "-t to choose path to template\n"
        "-m to choose path to message\n"
        "-o to set offset for generator\n"
        "-d to decrypt message\n"
        "-e to encrypt message\n"
    );
}
