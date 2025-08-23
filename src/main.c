#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "instance.h"
#include "security.h"

void print_help(void);
void print_demo(void);

int main(int argc, char *argv[]) {
    int status = lock_process_memory();
    if (status == -1) {
        #ifdef PARANOIC
            return 1;
        #endif

        // ok lets continue
    }

    // print_demo();

    // TODO add scanf for template in paranoic mode

    // 256 - usual len for filename on modern fs
    // may be a problem if we are using full filepath from root to file
    char *template = (char*)calloc(256, sizeof(char));
    if (template == NULL) {
        return 1;
    }

    char *msg = (char*)calloc(256, sizeof(char));
    if (msg == NULL) {
        return 1;
    }
    
    uint16_t offset = 0;
    uint8_t option = 0;
    int opt;

    while ((opt = getopt(argc, argv, "t:m:o:de")) != -1) {
        switch (opt) {
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
                print_demo();
                print_help();
                return 1;
        }
    }

    if (template == NULL || sizeof(template) == 0 || msg == NULL || sizeof(msg) == 0) {
        print_help();
        return 1;
    }

    if (option != OPTION_DECRYPT && option != OPTION_ENCRYPT) {
        fprintf(stderr, "ivalid option value\n");
        return 1;
    }

    #ifdef PARANOIC
    if (offset >= 16) {
        fprintf(stderr, "paranoic mode in action, you are already used same key 16 times. Create new template and send it\n");
        return 1;
    }
    #endif

    status = run_app(template, msg, offset, option);
    free(msg);
    return 0;
}

void print_help(void) {
    fprintf(
        stdout,
        "version 0.0.1\n\n"
        "\033[1;31mNO WARRANTIES UNTIL VERSION 0.1.0\033[0m\n\n"
        "usage: erebus -t <template> -m <message> -o <offset> -d|e\n"
        "example: erebus -t template.txt -m msg.txt -o 1 -d\n\n"
        "-t to choose path to template\n"
        "-m to choose path to message\n"
        "-o to set offset for generator\n"
        "-d to decrypt message\n"
        "-e to encrypt message\n"
    );
}

void print_demo(void) {
    const char * img = 
        "                                                            \n"
        "            ...                         ....                \n"
        "           ..                             ..                \n"
        "          ..                                                \n"
        "         ...               .                                \n"
        "         ..               ..    ...                         \n"
        "    .    ..          ... ..  .......                       .\n"
        "   ..    ..          ... .  .            .              .  .\n"
        "   .                  ....         .                       .\n"
        "  .   .             ...'..   ....                           \n"
        "  .       .        ..',;;,...';'.                     .     \n"
        " .         .        ....';;';'..   .                        \n"
        ".           .        ....',,'.     ..                .      \n"
        "             .      ........  .  .....                      \n"
        "             ...  ...             .  .....                  \n"
        "               ...... ..        ...  .'.                    \n"
        "                ..'..'..   .  ..'.. .'..                    \n"
        "      ..        ..'...........',.   .....   ........        \n"
        "          .....'',;'.  .......''.  .',,,'....               \n"
        "           ...',',;,'......   .. ...,''.'''..               \n"
        ".    .'','...............      ................. ....      .\n"
        ".    .',;'.................,'........................      .\n"
        ".       ........... ......'::'........''.                 ..\n"
        ".   .       ...''.. ..   .;ll,.  ... .','...             ...\n"
        ".   . ......','..         ;Ox.    ..  .'......           ...\n"
        "    .....'.''.      .     'ol.    ..   ..   .            ...\n"
        "   ...''.. ..       .     .;;..  ...               ..     ..\n"
        "  ......            ..    .''.. ....     .         ....     \n"
        "  .'.                ..   ....  ..       .            .    .\n"
        "                      .'. ..... ..     ..                  .\n"
        "          ....        .....''.  ..  ...                     \n"
        "      .   .......   .....  ..   ....        ..              \n"
        "       ....           ...                                ...\n"
        "        ....                                              ..\n"
        ".         .......                                         ..\n"
        "            .                                              .\n"
        " . .                                                       .\n"
        "   ..                                                      .\n"
        "   ...                                          ..         .\n"
        "    ..        .                               ....          \n";
                                                            

    fprintf(
        stdout,
        "%s",
        img
    );
}