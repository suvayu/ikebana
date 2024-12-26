#include <fmt/core.h>

#include <duckdb/common/arrow/arrow.hpp>

#include "quack.hpp"

void setup_db(duckdb_connection &con) {
  DUCKDB_STATE_OK(
      duckdb_query(con, "CREATE TABLE test (id INTEGER, name VARCHAR);", NULL),
      "failed: to CREATE TABLE {?}", "test");

  DUCKDB_STATE_OK(
      duckdb_query(
          con, "INSERT INTO test VALUES (1, 'Suv'), (2, null), (2, 'NotSuv');",
          NULL),
      "failed: to INSERT INTO {?} VALUES", "test");
}

void query_db(duckdb_connection &con) {
  duckdb_result result;

  DUCKDB_STATE_OK(duckdb_query(con, "SELECT * FROM test;", &result),
                  "failed: SELECT FROM {?}", "test");

  while (true) {
    duckdb_data_chunk chunk = duckdb_fetch_chunk(result);
    if (!chunk) {
      break;
    }
    idx_t row_count = duckdb_data_chunk_get_size(chunk);

    duckdb_vector col1 = duckdb_data_chunk_get_vector(chunk, 0);
    int32_t *col1_data = (int32_t *)duckdb_vector_get_data(col1);
    uint64_t *col1_validity = duckdb_vector_get_validity(col1);

    duckdb_vector col2 = duckdb_data_chunk_get_vector(chunk, 1);
    duckdb_string_t *col2_data =
        (duckdb_string_t *)duckdb_vector_get_data(col2);
    uint64_t *col2_validity = duckdb_vector_get_validity(col2);

    for (idx_t row = 0; row < row_count; row++) {
      if (duckdb_validity_row_is_valid(col1_validity, row)) {
        fmt::print("{:d}, ", col1_data[row]);
      } else {
        fmt::print("NULL, ");
      }
      if (duckdb_validity_row_is_valid(col2_validity, row)) {
        fmt::print("{:d}", col2_data[row].value.pointer.length);
      } else {
        fmt::print("NULL");
      }
      fmt::print("\n");
    }
    duckdb_destroy_data_chunk(&chunk);
  }

  duckdb_destroy_result(&result);
}

void arrow_query_db(duckdb_connection &con) {
  duckdb_arrow arrow_result;
  duckdb_arrow_array arrow_array;

  fmt::print("query:\n");
  DUCKDB_STATE_OK(duckdb_query_arrow(con, "SELECT * FROM test;", &arrow_result),
                  "failed: SELECT FROM {?}", "test");
  DUCKDB_STATE_OK(duckdb_query_arrow_array(arrow_result, &arrow_array),
                  "failed: to get *ArrowArray from {?}", "test");

  fmt::print("ncols: {}\n", duckdb_arrow_column_count(arrow_result));
  fmt::print("nrows: {}\n", duckdb_arrow_row_count(arrow_result));
  fmt::print("done!\n");

  // auto len = arrow_array->internal_ptr;
  auto *array = reinterpret_cast<ArrowArray *>(arrow_array);
  fmt::print("Array info: len={len}, nulls={nulls}, buffers={nbufs}, "
             "children={nchilds}\n",
             fmt::arg("len", array->length),
             fmt::arg("nulls", array->null_count),
             fmt::arg("nbufs", array->n_buffers),
             fmt::arg("nchilds", array->n_children));
  // fmt::print("Array info: len={len}", fmt::arg("len", array->length));
  duckdb_destroy_arrow(&arrow_result);
  fmt::print("destroyed!\n");
}
