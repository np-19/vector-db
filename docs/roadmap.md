# MiniVectorDB Detailed Implementation Roadmap

This document serves as the comprehensive implementation roadmap for **MiniVectorDB**. It lists every phase and version in the curriculum.

---

## Roadmap Index

### Phase 1 — Database Foundation
*   **v0.1 — Project and Database Foundation**: Set up CMake, compiler flags (C++20), GoogleTest/Benchmark structures, and basic command line hooks.
*   **v0.2 — Document Model**: Implement a JSON-like document structure using a clean, light variant types class (`Document`, `Value`).
*   **v0.3 — Serialization**: Define flat byte-level representations for simple types, strings, lists, maps, and floats.
*   **v0.4 — Persistent Database File**: Implement basic open, close, sync, and sequential write operations using standard platform APIs.

### Phase 2 — Pages and Disk Storage
*   **v0.5 — Page Abstraction**: Define physical page parameters (4096-byte boundaries) and page headers.
*   **v0.6 — Disk Manager**: Write raw sectors to disk and manage directory file mappings.
*   **v0.7 — Page Allocation**: Implement bitmapped or linked lists of free pages to recycle vacated slots.
*   **v0.8 — Page Validation/Checksums**: Introduce CRC32/Adler32 validation on page read/write.

### Phase 3 — Record Storage
*   **v0.9 — Record IDs**: Design a lightweight `RID` layout containing a 32-bit page ID and 16-bit slot index.
*   **v0.10 — Slotted Pages**: Write slot directory maps that allow reading variable-sized documents within pages.
*   **v0.11 — Document Storage**: Enable packing full documents into physical slots.
*   **v0.12 — Update/Delete**: Implement soft-deletes (tombstones), offset shifting, and page fragmentation cleanups.

### Phase 4 — Buffer Pool
*   **v0.13 — Buffer Pool**: Implement the frame manager cache that retains physical page pages in RAM.
*   **v0.14 — Pin/Unpin**: Handle locking semantics so active tasks can prevent page eviction.
*   **v0.15 — Dirty Pages**: Track pages with memory writes, ensuring they are queued for disk flushing.
*   **v0.16 — Clock/LRU Eviction**: Implement a page eviction algorithm to free frames when cache limits are hit.

### Phase 5 — Catalog and Metadata
*   **v0.17 — Collection Manager**: Organize documents under logical tables (collections).
*   **v0.18 — Catalog**: Define metadata schemas, listing collections, types, and sizes.
*   **v0.19 — Persistent Metadata**: Write catalog definitions to disk metadata block tables.

### Phase 6 — B+ Tree Index
*   **v0.20 — B+ Tree Fundamentals**: Structure node boundaries, internal keys, and leaf values.
*   **v0.21 — Search**: Implement binary point key searches inside trees.
*   **v0.22 — Insert and Split**: Split overflowing leaf and internal nodes recursively.
*   **v0.23 — Delete and Merge**: Re-balance nodes, borrow keys, and merge underflowing sibling nodes.
*   **v0.24 — Persistent B+ Tree**: Serialize/deserialize nodes using dedicated disk page offsets.
*   **v0.25 — Buffer-Pool-Backed B+ Tree**: Adapt B+ Tree reads/writes to acquire and pin pages via the Buffer Pool.

### Phase 7 — Query Engine
*   **v0.26 — Query Representation**: Formulate filter expressions as Abstract Syntax Trees (AST).
*   **v0.27 — Query Parser**: Translate simple metadata queries (e.g. `topic == "database"`) into execution blocks.
*   **v0.28 — Query Executor**: Build iterator-based executors using Volcano-style processing.
*   **v0.29 — Collection Scan**: Implement sequential scan iterators across record files.
*   **v0.30 — Index Scan**: Implement range scan iterators over B+ Trees.
*   **v0.31 — Basic Query Planner**: Map filters to index or collection scan executors.

### Phase 8 — Query Optimization
*   **v0.32 — Logical Plans**: Create logical operator trees independent of physical implementation.
*   **v0.33 — Physical Plans**: Map logical operators to concrete physical execution structures.
*   **v0.34 — Index Selection**: Detect filter conditions matching index paths and choose B+ Tree scans.
*   **v0.35 — Basic Cost Model**: Estimate cost based on disk page count vs. index selectivities.
*   **v0.36 — EXPLAIN**: Print logical/physical query execution trees.

### Phase 9 — Transactions
*   **v0.37 — Transaction Abstraction**: Define transaction states (Active, Committed, Aborted) and context.
*   **v0.38 — Transaction Manager**: Maintain active transaction tracking lists.
*   **v0.39 — Commit**: Implement final changes committing protocols.
*   **v0.40 — Rollback**: Restore pre-transaction states on aborts.

### Phase 10 — Concurrency Control
*   **v0.41 — Lock Abstraction**: Implement read (Shared) and write (Exclusive) lock descriptors.
*   **v0.42 — Lock Manager**: Manage globally shared resource lockers.
*   **v0.43 — Shared/Exclusive Locks**: Implement lock validation compatibilities.
*   **v0.44 — Wait Queues**: Queue blocked transactions requesting locks.
*   **v0.45 — Strict 2PL**: Enforce transaction lock acquisitions and hold them until final commit/abort.

