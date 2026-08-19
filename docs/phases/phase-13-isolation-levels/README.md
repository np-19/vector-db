# Phase 13: Transaction Isolation Levels

In this phase, you will build the isolation engine of your database: **Transaction Isolation Levels**. You will implement the standard SQL isolation levels (**Read Committed**, **Repeatable Read**, and **Serializable**) by configuring how snapshots are captured and how locks are upgraded, preventing concurrency anomalies like dirty reads, non-repeatable reads, and write skew.

---

## 1. High-Level Vision: Isolation Boundaries

ACID requires that transactions are isolated from each other. However, enforcing perfect isolation is expensive and degrades performance. Databases offer different **Isolation Levels** to let developers balance speed and correctness:

```text
  [ Read Committed ] ──► Captures a NEW snapshot at the start of EVERY query statement.
                         Prevents: Dirty Reads.
                         Allows high concurrency because snapshots change.
                         
  [ Repeatable Read ] ──► Captures a SINGLE snapshot once at transaction START.
                         Prevents: Dirty Reads, Non-Repeatable Reads.
                         Consistent point-in-time view throughout transaction.
                         
  [ Serializable ]    ──► Readers acquire S-locks, Writers acquire X-locks.
                         Prevents: All anomalies, including Write Skew.
                         Guarantees execution equivalent to serial (sequential) runs.
```

Anomalies prevented at each level:
1. **Dirty Reads**: Reading data modified by another transaction that hasn't committed yet. (Prevented by Read Committed and higher).
2. **Non-Repeatable Reads**: Reading the same row twice in a transaction and getting different values because a concurrent transaction committed changes in the middle. (Prevented by Repeatable Read and higher).
3. **Write Skew**: A logical conflict where two concurrent transactions read overlapping data, make decision updates, and commit, violating a global constraint because they didn't block each other. (Prevented only by Serializable).

---

## 2. Recommended Roadmap

1. **[v0.54 — Read Committed](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-13-isolation-levels/v0.54-read-committed.md)**: Capture fresh snapshots at statement starts.
2. **[v0.55 — Repeatable Read](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-13-isolation-levels/v0.55-repeatable-read.md)**: Pinned transaction-level snapshots.
3. **[v0.56 — Serializable Isolation](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-13-isolation-levels/v0.56-serializable.md)**: Enforce strict read locking to block concurrent updates.
