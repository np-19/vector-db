# Phase 12: Multi-Version Concurrency Control (MVCC)

In this phase, you will build the core versioning engine of your database: **Multi-Version Concurrency Control (MVCC)**. Instead of locking records and blocking readers while a writer is modifying a row, MVCC allows readers to look at a point-in-time snapshot of the database. You will implement visibility headers, transaction snapshots, historical version chains, and a background garbage collector to prune dead versions.

---

## 1. High-Level Vision: Readers Do Not Block Writers

In simple locking systems (Phase 10), if a transaction is writing to a page, all reader threads are blocked. This causes massive bottlenecks in read-heavy applications.

**MVCC** resolves this by never overwriting data in-place during updates. Instead, we create a new **Version** of the record:

```text
       ┌────────────────────────────────────────────────────────┐
       │                 Slotted Page Record Slot               │
       ├────────────────────────────────────────────────────────┤
       │ Active version (New)  ──► [xmin=102, xmax=0, val="C"]   │
       │                                │                       │
       │                         (prevVersionRID)               │
       │                                ▼                       │
       │ Historical version    ──► [xmin=101, xmax=102, val="B"]│
       │                                │                       │
       │                         (prevVersionRID)               │
       │                                ▼                       │
       │ Oldest version        ──► [xmin=100, xmax=101, val="A"]│
       └────────────────────────────────────────────────────────┘
```

1. **Version Stamping (`xmin` / `xmax`)**: Every record is stamped with:
   * **`xmin`**: The ID of the transaction that inserted this version.
   * **`xmax`**: The ID of the transaction that deleted/overwrote this version.
2. **Transaction Snapshots**: When a transaction starts, it captures a snapshot of active transaction IDs. The visibility engine uses this snapshot to decide whether a record version is "visible" to the reader.
3. **Version Chains**: When updating a record, we insert the new version and write its `prevVersionRID` pointer to point to the old slot location, forming a historical chain.
4. **Garbage Collection (Purging)**: As updates happen, older versions become useless because they are older than the oldest active transaction snapshot. A background **Garbage Collector** sweeps the pages, deleting these dead versions to reclaim space.

---

## 2. Recommended Roadmap

1. **[v0.49 — Versioned Records](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-12-mvcc/v0.49-versioned-records.md)**: Add MVCC visibility headers to records.
2. **[v0.50 — Visibility Rules](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-12-mvcc/v0.50-visibility-rules.md)**: Evaluate whether record versions are visible to a reader.
3. **[v0.51 — Transaction Snapshots](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-12-mvcc/v0.51-snapshots.md)**: Capture database states at transaction startup.
4. **[v0.52 — Version Chains](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-12-mvcc/v0.52-version-chains.md)**: Link updates to historical records.
5. **[v0.53 — MVCC Garbage Collection](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-12-mvcc/v0.53-mvcc-garbage-collection.md)**: Prune dead, invisible record versions to reclaim space.
