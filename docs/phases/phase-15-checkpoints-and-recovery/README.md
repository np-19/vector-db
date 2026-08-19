# Phase 15: Checkpoints & Crash Recovery (ARIES)

In this final core phase, you will build the ultimate fail-safe subsystem of your database: the **ARIES Crash Recovery Engine**. You will implement periodic **Checkpoints** to truncate the log replay window, write the **REDO Phase** to repeat history after restarts, write the **UNDO Phase** to roll back uncommitted transactions, and integrate them into a fully automated boot recovery pipeline.

---

## 1. High-Level Vision: The Recovery Pipeline

When a database boots up after a crash (such as a sudden power outage), the database file on disk is in an inconsistent state. Some committed writes did not make it to disk, while some uncommitted modifications did.

The database restores consistency using the **ARIES Recovery Protocol**:

```text
    [ Crash Occurred ] 
           │
           ▼  (Boot up database)
   [ Locate Checkpoint ] ──► Find the last recorded checkpoint LSN.
           │
           ▼
     [ REDO Phase ]      ──► Scan WAL FORWARD. Replay all updates (committed or uncommitted)
    (Repeat History)         since the checkpoint to restore RAM state.
           │
           ▼
     [ UNDO Phase ]      ──► Scan WAL BACKWARD. Replay undo logs of all transactions
    (Clean Up Drafts)        that were active (uncommitted) during the crash to roll them back.
           │
           ▼  (Write Compensation Log Records - CLRs)
   [ Recovery Complete ] ──► Database is now consistent. Boot cleanly!
```

1. **Checkpoints**: Periodically, a background thread flushes all dirty pages from RAM to disk and writes a `CHECKPOINT` log listing all active transactions. This ensures that during boot, we don't have to scan the WAL from the beginning of time -- we only scan starting from the checkpoint.
2. **REDO Phase**: Scans the WAL forward from the checkpoint, replaying all writes to "repeat history" exactly as it happened.
3. **UNDO Phase**: Scans the WAL backward from the end, rolling back any transactions that were active (uncommitted) at the time of the crash, restoring original before-images.
4. **Compensation Log Records (CLRs)**: Written during the UNDO phase to log the rollback operations, ensuring that if the database crashes *during* recovery, we don't get stuck in infinite recovery loops.

---

## 2. Recommended Roadmap

1. **[v0.61 — Checkpoint Manager](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-15-checkpoints-and-recovery/v0.61-checkpoints.md)**: Periodically flush dirty pages and log active transactions.
2. **[v0.62 — The REDO Phase](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-15-checkpoints-and-recovery/v0.62-redo.md)**: Scan forward to replay committed operations.
3. **[v0.63 — The UNDO Phase](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-15-checkpoints-and-recovery/v0.63-undo.md)**: Scan backward to roll back uncommitted operations.
4. **[v0.64 — Automated Recovery](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-15-checkpoints-and-recovery/v0.64-crash-recovery.md)**: Integrate REDO/UNDO into a boot sequence.
