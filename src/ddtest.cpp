#include <fmt/core.h>

#include <duckdb.h>
#include <duckdb/common/arrow/arrow.hpp>

#include "quack.hpp"

int main() {
  auto dbm = DBManager();

  setup_db(dbm.con);

  // arrow_query_db(dbm.con);
  query_db(dbm.con);

  return 0;
}
