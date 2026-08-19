# Phase 10: Concurrency Control

In this phase, you will build the concurrency engine of your database: the **Lock Manager**. You will define Shared and Exclusive lock modes, manage request wait queues, and implement **Strict 2-Phase Locking (2PL)** to serialize concurrent operations and prevent data races or dirty reads.

---

## 1. High-Level Vision: The Lock Manager

When multiple threads read and write to the same document page concurrently, data corruption occurs. We solve this by implementing a **Lock Manager**:

```text
  [ Transaction A (Read) ] ──► S-Lock (Page 1) ──► Compatibility Check ──► GRANTED
  [ Transaction B (Read) ] ──► S-Lock (Page 1) ──► Compatibility Check ──► GRANTED (Shared)
  
  [ Transaction C (Write) ] ──► X-Lock (Page 1) ──► Conflict Check (Blocked) ──► Append to Wait Queue
                                                                                    │
                                                                               (Commit/Abort)
                                                                                    │
                                                                               Wakes Up Thread
```

1. **Lock Modes (Shared / Exclusive)**: 
   * **Shared Locks (S)**: Multiple transactions can read a resource simultaneously (Readers do not block Readers).
   * **Exclusive Locks (X)**: Only one transaction can modify a resource (Writers block both Readers and Writers).
2. **Lock Request Queues**:Conflicting lock requests are placed in a **Wait Queue** and suspended using C++ condition variables. When the active lock is released, the Lock Manager wakes up the next threads in **FIFO (First-In, First-Out)** order to prevent starvation.
3. **Strict 2-Phase Locking (Strict 2PL)**: Transactions can only acquire locks during execution (Growing Phase) and **must hold all locks** until they commit or abort (Shrinking Phase). Releasing write locks early is prohibited because it would allow other threads to read uncommitted changes.

---

## 2. Recommended Roadmap

1. **[v0.41 — Lock Abstraction](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-10-concurrency-control/v0.41-lock-abstraction.md)**: Define request structures and compatibility matrices.
2. **[v0.42 — Lock Table Manager](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-10-concurrency-control/v0.42-lock-manager.md)**: Manage mapped queues using mutex locks and condition variables.
3. **[v0.43 — Shared & Exclusive Locks](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-10-concurrency-control/v0.43-shared-exclusive-locks.md)**: Implement the compatibility check algorithm.
4. **[v0.44 — Wait Queues](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-10-concurrency-control/v0.44-wait-queues.md)**: Implement FIFO request ordering and clean up aborted requests.
5. **[v0.45 — Strict 2PL Protocol](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-10-concurrency-control/v0.45-strict-2pl.md)**: Enforce the growing/shrinking rules to guarantee transaction isolation.
