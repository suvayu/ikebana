#include <string>
#include <vector>

#include <fmt/core.h>

#include <duckdb.h>
#include <duckdb/common/arrow/arrow.hpp>

#include "quack.hpp"
#include "status.h"

void setup_db(duckdb_connection &con) {
  DUCKDB_STATE_OK(
      duckdb_query(con, "CREATE TABLE test (id INTEGER, name VARCHAR);", NULL),
      "failed: to CREATE TABLE {?}", "test");

  DUCKDB_STATE_OK(
      duckdb_query(
          con, "INSERT INTO test VALUES (1, 'Suv'), (2, null), (3, 'NotSuv');",
          NULL),
      "failed: to INSERT INTO {?} VALUES", "test");
}

std::vector<std::vector<std::string>> query_db(duckdb_connection &con) {
  duckdb_result result;

  DUCKDB_STATE_OK(duckdb_query(con, "SELECT * FROM test;", &result),
                  "failed: SELECT FROM {?}", "test");

  const idx_t ncols = duckdb_column_count(&result);
  const idx_t nrows = duckdb_row_count(&result);
  std::vector<std::vector<std::string>> str_values(
      1 + nrows, std::vector<std::string>(ncols));

  // column header
  for (idx_t col = 0; col < ncols; ++col) {
    fmt::print((col < ncols - 1) ? "{}, " : "{}\n",
               duckdb_column_name(&result, col));
    str_values[0][col] = duckdb_column_name(&result, col);
  }

  idx_t cur_row = 1;
  while (auto chunk = duckdb_fetch_chunk(result)) {
    const idx_t _ncols = duckdb_data_chunk_get_column_count(chunk);
    const idx_t _nrows = duckdb_data_chunk_get_size(chunk);

    for (idx_t col = 0; col < _ncols; ++col) {
      duckdb_vector vec = duckdb_data_chunk_get_vector(chunk, col);
      uint64_t *validity = duckdb_vector_get_validity(vec);
      const duckdb_logical_type logical_type =
          duckdb_vector_get_column_type(vec);
      switch (const duckdb_type type_id = duckdb_get_type_id(logical_type)) {
      case duckdb_type::DUCKDB_TYPE_BOOLEAN:
        format_vec_data((bool *)duckdb_vector_get_data(vec), validity,
                        str_values, col, cur_row, _nrows);
        break;
      case duckdb_type::DUCKDB_TYPE_INTEGER:
        format_vec_data((int32_t *)duckdb_vector_get_data(vec), validity,
                        str_values, col, cur_row, _nrows);
        break;
      case duckdb_type::DUCKDB_TYPE_BIGINT:
        format_vec_data((int64_t *)duckdb_vector_get_data(vec), validity,
                        str_values, col, cur_row, _nrows);
        break;
      case duckdb_type::DUCKDB_TYPE_UINTEGER:
        format_vec_data((uint32_t *)duckdb_vector_get_data(vec), validity,
                        str_values, col, cur_row, _nrows);
        break;
      case duckdb_type::DUCKDB_TYPE_UBIGINT:
        format_vec_data((uint64_t *)duckdb_vector_get_data(vec), validity,
                        str_values, col, cur_row, _nrows);
        break;
      case duckdb_type::DUCKDB_TYPE_FLOAT:
        format_vec_data((float *)duckdb_vector_get_data(vec), validity,
                        str_values, col, cur_row, _nrows);
        break;
      case duckdb_type::DUCKDB_TYPE_DOUBLE:
        format_vec_data((double *)duckdb_vector_get_data(vec), validity,
                        str_values, col, cur_row, _nrows);
        break;
      case duckdb_type::DUCKDB_TYPE_VARCHAR:
      case duckdb_type::DUCKDB_TYPE_BLOB:
        format_vec_data((duckdb_string_t *)duckdb_vector_get_data(vec),
                        validity, str_values, col, cur_row, _nrows);
        break;
      default:
        fmt::println("unsupported type: {?}", fmt::underlying(type_id));
      }
    }

    for (idx_t row = cur_row; row < cur_row + _nrows; ++row) {
      for (idx_t col = 0; col < ncols; col++) {
        fmt::print((col < ncols - 1) ? "{}, " : "{}\n", str_values[row][col]);
      }
    }
    duckdb_destroy_data_chunk(&chunk);
    cur_row += _nrows;
  }
  duckdb_destroy_result(&result);
  return str_values;
}

void arrow_query_db(duckdb_connection &con) {
  duckdb_arrow arrow_result;
  duckdb_arrow_array arrow_array;

  fmt::println("query:");
  DUCKDB_STATE_OK(duckdb_query_arrow(con, "SELECT * FROM test;", &arrow_result),
                  "failed: SELECT FROM {?}", "test");
  DUCKDB_STATE_OK(duckdb_query_arrow_array(arrow_result, &arrow_array),
                  "failed: to get *ArrowArray from {?}", "test");

  fmt::println("ncols: {}", duckdb_arrow_column_count(arrow_result));
  fmt::println("nrows: {}", duckdb_arrow_row_count(arrow_result));
  fmt::println("done!");

  // auto len = arrow_array->internal_ptr;
  auto *array = reinterpret_cast<ArrowArray *>(arrow_array);
  fmt::println("Array info: len={len}, nulls={nulls}, buffers={nbufs}, "
               "children={nchilds}",
               fmt::arg("len", array->length),
               fmt::arg("nulls", array->null_count),
               fmt::arg("nbufs", array->n_buffers),
               fmt::arg("nchilds", array->n_children));
  // fmt::println("Array info: len={len}", fmt::arg("len", array->length));
  duckdb_destroy_arrow(&arrow_result);
  fmt::println("destroyed!");
}
