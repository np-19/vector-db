# MiniVectorDB — Systems & Database Engineering Curriculum

Welcome to **MiniVectorDB**, a serious database-engineering and systems programming project built in C++20 from scratch.

Unlike standard databases or high-level vector search libraries, the core objective of MiniVectorDB is **educational clarity**. By implementing each database subsystem ourselves—from disk and page management up to transactions, Multi-Version Concurrency Control (MVCC), write-ahead logging (WAL), B+ Trees, and Hierarchical Navigable Small World (HNSW) graphs—you will learn exactly *why* each subsystem exists, *how* it handles memory and hardware limits, and *how* it integrates into a cohesive storage engine.

---

## 1. Core Philosophy & Educational Focus
This database is engineered to teach systems programming principles rather than provide a simple CRUD library wrapper. 

Every component is built under the following rules:
- **No Third-Party Storage Engines**: No SQLite, RocksDB, FAISS, or HNSWlib. Every byte written to disk is formatted and managed by our own subsystems.
- **Deep Explanations**: For every version implemented, we must answer:
  1. What problem does it solve?
  2. Why does a database need it?
  3. What happens without it?
  4. How do real databases (PostgreSQL, InnoDB, MongoDB) solve the problem?
  5. How will MiniVectorDB solve it, and what simplifications/trade-offs are made?
- **Modern C++20 Practices**: Utilizing RAII, smart pointers, `std::span`, atomic operations, `std::shared_mutex`, explicit error handling, and strict const correctness.

---

## 2. System Architecture

MiniVectorDB combines a document-oriented JSON API with a high-performance vector search engine. The internal architecture consists of four primary layers:

```text
                         MiniVectorDB
                              │
                    ┌─────────┴─────────┐
                    │                   │
                Query/API          Vector Engine
                    │                   │
               Query Parser          HNSW
               Query Planner         KNN
               Query Executor       Distance
                    │                   │
                    └─────────┬─────────┘
                              │
                       Transaction Layer
                              │
                  ┌───────────┴───────────┐
                  │                       │
                 MVCC                  Lock Manager
                  │                       │
                  └───────────┬───────────┘
                              │
                        Storage Engine
                              │
             ┌────────────────┼────────────────┐
             │                │                │
        Record Manager    Buffer Pool      Index Manager
             │                │           ┌────┴────┐
             │                │         B+ Tree   HNSW
             └────────────────┼────────────┬───────┘
                              │            │
                         Page Manager      │
                              │            │
                         Disk Manager     │
                              │            │
                              └─────┬──────┘
                                    │
                                   WAL
                                    │
                                    ▼
                                   Disk
```

1. **Query/API Layer**: Parses queries, evaluates logical/physical plans, selects appropriate indexes, and executes scans.
2. **Vector Engine**: Implements distance metrics, brute-force k-Nearest Neighbor (kNN) search, and an HNSW graph index.
3. **Transaction Layer**: Provides ACID compliance using strict 2-Phase Locking (2PL), Deadlock Detection, Multi-Version Concurrency Control (MVCC), and crash recovery via a Write-Ahead Log (WAL).
4. **Storage Engine**: Manages disk files, fixed-size physical pages, slotted records for variable-size documents, page caching in the Buffer Pool, and structural indexing using B+ Trees.

---

## 3. Curriculum Roadmap & Directory Structure

The repository keeps public interfaces under `include/minivectordb/`, private
implementations under `src/`, executable entry points under `apps/`, tests
under `tests/`, benchmarks under `benchmarks/`, and shared CMake modules under
`cmake/`. Generated files belong in `build/` and are not committed.

The curriculum is structured into **24 development phases** across **93 incremental versions**:

