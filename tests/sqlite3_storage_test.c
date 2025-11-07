#include "../src/sqlite3_storage.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sqlite3_flow_test(void);

void sqlite3_flow_test(void) {
    fprintf(stdout, "\tTEST init storage.....................................................");
    {
        remove("storage.db");

        {
            storage *st = NULL;
            assert(ping_storage(st) == -1);
            assert(strcmp(get_sql_error(), "storage is null") == 0);

            st = (storage*)malloc(sizeof(storage));
            assert(ping_storage(st) == -1);
            assert(strcmp(get_sql_error(), "database handle is null") == 0);
        }


        storage *st = (storage*)calloc(1, sizeof(storage));
        assert(st != NULL);

        {
            assert(init_storage(st) == 0);
            assert(ping_storage(st) == 0);
        }
        
        {
            assert(get_dialogs(st) == 0);
            assert(st->count == 0);
            assert(st->data == NULL);
        }

        {
            assert(insert_dialog(st, "testuse01", "key") == 0);

            assert(st->count == 1);
            assert(strcpy(st->data->username, "testuse01"));
            assert(strcpy(st->data->key, "key"));
            assert(st->data->relative_id == 1);
            assert(st->data->offset == 0);
        }

        {
            assert(update_dialog(st, 1, "testuse01") == 0);
            assert(st->count == 1);
            assert(strcpy(st->data->username, "testuse01"));
            assert(strcpy(st->data->key, "key"));
            assert(st->data->relative_id == 1);
            assert(st->data->offset == 1);
        }

        {
            assert(insert_dialog(st, "testuse02", "key") == 0);
            assert(st->count == 2);

            dialog tmp = st->data[0];
            assert(strcpy(tmp.username, "testuse01"));
            assert(strcpy(tmp.key, "key"));
            assert(tmp.relative_id == 1);
            assert(tmp.offset == 1);

            tmp = st->data[1];
            assert(strcpy(tmp.username, "testuse02"));
            assert(strcpy(tmp.key, "key"));
            assert(tmp.relative_id == 2);
            assert(tmp.offset == 0);
        }

        {
            assert(remove_dialog(st, "testuse02") == 0);
            assert(st->count == 1);

            dialog tmp = st->data[0];
            assert(strcpy(tmp.username, "testuse01"));
            assert(strcpy(tmp.key, "key"));
            assert(tmp.relative_id == 1);
            assert(tmp.offset == 1);
        }

        remove("storage.db");
    }
    fprintf(stdout, "OK\n");
}