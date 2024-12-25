#pragma once
#ifndef _status_h
#define _status_h

#include <duckdb.h>
#include <fmt/core.h>

#define DUCKDB_STATE_OK(status, msg_fmt, ...)                                  \
  do {                                                                         \
    if (status == DuckDBError)                                                 \
      fmt::print(msg_fmt "\n" __VA_OPT__(, ) __VA_ARGS__);                     \
  } while (0)

#endif
