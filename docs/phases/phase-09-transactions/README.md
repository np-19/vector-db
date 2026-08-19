# Phase 9: Transactions

In this phase, you will build the core transaction system of your database: the **Transaction Manager**. You will define transaction lifecycles, manage unique transaction IDs, and implement the atomic **Commit** and **Rollback** protocols to ensure that groups of database operations either execute completely or leave the database untouched.

---

## 1. High-Level Vision: The ACID Guarantee

Without transactions, if a database crashes halfway through updating three documents, some files will have the new data while others will keep the old data, leaving your database in a corrupted state. 

Transactions solve this by wrapping multiple operations in a single context:

```text
       ┌────────────────────────┐
       │   Transaction Begin    │  ──► Assigns unique TxnID (e.g., Txn 101)
       └───────────┬────────────┘
                   │
           Executes Updates
                   │
         ┌─────────┴─────────┐
         ▼                   ▼
    [ Commit Path ]    [ Rollback Path ]
   (Success, Commit)   (Failure / Abort)
         │                   │
         ├───────────────────┼───────────────────┐
         ▼ (Commit helper)   ▼ (Rollback helper)  ▼ (Release locks)
    Write COMMIT Log    Read UNDO Logs       Release locks
    Flush logs to disk  Revert updates       Notify waiters
    Release locks       Release locks
```

1. **Transaction State Model**: We track whether a transaction is `ACTIVE`, `COMMITTED`, or `ABORTED`.
2. **Atomic Commits**: On commit, the database flushes all WAL log buffers to disk, flags the transaction state as `COMMITTED`, and releases all locks held by the transaction.
3. **Atomic Rollbacks (Aborts)**: If a query fails or a transaction aborts, we use **Undo Logs** (the "before-image" of page bytes) to revert all modifications in reverse order, returning the database to its exact pre-transaction state before releasing locks.

---

## 2. Recommended Roadmap

1. **[v0.37 — Transaction Abstraction](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-09-transactions/v0.37-transaction-abstraction.md)**: Define the core `Transaction` descriptor, states, and locked resources registers.
2. **[v0.38 — Transaction Manager](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-09-transactions/v0.38-transaction-manager.md)**: Build the global registry allocating unique, thread-safe transaction IDs.
3. **[v0.39 — Commit Protocol](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-09-transactions/v0.39-commit.md)**: Flush logs to disk and finalize transaction state changes.
4. **[v0.40 — Rollback Protocol](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-09-transactions/v0.40-rollback.md)**: Read undo images to revert modifications in reverse order.
