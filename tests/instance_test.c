#include "../src/instance.h"
#include "../src/adler32.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int extract_key_from_templatefile(const char* template, char* key);
int read_message_from_file(const char *msg, char *msg_buffer);
int writeout_message(char *fname, char *msg);

void instance_test(void);
void read_message_test(void);
void write_message_test(void);

void instance_test(void) {
    fprintf(stdout, "\tTEST empty filename...................................................");
    {
        char *buffer = (char*)calloc(256, sizeof(char));
        int status = extract_key_from_templatefile("", buffer);
        assert(status == -1);

        free(buffer);
    }
    fprintf(stdout, "OK\n");

    fprintf(stdout, "\tTEST file not exists..................................................");
    {
        char *buffer = (char*)calloc(256, sizeof(char));
        int status = extract_key_from_templatefile("not_exists.txt", buffer);
        assert(status == -1);

        free(buffer);
    }
    fprintf(stdout, "OK\n");

    fprintf(stdout, "\tTEST invalid template.................................................");
    {
        char *buffer = (char*)calloc(256, sizeof(char));
        int status = extract_key_from_templatefile("tests/data/key1.txt", buffer);
        assert(status == -1);

        free(buffer);
    }
    fprintf(stdout, "OK\n");

    fprintf(stdout, "\tTEST too short key in template........................................");
    {
        char *buffer = (char*)calloc(4096, sizeof(char));

        int status = extract_key_from_templatefile("./tests/data/key2.txt", buffer);
        assert(status == -1);

        free(buffer);
    }
    fprintf(stdout, "OK\n");

    fprintf(stdout, "\tTEST valid template...................................................");
    {
        char *buffer = (char*)calloc(4096, sizeof(char));

        int status = extract_key_from_templatefile("./tests/data/key3.txt", buffer);
        assert(status == 0);

        char *expected = (char*)calloc(4096, sizeof(char));
        expected = strncpy(expected, (char*)"hehe1hehe2hehe3hehe4\0", 21);
        assert(memcmp(expected, buffer, 4096) == 0);

        free(buffer);
        free(expected);
    }
    fprintf(stdout, "OK\n");

    #ifdef PARANOIC
    fprintf(stdout, "\tTEST invalid paranoic.................................................");
    {
        char *buffer = (char*)calloc(4096, sizeof(char));

        int status = extract_key_from_templatefile("./tests/data/key3.txt", buffer);
        assert(status == -1);

        free(buffer);
    }
    fprintf(stdout, "OK\n");

    fprintf(stdout, "\tTEST valid paranoic..................................................");
    {
        char *buffer = (char*)calloc(4096, sizeof(char));

        int status = extract_key_from_templatefile("./tests/data/key3.txt", buffer);
        assert(status == 0);

        char *expected = (char*)calloc(4096, sizeof(char));
        expected = strncpy(expected, (char*)"hehe1hehe2hehe3hehe4", 20);
        assert(memcmp(expected, buffer, 4096) == 0);

        free(buffer);
        free(expected);
    }
    fprintf(stdout, "OK\n");
    #endif
}

void read_message_test(void) {
    fprintf(stdout, "\tTEST file not exists..................................................");
    {
        char *buffer = (char*)calloc(256, sizeof(char));
        int status = read_message_from_file("./tests/data/not_exists.txt", buffer);
        assert(status == -1);
        assert(errno == 2);

        free(buffer);
    }
    fprintf(stdout, "OK\n");

    fprintf(stdout, "\tTEST file exists......................................................");
    {
        char *buffer = (char*)calloc(4096, sizeof(char));
        int status = read_message_from_file("./tests/data/file1.txt", buffer);
        assert(status == 0);

        char *expected = (char*)calloc(4096, sizeof(char));
        expected = strncpy(expected, (char*)"not a random text\0", 18);
        assert(memcmp(expected, buffer, 4096) == 0);

        free(buffer);
    }
    fprintf(stdout, "OK\n");
}

void write_message_test(void) {
    fprintf(stdout, "\tTEST file read........................................................");
    {
        char * payload = "hehe";
        assert(writeout_message("./tests/data/msg_msg.txt", payload) == 0);

        FILE* msgfile = fopen("./tests/data/encrypted_msg_msg.txt", "r");
        assert(msgfile != NULL);

        char *file_data = (char*)calloc(4096, sizeof(char));
        assert(file_data != NULL);
        size_t n = fread(file_data, 1, 4096, msgfile);
        assert(n == 4);

        char *buffer = (char*)calloc(4096, sizeof(char));
        assert(buffer != NULL);

        memcpy(buffer, payload, 4);

        assert(memcmp(buffer, file_data, 4096) == 0);

        free(buffer);
        free(file_data);
        fclose(msgfile);

        assert(remove("./tests/data/encrypted_msg_msg.txt") == 0);
    }
    fprintf(stdout, "OK\n");
}

