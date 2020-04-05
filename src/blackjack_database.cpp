//
// Created by richard on 12/31/19.
//

#include <iostream>
#include "blackjack_database.h"

blackjack_database::blackjack_database() {
}

int blackjack_database::create() {
    using namespace std;
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql;

/* Open database */
    rc = sqlite3_open("test.db", &db);

    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return (0);
    } else {
        fprintf(stdout, "Opened database successfully\n");
    }

/* Create SQL statement */
    sql = "CREATE TABLE COMPANY("  \
      "ID INT PRIMARY KEY     NOT NULL," \
      "NAME           TEXT    NOT NULL," \
      "AGE            INT     NOT NULL," \
      "ADDRESS        CHAR(50)," \
      "SALARY         REAL );";

/* Execute SQL statement */
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    } else {
        fprintf(stdout, "Table created successfully\n");
    }
    sqlite3_close(db);
}

blackjack_database::~blackjack_database() = default;