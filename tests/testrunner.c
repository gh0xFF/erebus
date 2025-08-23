#include <stdio.h>

extern void adler32_test(void);
extern void parse_template_test(void);
extern void instance_test(void);
extern void read_message_test(void);
extern void write_message_test(void);
extern void chacha20_test(void);
extern void garbage_generator_test(void);

int main(void) {
    fprintf(stdout, "RUN  adler32....................................................................\n");
    adler32_test();
    fprintf(stdout, "RUN  adler32..................................................................OK\n");

    fprintf(stdout, "RUN  chacha20...................................................................\n");
    chacha20_test();
    fprintf(stdout, "RUN  chacha20.................................................................OK\n");

    fprintf(stdout, "RUN  template...................................................................\n");
    parse_template_test();
    fprintf(stdout, "RUN  template.................................................................OK\n");

    fprintf(stdout, "RUN  instance...................................................................\n");
    instance_test();
    fprintf(stdout, "................................................................................\n");
    read_message_test();
    fprintf(stdout, "................................................................................\n");
    write_message_test();
    fprintf(stdout, "................................................................................\n");

    fprintf(stdout, "RUN  instance.................................................................OK\n");

   

}