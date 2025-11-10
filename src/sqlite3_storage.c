#include <sqlite3.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3_storage.h"

// int close_storage(storage *st);
// int get_dialogs(storage *st);
// int update_dialog(storage *st, int offset, const char* uname);
// int insert_dialog(storage *st, const char* uname, const char* key);
// int ping_storage(storage *st);
// int init_storage(storage *st);
// int create_dialog(storage *s, const dialog *d);
// int remove_dialog(storage *st, const char *uname);
int cb(void *data, int cnt, char **cols, char **nms);
int set_sql_error_safe(const char* format, ...);

int init_storage(storage *st) {
    if (!st) {
        set_sql_error_safe("null instance storage");
        return -1;
    }

    st->count = 0;
    st->data = NULL;

    st->db = (sqlite3**)calloc(1, sizeof(sqlite3*));
    if (st->db == NULL) {
        set_sql_error_safe("cannot allocate memory for dialogs");
        return -1;
    }

    if (sqlite3_open("storage.db", st->db) != SQLITE_OK) {
        set_sql_error_safe("error while creating storage: %s", sqlite3_errmsg(*(st->db)));
        return -1;
    }

    sqlite3_soft_heap_limit64(1024 * 1024);

    if (ping_storage(st) != 0) {
        set_sql_error_safe("error while ping storage");
        free(st->db);
        return -1;
    }

    // TODO add index for fast search
    const char *sql = "CREATE TABLE IF NOT EXISTS dialogs("
        "id INTEGER PRIMARY KEY AUTOINCREMENT," 
        "offset INTEGER DEFAULT 0," 
        "username TEXT DEFAULT ''," 
        "key TEXT DEFAULT '');";

    char *err = NULL;
    if (sqlite3_exec(*(st->db), sql, 0, 0, &err) != SQLITE_OK) {
        set_sql_error_safe("error while creating data schema: %s", sqlite3_errmsg(*(st->db)));
        if (err != NULL) {
            sqlite3_free(err);
        }
        sqlite3_close(*(st->db));
        st->db = NULL;
        return -1;
    }

    return 0;
}

int ping_storage(storage *st) {
    if (st == NULL) {
        set_sql_error_safe("storage is null");
        return -1;
    }

    if (st->db == NULL) {
        set_sql_error_safe("database handle is null");
        return -1;
    }

    sqlite3_stmt *stmt;
    const char *sql = "SELECT 1";
    
    if (sqlite3_prepare_v2(*(st->db), sql, -1, &stmt, NULL) != SQLITE_OK) {
        set_sql_error_safe("prepare failed: %s\n", sqlite3_errmsg(*(st->db)));
        return -1;
    }

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        set_sql_error_safe("step failed: %s\n", sqlite3_errmsg(*(st->db)));
        sqlite3_finalize(stmt);
        return -1;
    }

    sqlite3_finalize(stmt);
    sqlite3_db_release_memory(*(st->db));
    
    return 0;
}

int insert_dialog(storage *st, const char* uname, const char* key) {
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO dialogs (username, key) VALUES (?, ?)";

    if (sqlite3_prepare_v2(*(st->db), sql, -1, &stmt, 0) != SQLITE_OK) {
        set_sql_error_safe("error while inserting dialog: %s\n", sqlite3_errmsg(*(st->db)));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, uname, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, key, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        set_sql_error_safe("error while inserting dialog: %s\n", sqlite3_errmsg(*(st->db)));
        return -1;
    }

    sqlite3_finalize((stmt));

    return get_dialogs(st);
}

int update_dialog(storage *st, int offset, const char* uname) {
    sqlite3_stmt *stmt;
    const char *sql = "UPDATE dialogs SET offset = ? WHERE username = ?";

    if (sqlite3_prepare_v2(*(st->db), sql, -1, &stmt, 0) != SQLITE_OK) {
        set_sql_error_safe("error while updating dialog: %s\n", sqlite3_errmsg(*(st->db)));
        return -1;
    }

    sqlite3_bind_int(stmt, 1, offset);
    sqlite3_bind_text(stmt, 2, uname, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        set_sql_error_safe("error while updating dialog: %s\n", sqlite3_errmsg(*(st->db)));
        return -1;
    }

    sqlite3_finalize((stmt));

    return get_dialogs(st);
}

// NOT SQLITE ACTIVITIES!!!
int create_dialog(storage *st, const dialog *d) {
    if (!st || !d) return -1;
    
    dialog *new_data = (dialog*)realloc(st->data, (st->count + 1) * sizeof(dialog));
    if (!new_data) return -1;
    
    st->data = new_data;
    st->data[st->count] = *d;
    st->count++;
    
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
        snprintf(SQL_ERR_REASON, sizeof(SQL_ERR_REASON), "callback error num of columns\n");
        return 0;
    }

    if (!cols[0] || !cols[1] || !cols[2] || !cols[3]) {
        snprintf(SQL_ERR_REASON, sizeof(SQL_ERR_REASON), "callback error missing data in row\n");
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

    return create_dialog(s, &d);
}

int get_dialogs(storage *st) {
    if (!st) return -1;
    
    char *err = 0;
    const char *sql = "SELECT id, offset, username, key FROM dialogs";
    
    st->count = 0;

    if (sqlite3_exec(*(st->db), sql, cb, st, &err) != SQLITE_OK) {
        set_sql_error_safe("error while loading dialog: %s\n", sqlite3_errmsg(*(st->db)));
        sqlite3_free(err);
        return -1;
    }

    return 0;
}

int remove_dialog(storage *st, const char * uname) {
    sqlite3_stmt *stmt;
    const char *sql = "DELETE FROM dialogs where username = ?";

    if (sqlite3_prepare_v2(*(st->db), sql, -1, &stmt, 0) != SQLITE_OK) {
        set_sql_error_safe("error while removing dialog: %s\n", sqlite3_errmsg(*(st->db)));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, uname, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        set_sql_error_safe("error while removing dialog: %s\n", sqlite3_errmsg(*(st->db)));
        return -1;
    }

    sqlite3_finalize(stmt);

    return get_dialogs(st);
}

int close_storage(storage *st) {
    if (sqlite3_close(*(st->db)) != SQLITE_DONE) {
        set_sql_error_safe("error while closing storage: %s\n", sqlite3_errmsg(*(st->db)));
        return -1;
    }

    return 0;
}

int set_sql_error_safe(const char* format, ...) {
    va_list args;
    va_start(args, format);
    int result = vsnprintf(SQL_ERR_REASON, sizeof(SQL_ERR_REASON), format, args);
    va_end(args);
    
    if ((unsigned long)result >= sizeof(SQL_ERR_REASON)) {
        return -1;
    }
    return 0;
}

const char* get_sql_error(void) {
    return SQL_ERR_REASON;
}
