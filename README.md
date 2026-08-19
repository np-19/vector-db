# MiniVectorDB

MiniVectorDB is a C++20 educational database-engineering project. It builds a
document store and vector-search engine from first principles, including page
storage, transactions, MVCC, WAL, B+ trees, and HNSW indexing.

## Build

```powershell
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

The `minivectordb_cli` executable is an intentionally small application entry
point. Unit tests and benchmarks can be disabled with `-DBUILD_TESTING=OFF`
and `-DMINIVECTORDB_BUILD_BENCHMARKS=OFF`.

## Layout

- `include/minivectordb/`: public headers
- `src/`: library implementation
- `apps/`: executable entry points
- `tests/`: unit tests
- `benchmarks/`: performance benchmarks
- `cmake/`: shared CMake modules
- `docs/`: architecture and curriculum documentation

API documentation can be generated from `docs/Doxyfile` with Doxygen.

See [the documentation index](docs/README.md) for the full learning roadmap.
