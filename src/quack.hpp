#pragma once
#ifndef _quack_h_
#define _quack_h_

#include <string>
#include <type_traits>
#include <vector>

#include <fmt/core.h>

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

template <typename T>
void format_vec_data(const T *data, uint64_t *validity,
                     std::vector<std::vector<std::string>> &res, idx_t col,
                     idx_t row_start, idx_t nrows) {
  for (idx_t row = 0; row < nrows; ++row) {
    if (duckdb_validity_row_is_valid(validity, row)) {
      if constexpr (std::is_same_v<duckdb_string_t, T>) {
        res[row_start + row][col] =
            fmt::format("{:s}", duckdb_string_is_inlined(data[row])
                                    ? data[row].value.inlined.inlined
                                    : data[row].value.pointer.ptr);
      } else {
        res[row_start + row][col] = fmt::format("{}", data[row]);
      }
    } else {
      res[row_start + row][col] = fmt::format("NULL");
    }
  }
}

void setup_db(duckdb_connection &con);
std::vector<std::vector<std::string>> query_db(duckdb_connection &con);
void arrow_query_db(duckdb_connection &con);

#endif // _quack_h_
