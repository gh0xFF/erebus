#include "../src/template.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void parse_template_test(void);

void parse_template_test(void) {
    fprintf(stdout, "\tTEST empty template...................................................");
    {
        char *template = (char*)calloc(4096, sizeof(char));
        char *key = (char*)calloc(256, sizeof(char));
        int len = 0;

        int status = extract_key_from_template(template, key, &len);
        assert(status == -1);

        free(template);
        free(key);
    }
    fprintf(stdout, "OK\n");

    fprintf(stdout, "\tTEST invalid template1................................................");
    {
        char *template = (char*)calloc(4096, sizeof(char));
        strcpy(template, (char*)"not hehe");

        char *key = (char*)calloc(256, sizeof(char));
        int len = 0;

        int status = extract_key_from_template(template, key, &len);
        assert(status == -1);

        free(template);
        free(key);
    }
    fprintf(stdout, "OK\n");

    fprintf(stdout, "\tTEST invalid template2................................................");
    {
        char *template = (char*)calloc(4096, sizeof(char));
        strcpy(template, (char*)"select fav beer{}}");

        char *key = (char*)calloc(256, sizeof(char));
        int len = 0;

        int status = extract_key_from_template(template, key, &len);
        assert(status == -1);

        free(template);
        free(key);
    }
    fprintf(stdout, "OK\n");

    fprintf(stdout, "\tTEST invalid template3................................................");
    {
        char *template = (char*)calloc(4096, sizeof(char));
        strcpy(template, (char*)"select fav beer{ipa}}");

        char *key = (char*)calloc(256, sizeof(char));
        int len = 0;

        int status = extract_key_from_template(template, key, &len);
        assert(status == -1);

        free(template);
        free(key);
    }
    fprintf(stdout, "OK\n");

    fprintf(stdout, "\tTEST invalid template4................................................");
    {
        char *template = (char*)calloc(4096, sizeof(char));
        strcpy(template, (char*)"select fav beer{{ipa}");

        char *key = (char*)calloc(256, sizeof(char));
        int len = 0;

        int status = extract_key_from_template(template, key, &len);
        assert(status == -1);

        free(template);
        free(key);
    }
    fprintf(stdout, "OK\n");

    fprintf(stdout, "\tTEST invalid template5................................................");
    {
        char *template = (char*)calloc(4096, sizeof(char));
        strcpy(template, (char*)"select fav beer{{ipa}}");

        char *key = (char*)calloc(256, sizeof(char));
        int len = 0;

        int status = extract_key_from_template(template, key, &len);
        assert(status == -1);

        free(template);
        free(key);
    }
    fprintf(stdout, "OK\n");

    fprintf(stdout, "\tTEST invalid template6................................................");
    {
        char *template = (char*)calloc(4096, sizeof(char));
        strcpy(template, (char*)"select fav beer{{ipa");

        char *key = (char*)calloc(256, sizeof(char));
        int len = 0;

        int status = extract_key_from_template(template, key, &len);
        assert(status == -1);

        free(template);
        free(key);
    }
    fprintf(stdout, "OK\n");

    fprintf(stdout, "\tTEST invalid template7................................................");
    {
        char *template = (char*)calloc(4096, sizeof(char));
        strcpy(template, (char*)"select fav beer ipa}}");

        char *key = (char*)calloc(256, sizeof(char));
        int len = 0;

        int status = extract_key_from_template(template, key, &len);
        assert(status == -1);

        free(template);
        free(key);
    }
    fprintf(stdout, "OK\n");

    fprintf(stdout, "\tTEST invalid template8................................................");
    {
        char *template = (char*)calloc(4096, sizeof(char));
        strcpy(template, (char*)"select fav beer{ipa}, select fav os{plan9");

        char *key = (char*)calloc(256, sizeof(char));
        int len = 0;

        int status = extract_key_from_template(template, key, &len);
        assert(status == -1);

        free(template);
        free(key);
    }
    fprintf(stdout, "OK\n");

    fprintf(stdout, "\tTEST invalid template9................................................");
    {
        char *template = (char*)calloc(4096, sizeof(char));
        strcpy(template, (char*)"select fav beer{ipa}, select fav os{plan9");

        char *key = (char*)calloc(256, sizeof(char));
        int len = 0;

        int status = extract_key_from_template(template, key, &len);
        assert(status == -1);

        free(template);
        free(key);
    }
    fprintf(stdout, "OK\n");

    fprintf(stdout, "\tTEST template too big.................................................");
    {
        char *template = (char*)calloc(4096*2, sizeof(char));
        strcpy(template, (char*)"select fav beer{ipa}, select fav os{plan9");

        char *key = (char*)calloc(256, sizeof(char));
        int len = 0;

        int status = extract_key_from_template(template, key, &len);
        assert(status == -1);

        free(template);
        free(key);
    }
    fprintf(stdout, "OK\n");

    fprintf(stdout, "\tTEST valid template1..................................................");
    {
        char *template = (char*)calloc(4096, sizeof(char));
        strcpy(template, (char*)"select fav beer{ipa}, select fav os{plan9}");

        char *key = (char*)calloc(256, sizeof(char));
        int len = 0;

        int status = extract_key_from_template(template, key, &len);
        assert(status == 0);
        assert(len == 8);
        assert(strlen(key) == 8);

        free(template);
        free(key);
    }
    fprintf(stdout, "OK\n");

    fprintf(stdout, "\tTEST valid template2..................................................");
    {
        char *template = (char*)calloc(4096, sizeof(char));
        strcpy(template, (char*)"назови любимое пиво {охота крепкое}");

        char *key = (char*)calloc(256, sizeof(char));
        int len = 0;

        int status = extract_key_from_template(template, key, &len);
        assert(status == 0);
        assert(len == 25);
        assert(strlen(key) == 25);

        free(template);
        free(key);
    }
    fprintf(stdout, "OK\n");
}