### Phase 11 — Deadlock Detection
*   **v0.46 — Wait-for Graph**: Graph transactional dependencies of locked transactions.
*   **v0.47 — Cycle Detection**: Track cycles in transaction wait graphs using Depth First Search (DFS).
*   **v0.48 — Deadlock Resolution**: Abort victim transactions to break detected cycles.

### Phase 12 — MVCC
*   **v0.49 — Versioned Records**: Embed transactional timestamps (`xmin`, `xmax`) in slotted record headers.
*   **v0.50 — Visibility Rules**: Determine if record versions are visible based on active transaction lists.
*   **v0.51 — Snapshots**: Create transaction snapshots listing active transaction IDs.
*   **v0.52 — Version Chains**: Link updated records back to their older versions in history.
*   **v0.53 — MVCC Garbage Collection**: Clean up outdated, unneeded record versions.

### Phase 13 — Isolation Levels
*   **v0.54 — Read Committed**: Implement visibility rules verifying committed data at statement start.
*   **v0.55 — Repeatable Read**: Enforce transaction-start snapshots to prevent non-repeatable reads.
*   **v0.56 — Serializable**: Detect serialization anomalies (e.g. write skew) or use strict page range locks.

### Phase 14 — WAL
*   **v0.57 — WAL Abstraction**: Formulate sequential logging structures.
*   **v0.58 — Log Records**: Format update, transaction boundary, and page change logs.
*   **v0.59 — WAL Writer**: Flush log buffers to disk prior to modifying corresponding dirty pages.
*   **v0.60 — Commit Durability**: Force WAL write and disk flush on transaction commits.

### Phase 15 — Checkpoints and Crash Recovery
*   **v0.61 — Checkpoints**: Periodically flush dirty buffer pages and write active transaction states to log files.
*   **v0.62 — REDO**: Replay transactions from the checkpoint forward.
*   **v0.63 — UNDO**: Reverse modifications made by uncommitted transactions.
*   **v0.64 — Crash Recovery**: Reconstruct database states by running REDO and UNDO passes.

### Phase 16 — Vector Engine
*   **v0.65 — Vector Type**: Create float array representations with strict dimensions.
*   **v0.66 — Distance Functions**: Build SIMD-accelerated distance metrics (L2, Cosine, Dot Product).
*   **v0.67 — Vector Persistence**: Read/write vectors to slotted page records.

### Phase 17 — Brute-Force Vector Search
*   **v0.68 — Brute-force KNN**: Implement sequential scans comparing all vectors.
*   **v0.69 — Top-K Optimization**: Track top results using max-heaps (`std::priority_queue`).
*   **v0.70 — Search Metrics**: Compute index latency and search statistics.

### Phase 18 — HNSW Vector Index
*   **v0.71 — HNSW Data Model**: Define layered graph nodes and link layouts.
*   **v0.72 — Graph Construction**: Build multi-layer graphs using probabilistic node elevations.
*   **v0.73 — Search**: Implement greedy routing across graph layers.
*   **v0.74 — Insert**: Insert new elements and establish neighbor connections.
*   **v0.75 — Delete**: Remove nodes from graphs and reconnect neighbors.
*   **v0.76 — Persistent HNSW**: Map HNSW graph nodes to database page frames.

### Phase 19 — Metadata + Vector Hybrid Search
*   **v0.77 — Metadata Filtering**: Extract document IDs matching metadata conditions.
*   **v0.78 — Hybrid Search**: Combine filters and vector search using pre-filtering, post-filtering, or single HNSW index scan with filter-pushdown.
*   **v0.79 — Query Optimization**: Dynamically switch hybrid search strategies based on cost estimation.

### Phase 20 — Transactional Vector Operations
*   **v0.80 — Transactional Vector Storage**: Ensure new vectors are isolated from concurrent transactions.
*   **v0.81 — Transactional Vector Index**: Maintain graph structures with transactional visibility.
*   **v0.82 — MVCC-aware Vector Search**: Return vectors only if their corresponding record versions are visible.

### Phase 21 — Compaction and Garbage Collection
*   **v0.83 — Record Compaction**: Reclaim space from tombstones and shift active records.
*   **v0.84 — MVCC Garbage Collection**: Reclaim space from dead record versions.
*   **v0.85 — Vector Index Compaction**: Reclaim space from marked-deleted HNSW nodes.
*   **v0.86 — Index Rebuilding**: Reconstruct indexes from scratch to optimize node layouts.

### Phase 22 — Background Workers
*   **v0.87 — Worker Framework**: Manage background threads and tasks.
*   **v0.88 — Checkpoint Worker**: Run periodic checkpointing in the background.
*   **v0.89 — Garbage Collector**: Run MVCC and record compaction in the background.
*   **v0.90 — Vector Maintenance Worker**: Update HNSW node layouts in the background.

### Phase 23 — Observability
*   **v0.91 — Metrics**: Track buffer hits, read/write I/O, cache evictions, lock waits, and transactions.
*   **v0.92 — Diagnostics**: Print locks, active transactions, and index details.
*   **v0.93 — Query Statistics**: Measure executor latency, page reads, and recall rate.

### Phase 24 — Final Integration
*   **v1.0 — MiniVectorDB**: Combine all layers into a unified, ACID-compliant database.
