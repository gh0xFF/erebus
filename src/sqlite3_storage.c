#include <sqlite3.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3_storage.h"

int close_storage(sqlite3 *db);
int get_dialogs(sqlite3 *db, storage *st);
int update_dialog(sqlite3 *db, int offset, const char* uname);
int insert_dialog(sqlite3 *db, const char* uname, const char* key);
int ping_storage(sqlite3 *db);
int init_storage(sqlite3 **db);
int create_dialog(storage *s, const dialog *d);
int remove_dialog(sqlite3 *db, const char *uname);
int cb(void *data, int cnt, char **cols, char **nms);


int init_storage(sqlite3 **db) {
    if (sqlite3_open("storage.db", db) != SQLITE_OK) {
        fprintf(stderr, "error while creating storage: %s\n", sqlite3_errmsg(*db));
        return -1;
    }

    sqlite3_soft_heap_limit64(1024 * 1024);

    if (ping_storage(*db) != 0) {
        fprintf(stderr, "error while ping storage\n");
        db = NULL;
        return -1;
    }

    // TODO add index for fast search
    char *sql = "CREATE TABLE IF NOT EXISTS dialogs("
        "id INTEGER PRIMARY KEY AUTOINCREMENT," 
        "offset INTEGER DEFAULT 0," 
        "username TEXT DEFAULT ''," 
        "key TEXT DEFAULT '');";

    char *err = NULL;
    if (sqlite3_exec(*db, sql, 0, 0, &err) != SQLITE_OK) {
        fprintf(stderr, "error while inserting dialog: %s\n", sqlite3_errmsg(*db));
        if (err != NULL) {
            sqlite3_free(err);
        }
        sqlite3_close(*db);
        db = NULL;
        return -1;
    }

    return 0;
}

int ping_storage(sqlite3 *db) {
    if (db == NULL) {
        fprintf(stderr, "Database handle is NULL\n");
        return -1;
    }

    sqlite3_stmt *stmt;
    const char *sql = "SELECT 1";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Prepare failed: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        fprintf(stderr, "Step failed: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);
    sqlite3_db_release_memory(db);
    
    return 0;
}

int insert_dialog(sqlite3 *db, const char* uname, const char* key) {
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO dialogs (username, key) VALUES (?, ?)";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "error while inserting dialog: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, uname, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, key, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "error while exec: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_finalize((stmt));
    return 0;
}

int update_dialog(sqlite3 *db, int offset, const char* uname) {
    sqlite3_stmt *stmt;
    const char *sql = "UPDATE dialogs SET offset = ? WHERE username = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "error while inserting dialog: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, offset);
    sqlite3_bind_text(stmt, 2, uname, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "error while exec: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_finalize((stmt));
    return 0;
}

int create_dialog(storage *s, const dialog *d) {
    if (!s || !d) return -1;
    
    dialog *new_data = (dialog*)realloc(s->data, (s->count + 1) * sizeof(dialog));
    if (!new_data) return -1;
    
    s->data = new_data;
    s->data[s->count] = *d;
    s->count++;
    
    return 0;
}

int cb(
    void *data,
    int cnt, 
    char **cols, 
    char **nms __attribute__((unused))
) {
    storage *s = (storage*)data;

    if (cnt < 4) {
        fprintf(stderr, "Warning: Expected 4 columns, got %d\n", cnt);
        return 0;
    }

    if (!cols[0] || !cols[1] || !cols[2] || !cols[3]) {
        fprintf(stderr, "Warning: Missing data in dialog row\n");
        return 0;
    }

    dialog d = {
        .relative_id = (uint16_t)atol(cols[0]),
        .offset = (uint16_t)atol(cols[1]),
    };

    strncpy(d.username, cols[2] ? cols[2] : "", sizeof(d.username) - 1);
    d.username[sizeof(d.username) - 1] = '\0';
    
    strncpy(d.key, cols[3] ? cols[3] : "", sizeof(d.key) - 1);
    d.key[sizeof(d.key) - 1] = '\0';

    create_dialog(s, &d);
    return 0;
}

int get_dialogs(sqlite3 *db, storage *st) {
    if (!st) return -1;
    
    char *err = 0;
    char *sql = "SELECT id, offset, username, key FROM dialogs";
    
    if (sqlite3_exec(db, sql, cb, st, &err) != SQLITE_OK) {
        fprintf(stderr, "Error while loading dialogs: %s\n", err);
        sqlite3_free(err);
        return -1;
    }

    return 0;
}

int remove_dialog(sqlite3 *db, const char * uname) {
    sqlite3_stmt *stmt;
    const char *sql = "DELETE FROM dialogs where username = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "error while removing dialog: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, uname, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        fprintf(stderr, "error while exec: %s\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_finalize(stmt);
    return 0;
}

int close_storage(sqlite3 *db) {
    return sqlite3_close(db);
}