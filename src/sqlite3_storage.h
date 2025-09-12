#ifndef SQLITE3_STORAGE_H
#define SQLITE3_STORAGE_H

#include <stdint.h>
#include <sqlite3.h>

typedef struct dialog {
    uint16_t relative_id;
    uint16_t offset;
    char username[256];
    char key[256];
} dialog;

typedef struct storage {
    uint16_t count;
    dialog *data;
} storage;

int close_storage(sqlite3 *db);
int get_dialogs(sqlite3 *db, storage *st);
int update_dialog(sqlite3 *db, int offset, const char* uname);
int insert_dialog(sqlite3 *db, const char* uname, const char* key);
int create_dialog(storage *s, const dialog *d);
int ping_storage(sqlite3 *db);
int init_storage(sqlite3 **db);

#endif