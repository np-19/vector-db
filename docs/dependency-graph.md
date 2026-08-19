# MiniVectorDB Compilation & Logic Dependency Graph

This document details the compile-time and run-time dependency structure of **MiniVectorDB**. It ensures that components are built in the correct order to guarantee architectural layering.

---

## 1. Subsystem Graph (Mermaid)

```mermaid
graph TD
    %% Core Storage Branch
    DiskManager["Disk Manager (Phase 2)"] --> PageManager["Page Manager (Phase 2)"]
    PageManager --> RecordManager["Record Manager (Slotted Pages, Phase 3)"]
    RecordManager --> BufferPoolManager["Buffer Pool Manager (Phase 4)"]
    BufferPoolManager --> CatalogManager["Catalog & Metadata (Phase 5)"]
    
    %% Indexing Subsystem
    BufferPoolManager --> BPlusTree["B+ Tree Index (Phase 6)"]
    BPlusTree --> IndexManager["Index Manager (Phase 6/18)"]
    
    %% Vector Branch
    RecordManager --> VectorType["Vector Type (Phase 16)"]
    VectorType --> DistanceFuncs["Distance Functions (SIMD, Phase 16)"]
    DistanceFuncs --> BruteForceKNN["Brute-Force KNN (Phase 17)"]
    BruteForceKNN --> HNSW["HNSW Graph Index (Phase 18)"]
    HNSW --> PersistentHNSW["Persistent HNSW (Buffer Pool Backed, Phase 18)"]
    PersistentHNSW --> IndexManager

    %% Transaction Layer
    CatalogManager --> TxnManager["Transaction Manager (Phase 9)"]
    TxnManager --> LockManager["Lock Manager (S/X locks, 2PL, Phase 10)"]
    LockManager --> DeadlockDetector["Deadlock Detector (Phase 11)"]
    DeadlockDetector --> MVCC["MVCC (Visibility & Version Chains, Phase 12)"]
    MVCC --> IsolationLevels["Isolation Levels (RC, RR, Serializable, Phase 13)"]
    IsolationLevels --> WAL["WAL Writer (Phase 14)"]
    WAL --> CheckpointsRecovery["Checkpoints & Crash Recovery (Phase 15)"]

    %% Query Engine
    CatalogManager --> QueryParser["Query Parser (Phase 7)"]
    IndexManager --> QueryPlanner["Query Planner (Phase 7/8)"]
    QueryParser --> QueryPlanner
    QueryPlanner --> QueryExecutor["Query Executor (Volcano Iterators, Phase 7)"]
    CheckpointsRecovery --> QueryExecutor
    
    %% Advanced Features
    QueryExecutor --> HybridSearch["Hybrid Search (Metadata + Vector, Phase 19)"]
    HybridSearch --> TxnVectorOps["Transactional Vector Search (Phase 20)"]
    TxnVectorOps --> CompactionGC["Compaction & GC (Phase 21)"]
    CompactionGC --> BackgroundWorkers["Background Workers (Phase 22)"]
    BackgroundWorkers --> Observability["Observability (Phase 23)"]
    Observability --> FinalIntegration["Final MiniVectorDB v1.0 (Phase 24)"]
```

---

## 2. Compilation Ordering Reference

When compiling MiniVectorDB, the build system (CMake) structures dependencies to avoid circular inclusions. The layers compile in the following order:

### Layer 1: Core Foundation & Data Formats (Leaf nodes)
- **Document / JSON Variant Type**: Independent representation of JSON documents (`Value`, `Document`).
- **Serializer**: Utility to convert raw structures into binary representation.
- **Common Types**: `page_id_t`, `txn_id_t`, `lsn_t`, `rid_t`.

### Layer 2: Low-Level Storage
- **Disk Manager**: Directly interacts with the file system.
- **Page Manager**: Manipulates raw 4096-byte frames.
- **Buffer Pool**: Manages cache frames and page pinning. Depends on Disk Manager and Page Manager.

### Layer 3: Physical Records & Access Methods
- **Slotted Page**: Physical layout of documents. Depends on Page Manager.
- **Record Manager**: Implements row deletion/insertion/tombstones on slotted pages.
- **B+ Tree Index**: Tree navigation. Depends on Record Manager and Buffer Pool.

### Layer 4: Transactions & Concurrency Control
- **Transaction Manager**: Tracks states of executions.
- **Lock Manager**: Strict 2PL queues. Depends on Transaction Manager.
- **WAL Writer**: Performs writes to WAL files. Must compile alongside Transaction Manager to sync commits.
- **MVCC visibility**: Decides page row visibility based on snapshots.

### Layer 5: Query Execution & Vector Calculations
- **Vector Engine**: Implements distance equations.
- **HNSW Index**: Builds multi-layered graphs. Depends on Vector Engine and Buffer Pool.
- **Query Planner / Executor**: Implements scans. Depends on B+ Tree, HNSW, Catalog, and Transactions.

### Layer 6: Integration & background tasks
- **Compaction & GC**: Cleans up dead rows.
- **Workers**: Handles periodic checkpoint flush, garbage collection, and compaction.
- **Observability**: Exposes internal statistics.
- **MiniVectorDB Client API**: The main application interface linking everything.
