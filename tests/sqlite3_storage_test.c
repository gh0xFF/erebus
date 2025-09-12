#include "../src/sqlite3_storage.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

void init_stroage_test(void);
void sqlite3_flow_test(void);

void init_stroage_test(void) {
    fprintf(stdout, "\tTEST init storage.....................................................");
    {
        sqlite3 *db = NULL;
        assert(init_storage(&db) == 0);
        assert(ping_storage(db) == 0);
        assert(close_storage(db) == 0);
    }
    fprintf(stdout, "OK\n");
}

void sqlite3_flow_test(void) {
    fprintf(stdout, "\tTEST init storage.....................................................");
    {
        remove("storage.db");

        sqlite3 *db = NULL;
        assert(init_storage(&db) == 0);
        assert(ping_storage(db) == 0);

        assert(insert_dialog(db, "testuse01", "key") == 0);

        storage *st = (storage*)calloc(1, sizeof(storage));
        assert(get_dialogs(db, st) == 0);
        assert(st != NULL);

        assert(st->count == 1);
        assert(st->data->relative_id == 1);
        assert(st->data->offset == 0);
        assert(strcpy(st->data->username, "testuse01"));
        assert(strcpy(st->data->key, "key"));

        assert(update_dialog(db, 1, "testuse01") == 0);

        // update will update db, to get actual data need to read from db
        st = NULL;
        st = (storage*)calloc(1, sizeof(storage));
        assert(get_dialogs(db, st) == 0);
        assert(st != NULL);

        assert(st->count == 1);
        assert(st->data->relative_id == 1);
        assert(st->data->offset == 1);
        assert(strcpy(st->data->username, "testuse01"));
        assert(strcpy(st->data->key, "key"));

        assert(insert_dialog(db, "testuse02", "key") == 0);

        st = NULL;
        st = (storage*)calloc(1, sizeof(storage));
        assert(get_dialogs(db, st) == 0);
        assert(st != NULL);

        assert(st->count == 2);

        dialog *d1 = &st->data[0];
        assert(d1->relative_id == 1);
        assert(d1->offset == 1);
        assert(strcpy(d1->username, "testuse01"));
        assert(strcpy(d1->key, "key"));

        dialog *d = &st->data[1];
        assert(d->relative_id == 2);
        assert(d->offset == 0);
        assert(strcpy(d->username, "testuse02"));
        assert(strcpy(d->key, "key"));

        assert(close_storage(db) == 0);
        remove("storage.db");
    }
    fprintf(stdout, "OK\n");
}