### Root Documentation
- [Architecture Blueprint](file:///d:/C++/Projects/mini-vector-db/docs/architecture.md) — Detailed breakdown of every layer.
- [Roadmap](file:///d:/C++/Projects/mini-vector-db/docs/roadmap.md) — The phase-by-phase version list.
- [Dependency Graph](file:///d:/C++/Projects/mini-vector-db/docs/dependency-graph.md) — Subsystem order of compilation and integration.
- [Testing Strategy](file:///d:/C++/Projects/mini-vector-db/docs/testing-strategy.md) — Complete plans for correctness, stress, concurrency, and crash testing.
- [Testing Guide](testing-guide.md) — Beginner walkthrough for writing and running GoogleTest tests.
- [Performance Strategy](file:///d:/C++/Projects/mini-vector-db/docs/performance-strategy.md) — Benchmarking setup, latency targets, and profiling procedures.
- [Benchmarking Guide](benchmarking-guide.md) — Beginner walkthrough for creating and running performance benchmarks.
- [CMake Guide](cmake-guide.md) — Beginner explanation of configuring, building, targets, dependencies, and every project CMake file.

### Development Phases
1. [Phase 1: Database Foundation](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-01-foundation/README.md) (`v0.1` - `v0.4`)
2. [Phase 2: Pages and Disk Storage](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-02-pages-and-disk/README.md) (`v0.5` - `v0.8`)
3. [Phase 3: Record Storage](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-03-record-storage/README.md) (`v0.9` - `v0.12`)
4. [Phase 4: Buffer Pool](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-04-buffer-pool/README.md) (`v0.13` - `v0.16`)
5. [Phase 5: Catalog and Metadata](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-05-catalog-and-metadata/README.md) (`v0.17` - `v0.19`)
6. [Phase 6: B+ Tree Index](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-06-b-plus-tree/README.md) (`v0.20` - `v0.25`)
7. [Phase 7: Query Engine](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-07-query-engine/README.md) (`v0.26` - `v0.31`)
8. [Phase 8: Query Optimization](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-08-query-optimization/README.md) (`v0.32` - `v0.36`)
9. [Phase 9: Transactions](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-09-transactions/README.md) (`v0.37` - `v0.40`)
10. [Phase 10: Concurrency Control](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-10-concurrency-control/README.md) (`v0.41` - `v0.45`)
11. [Phase 11: Deadlock Detection](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-11-deadlock-detection/README.md) (`v0.46` - `v0.48`)
12. [Phase 12: MVCC](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-12-mvcc/README.md) (`v0.49` - `v0.52`)
13. [Phase 13: Isolation Levels](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-13-isolation-levels/README.md) (`v0.54` - `v0.56`)
14. [Phase 14: WAL](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-14-wal/README.md) (`v0.57` - `v0.60`)
15. [Phase 15: Checkpoints and Crash Recovery](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-15-checkpoints-and-recovery/README.md) (`v0.61` - `v0.64`)
16. [Phase 16: Vector Engine](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-16-vector-engine/README.md) (`v0.65` - `v0.67`)
17. [Phase 17: Brute-Force Vector Search](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-17-brute-force-vector-search/README.md) (`v0.68` - `v0.70`)
18. [Phase 18: HNSW Vector Index](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-18-hnsw-vector-index/README.md) (`v0.71` - `v0.76`)
19. [Phase 19: Metadata + Vector Hybrid Search](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-19-hybrid-search/README.md) (`v0.77` - `v0.79`)
20. [Phase 20: Transactional Vector Operations](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-20-transactional-vector-ops/README.md) (`v0.80` - `v0.82`)
21. [Phase 21: Compaction and Garbage Collection](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-21-compaction-and-gc/README.md) (`v0.83` - `v0.86`)
22. [Phase 22: Background Workers](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-22-background-workers/README.md) (`v0.87` - `v0.90`)
23. [Phase 23: Observability](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-23-observability/README.md) (`v0.91` - `v0.93`)
24. [Phase 24: Final Integration](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-24-final-integration/README.md) (`v1.0`)

---

## 4. Getting Started

To begin implementing MiniVectorDB:
1. Review the [Architecture Blueprint](file:///d:/C++/Projects/mini-vector-db/docs/architecture.md) and [Dependency Graph](file:///d:/C++/Projects/mini-vector-db/docs/dependency-graph.md) to understand how components link together.
2. Read the [Testing Strategy](file:///d:/C++/Projects/mini-vector-db/docs/testing-strategy.md) to understand how to write and run verification suites.
3. Open [Phase 1 README](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-01-foundation/README.md) and implement `v0.1` project foundation following the detailed specification.
