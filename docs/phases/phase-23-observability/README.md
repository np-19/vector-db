# Phase 23: Database Observability

In this phase, you will build the monitoring dashboard of your database: **Database Observability**. A database should never be a black box; developers need visibility to track memory health, optimize slow queries, and inspect lock waits. You will build a central Metrics Registry using atomic counters, implement Diagnostic Views to introspect locks and active transactions, and write a slow query profiling engine.

---

## 1. High-Level Vision: Introspection & Profiling

To manage database performance, we build three observability channels:

```text
  [ Channel 1: MetricsRegistry ]  ──► Atomic lock-free counters in RAM.
                                      Tracks: Cache Hits/Misses, WAL Flushes, Commits.
                                      * Low overhead health counters.
                                 
  [ Channel 2: Diagnostic Views ] ──► introspects active tables:
                                      `DumpLockTable()`: Shows who is blocking whom.
                                      `DumpActiveTransactions()`: Shows running steps.
                                      * Used for deadlock troubleshooting.
                                 
  [ Channel 3: Query Profiler ]   ──► Slow Query Logger.
                                      Logs latency, page reads, and distance calculations.
                                      * Used for query query plan tuning.
```

1. **Metrics Registry (Atomic Instrumentation)**: Collects global database counters without holding locks (using atomic increments) to guarantee that monitoring does not degrade query execution speeds.
2. **System Diagnostic Views**: Prints human-readable tables showing the current lock queues and transaction states, helping developers diagnose active deadlocks or transaction leaks.
3. **Query execution statistics**: Profiles every single query plan run, recording the precise elapsed microsecond latency, number of disk page accesses, and count of vector calculations.

---

## 2. Recommended Roadmap

1. **[v0.91 — Metrics Registry](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-23-observability/v0.91-metrics.md)**: Collect database-wide events using lock-free atomic counters.
2. **[v0.92 — Diagnostic Introspection](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-23-observability/v0.92-diagnostics.md)**: Dump lock tables and active transaction queues.
3. **[v0.93 — Query Profiling](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-23-observability/v0.93-query-statistics.md)**: Profile slow queries and log resource statistics.
