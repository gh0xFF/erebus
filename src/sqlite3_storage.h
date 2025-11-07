#ifndef SQLITE3_STORAGE_H
#define SQLITE3_STORAGE_H

#include <stdint.h>
#include <sqlite3.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
static char SQL_ERR_REASON[512];
#pragma GCC diagnostic pop

typedef struct dialog {
    uint16_t    relative_id;
    uint16_t    offset;
    char        username[256];
    char        key[256];
} dialog;

typedef struct storage {
    sqlite3     **db;   // databace instance
    uint16_t    count;  // counter to iterate without != NULL checks
    dialog      *data;  // data
} storage;

int close_storage(storage *st);
int get_dialogs(storage *st);
int update_dialog(storage *st, int offset, const char* uname);
int insert_dialog(storage *st, const char* uname, const char* key);
int create_dialog(storage *st, const dialog *d);
int remove_dialog(storage *st, const char *uname);
int ping_storage(storage *st);
int init_storage(storage *st);

const char* get_sql_error(void);

#endif