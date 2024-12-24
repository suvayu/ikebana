#pragma once

#ifndef _quack_h_
#define _quack_h_

#include <duckdb.h>

#include "status.h"

struct DBManager {
  duckdb_database db;
  duckdb_connection con;

  DBManager(const char *path) {
    DUCKDB_STATE_OK(duckdb_open(path, &db), "failed: to open db: {?}", path);
    DUCKDB_STATE_OK(duckdb_connect(db, &con), "failed: to connect: {?}", path);
  }

  DBManager() {
    in_memory = true;
    DUCKDB_STATE_OK(duckdb_open(NULL, &db), "failed: to open in-memory db");
    DUCKDB_STATE_OK(duckdb_connect(db, &con), "failed: to connect: in-memory");
  }

  /*
  ~DBManager() {
    duckdb_disconnect(&con);
    duckdb_close(&db);
  }
  */

private:
  bool in_memory{false};
};

#endif // _quack_h_
