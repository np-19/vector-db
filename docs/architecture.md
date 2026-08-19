# MiniVectorDB Architecture Blueprint

This document details the system architecture of **MiniVectorDB**, mapping the structure of physical files, memory structures, query pipelines, and transaction mechanics. MiniVectorDB is divided into four main layers:

```text
+-----------------------------------------------------------------------+
|                           Query & API Layer                           |
|  - Parses high-level commands (insert, find, vectorSearch)            |
|  - Generates logical & physical query plans                           |
|  - Selects optimal scan methods (Index Scan vs. Collection Scan)       |
+-----------------------------------------------------------------------+
                                    │
                                    ▼
+-----------------------------------------------------------------------+
|                           Transaction Layer                           |
|  - Manages Transaction IDs (TxnID) & transaction states                |
|  - Controls access via Lock Manager (Strict 2-Phase Locking)          |
|  - Achieves multi-version reader isolation via MVCC                   |
|  - Restores state after crashes using the Write-Ahead Log (WAL)       |
+-----------------------------------------------------------------------+
                                    │
                                    ▼
+-----------------------------------------------------------------------+
|                             Storage Engine                            |
|  - Manages file layouts and virtual-to-physical translations         |
|  - Disk Manager: Writes pages to disk and tracks free pages          |
|  - Buffer Pool: Pins/unpins pages in memory, manages clock eviction   |
|  - Record Manager: Layouts variable records in slotted physical pages  |
|  - Index Manager: Maintains B+ Trees for document/metadata lookup    |
+-----------------------------------------------------------------------+
                                    │
                                    ▼
+-----------------------------------------------------------------------+
|                             Vector Engine                             |
|  - Computes SIMD distance functions (L2, Cosine, Dot Product)        |
|  - Computes exact KNN search using linear brute-force scan            |
|  - Builds HNSW graphs backed by the buffer pool for fast search      |
+-----------------------------------------------------------------------+
```

---

## 1. Storage Subsystem & Disk Format

A MiniVectorDB database is persisted as a single database file (e.g. `minivectordb.db`) accompanied by a write-ahead log file (`minivectordb.wal`). 

### Physical Disk Pages
The database file is split into fixed-size physical pages of **4096 bytes** (matching typical OS page boundary sizes to optimize disk I/O aligned access). Pages are identified by a 32-bit `page_id_t` (0, 1, 2...).

There are three primary page formats:
1. **Metadata Page (Page 0)**: Contains database headers, schema, root page IDs for indexes, and free page list pointers.
2. **Slotted Page**: Stores document records. Contains a header with a slot count, free space pointer, and directory array growing downwards; record data is written from the bottom of the page growing upwards.
3. **B+ Tree Node Page**: Stores key-pointer or key-value pairs depending on whether it is an internal or leaf node.

```text
Slotted Page Layout (4KB):
+------------------------------------------------------------+
| PageHeader (LSN, PageType, FreeSpacePointer, SlotCount)    |
+------------------------------------------------------------+
| Slot 0 (Offset: 4000, Length: 96)                          |
| Slot 1 (Offset: 3850, Length: 150)                         |
| Slot 2 (Offset: 3700, Length: 150)                         |
| ...                                                        |
|                                                            |
|                    <--- FREE SPACE --->                    |
|                                                            |
| Record 2 (Offset: 3700, size: 150 bytes)                   |
| Record 1 (Offset: 3850, size: 150 bytes)                   |
| Record 0 (Offset: 4000, size: 96 bytes)                    |
+------------------------------------------------------------+
```

### Buffer Pool Manager
The **Buffer Pool Manager** acts as the virtual memory manager for the database. Instead of loading the entire file into RAM, the database loads pages into a fixed-size cache of `BufferFrames`.
- **Pinning**: An active thread "pins" a page to ensure it remains in memory while being read/written.
- **Clock Eviction**: When a new page must be fetched and the cache is full, the Clock Sweep algorithm identifies unpinned frames for eviction.
- **Dirty Pages**: Pages modified in RAM are marked "dirty". They are written to disk only when evicted or forced by a checkpoint, and always *after* the corresponding WAL record has been flushed to disk.

---

## 2. Indexing Subsystem

To avoid full scans, MiniVectorDB utilizes two index structures:

### B+ Tree Index
A standard database B+ Tree tailored for disk storage:
- Handles key-value operations for metadata columns (like integers or strings) and the primary document ID.
- Each node corresponds to a unique `page_id_t` in the Buffer Pool, rather than containing raw memory pointers.
- Provides $O(\log N)$ point lookups and efficient sequential range scans using sibling pointers at the leaf level.

### HNSW Vector Graph
For vector search, the HNSW index creates a multi-layered skip-list of graph nodes:
- Each node contains a vector representation and lists of neighboring node pointers.
- During construction, nodes are inserted at a random height. Search starts at the top layer (coarse navigation) and descends to layer 0 (fine-grained navigation).
- The HNSW index is mapped to physical pages, enabling vector graphs to exceed RAM sizes by backing node relationships with the Buffer Pool.

---

## 3. Transaction & Recovery Subsystem

MiniVectorDB guarantees ACID properties through three components working in tandem:

```text
                  +--------------------------+
                  |    Transaction Manager   |
                  +-------------┬------------+
                                │
         ┌──────────────────────┼──────────────────────┐
         ▼                      ▼                      ▼
+------------------+   +------------------+   +------------------+
|   Lock Manager   |   |   MVCC Version   |   |  WAL/LSN Engine  |
|  - Strict 2PL    |   |  - Reader Snaps  |   |  - Append-only   |
|  - S/X Locks     |   |  - xmin / xmax   |   |  - REDO / UNDO   |
+------------------+   +------------------+   +------------------+
```

1. **Strict 2-Phase Locking (2PL)**: Transactions acquire shared locks (S-locks) to read and exclusive locks (X-locks) to write. Locks are held until transaction commit/abort (strict phase), preventing dirty writes, dirty reads, and cascading rollbacks.
2. **Multi-Version Concurrency Control (MVCC)**: Allows non-blocking reads. Each record modification creates a new record version stamped with the creating Transaction ID (`xmin`) and the deleting Transaction ID (`xmax`). A transaction reads the snapshot corresponding to its start time, completely ignoring uncommitted versions without blocking writes.
3. **Write-Ahead Logging (WAL)**: All modifications (inserts, updates, index splits) write a record to the WAL before being applied to the data page. Each log record contains an incremental Log Sequence Number (LSN). During startup recovery, the WAL is replayed to reconstruct the buffer pool (REDO) and roll back uncommitted transactions (UNDO).

---

## 4. Query Pipeline

When a user runs a search like `db.vectorSearch(queryVector, k, {"difficulty": "advanced"})`:
1. **Parser**: Translates the arguments into an abstract query representation.
2. **Planner**: Evaluates possible physical operations. It checks if an index exists for the field `difficulty`.
3. **Index Selection**: If an index exists, it creates an `IndexScan` plan to filter candidate Record IDs. If not, it falls back to a `CollectionScan`.
4. **Executor**: If using hybrid indexing, the query planner can execute:
   - *Pre-filtering*: Scan B+ Tree for `difficulty == "advanced"`, collect valid document record IDs, and compute distance only on those.
   - *Post-filtering*: Search the HNSW index for top-K vectors, and drop results that do not match the filter.
   - *Single HNSW Index Scan with filter-pushdown*: Filter graph neighbors during HNSW traversal.
