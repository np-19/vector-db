# Phase 20: Transactional Vector Operations

In this phase, you will bridge the gap between transactional isolation (MVCC) and your vector search index: **Transactional Vector Operations**. If a user inserts a vector within a transaction that subsequently aborts, the vector must be rolled back from the database and pruned from the HNSW graph index. You will implement transactional vector storage, index version rollbacks, and write **MVCC-Aware Vector Search** to filter out invisible records during graph traversals.

---

## 1. High-Level Vision: Consistent Vector Views

When a database modifies records, it must guarantee ACID isolation. This becomes highly complex in vector indices because the graph structure connects nodes physically:

```text
  [ Transaction 101 Starts ] ──► Inserts Vector V1 ──► Added to HNSW Graph
                                                           │
                                        (Concurrently reads database)
                                                           ▼
  [ Transaction 102 (Reader) ] ─► Query: Search nearest to V1
                                  Is V1 visible to T102?
                                  No (T101 is still uncommitted!)
                                  * Action: Skip V1 during graph traversal.
                                  
  [ Transaction 101 Aborts ]  ──► Rollback:
                                  1. Delete V1 slot from page.
                                  2. Revert neighbor links in HNSW graph.
```

1. **Transactional Vector Storage**: Stamping vector document records with `xmin` and `xmax` inside page slots, ensuring they follow standard MVCC visibility rules.
2. **Transactional Index Rollback**: If a transaction aborts, we must locate any node it inserted into the HNSW graph, erase it, and reconnect its neighbors back to their original states using the transaction's undo log registry.
3. **MVCC-Aware Vector Search**: Checking row visibility (against the query snapshot) *during* graph traversal. If a node represents an uncommitted write or has been deleted by a committed transaction, the search engine skips it to avoid returning dirty data, but still allows routing through it to maintain path connectivity.

---

## 2. Recommended Roadmap

1. **[v0.80 — Transactional Vector Storage](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-20-transactional-vector-ops/v0.80-transactional-vector-storage.md)**: Stamp vector documents with transaction bounds.
2. **[v0.81 — Transactional Vector Index](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-20-transactional-vector-ops/v0.81-transactional-vector-index.md)**: Restore HNSW graph links during transaction aborts.
3. **[v0.82 — MVCC-Aware Vector Search](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-20-transactional-vector-ops/v0.82-mvcc-aware-vector-search.md)**: Evaluate record visibility during graph routing scans.
