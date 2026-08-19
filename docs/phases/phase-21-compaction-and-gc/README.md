# Phase 21: Compaction & Garbage Collection

In this final database engine optimization phase, you will implement the cleanup processes of your database: **Compaction & Garbage Collection**. Over time, database updates and deletions leave physical storage and graph indexes fragmented, degrading read performance. You will build slotted page record compaction to shift active records and reclaim dead space, write MVCC version pruning helpers, prune soft-deleted nodes from HNSW graphs, and implement automated **Index Rebuilding (REINDEX)**.

---

## 1. High-Level Vision: The Cleanup Pipeline

As a database runs, it naturally accumulates junk. We need physical sweeps to maintain performance:

```text
  [ Fragmentation Bloat ] ────► Slotted page contains empty gaps from deleted rows.
                                HNSW graph contains vacant "tombstoned" nodes.
                                 │
                                 ▼  (Run cleanup pipelines)
  [ Step 1: Page Compaction ] ──► Squeeze slotted page data array to shift active records
                                  and consolidate free space at the page center.
                                 │
  [ Step 2: MVCC version GC ] ──► Sweep version chains. Erase historical copies older
                                  than the oldest active transaction.
                                 │
  [ Step 3: Graph Pruning ]   ──► Erase soft-deleted node links in HNSW graphs
                                  and reconnect neighbors directly.
                                 │
  [ Step 4: Index Rebuild ]   ──► Run REINDEX: compile a fresh index in temporary pages
                                  and swap the catalog pointers, recycling old pages.
```

1. **Slotted Page Compaction**: Reclaims fragment gaps on data pages by shifting active records toward the end of the page, restoring a contiguous free-space window.
2. **MVCC Version GC**: Sweeps version chains and removes old records with committed `xmax` timestamps older than the oldest running query.
3. **HNSW Graph Compaction (Pruning)**: Physically deletes tombstoned nodes from the HNSW graph and updates neighbor connections to bypass them completely.
4. **Index Rebuilding (REINDEX)**: Compiles a fresh graph or B+ Tree index on a clean set of pages by scanning the collection, and performs an atomic catalog root swap to recycle old fragmented index pages.

---

## 2. Recommended Roadmap

1. **[v0.83 — Page Record Compaction](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-21-compaction-and-gc/v0.83-record-compaction.md)**: Consolidate page free space.
2. **[v0.84 — MVCC Version GC Sweep](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-21-compaction-and-gc/v0.84-mvcc-garbage-collection.md)**: Prune unneeded historical records from page slots.
3. **[v0.85 — Vector Index Compaction](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-21-compaction-and-gc/v0.85-vector-index-compaction.md)**: Reconnect neighbors and purge tombstones from HNSW graphs.
4. **[v0.86 — Automated Index Rebuilding](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-21-compaction-and-gc/v0.86-index-rebuilding.md)**: Perform index root page pointer swaps to recycle space.
