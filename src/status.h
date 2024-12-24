#pragma once
#ifndef _status_h
#define _status_h

#include <fmt/core.h>
#include <duckdb.h>

// FIXME: add newline to message
#define DUCKDB_STATE_OK(status, msg_fmt, ...) \
  if (status == DuckDBError) fmt::print(msg_fmt __VA_OPT__(,) __VA_ARGS__)

#endif
