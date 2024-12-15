# Ikebana: a DuckDB based client that brings data together

## How to compile?

This project manages dependencies with `vcpkg`, so make sure you have
it setup correctly.

```bash
$ cmake -B build -S .
$ cmake --build build
```

The build also generates a compile command database in the build
directory (`build/compile_commands.json`).  For language server or
other IDE feature support, please point your tooling to this database.

If `vcpkg` is installed in a non-standard location (e.g. somewhere in
your home directory), you can always point to it when you invoke
`cmake` the first time by setting the environment variable
`VCPKG_ROOT`.

```bash
$ VCPKG_ROOT=/path/to/vcpkg cmake -B build -S .
```

Note that DuckDB is vendored under `third_party` since there are no
ports for `vcpkg`.
