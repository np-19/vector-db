# Phase 22: Background Workers Framework

In this phase, you will build the asynchronous daemon framework of your database: the **Background Workers Framework**. Running heavy operations (like WAL checkpoint flushing, slotted page compaction, and HNSW graph rebalancing) synchronously inside user query threads causes sudden, massive latency spikes for clients. You will build a base thread scheduling framework, write a background Checkpoint flusher, build an autovacuum Garbage Collector, and implement a background Vector Index Maintenance worker.

---

## 1. High-Level Vision: Parallel Maintenance

By offloading heavy cleanup and saving tasks to background worker threads, client transactions can insert and query records with near-zero latency overhead:

```text
  [ Client Thread ]      ──► SQL Query / Insert ──────────► Near-Zero Latency
                                                               │
                                                 (Asynchronous Notification)
                                                               │
                                                               ▼
  [ Background Workers ] ──► Checkpoint Worker   ──► Syncs dirty buffer pages to disk.
                             Autovacuum GC       ──► Sweeps dead row slots.
                             Vector Index Worker ──► Re-links HNSW node paths.
```

To coordinate these threads safely:
1. **Thread Loops stop flags**: We use C++ threads running infinite `while(!stopRequested)` loops.
2. **Synchronized Sleep**: Threads sleep on C++ condition variables using timeouts (e.g. waking up every 5 seconds) to avoid wasting CPU cycles.
3. **Clean Shutdown**: When the database shuts down, we set the stop flags and call thread `join()` statements to wait for active worker tasks to finish cleanly, preventing data corruption and memory leaks.

---

## 2. Recommended Roadmap

1. **[v0.87 — Worker Framework](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-22-background-workers/v0.87-worker-framework.md)**: Build stop-flag thread loops coordinating clean shutdowns.
2. **[v0.88 — Checkpoint Worker](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-22-background-workers/v0.88-checkpoint-worker.md)**: Flush dirty pages to disk automatically.
3. **[v0.89 — Autovacuum Garbage Collector](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-22-background-workers/v0.89-garbage-collector.md)**: Sweep dead versions and compact pages in the background.
4. **[v0.90 — Vector Maintenance Worker](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-22-background-workers/v0.90-vector-maintenance-worker.md)**: Prune and rebalance HNSW graphs asynchronously.
