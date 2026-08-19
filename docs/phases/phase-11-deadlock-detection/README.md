# Phase 11: Deadlock Detection & Resolution

In this phase, you will implement the deadlock handling system of your database: **Deadlock Detection**. When multiple concurrent transactions hold locks on resources while waiting for locks held by each other, a permanent freeze occurs (a **Deadlock**). You will build a **Wait-for Graph** to track transaction dependencies, implement **Cycle Detection** using Depth-First Search (DFS), and write a **Deadlock Resolver** to select and abort a victim transaction to break the freeze.

---

## 1. High-Level Vision: Breaking the Lock

Consider two transactions running concurrently:
* Transaction 1 holds a lock on Record A, and requests a lock on Record B.
* Transaction 2 holds a lock on Record B, and requests a lock on Record A.

Both threads go to sleep waiting for each other. Neither can make progress. They are deadlocked:

```text
       ┌──────────────────┐               ┌──────────────────┐
       │  Transaction 1   │ ──(Waits For)─►  Transaction 2   │
       │  (Holds Lock A)  │ ◄──(Waits For)── │  (Holds Lock B)  │
       └──────────────────┘               └──────────────────┘
```

To resolve this:
1. **Wait-for Graph**: We construct a directed graph where nodes are active transaction IDs, and edges represent dependencies ($T_1 \to T_2$ means $T_1$ is blocked waiting for a lock held by $T_2$).
2. **Cycle Detection**: We run a background cycle detector that scans the Wait-for Graph using a **Depth-First Search (DFS)** algorithm to identify loops (cycles).
3. **Deadlock Resolution**: Once a cycle is detected, we choose one transaction as the **Victim** (usually the youngest transaction with the highest TxID, to avoid wasting the progress of older transactions) and abort it. This releases its locks, breaking the cycle and allowing the other transaction to continue.

---

## 2. Recommended Roadmap

1. **[v0.46 — Wait-for Graph](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-11-deadlock-detection/v0.46-wait-for-graph.md)**: Implement the adjacency list model to track transaction dependencies.
2. **[v0.47 — DFS Cycle Detection](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-11-deadlock-detection/v0.47-cycle-detection.md)**: Traverse the graph to find circular loops.
3. **[v0.48 — Deadlock Resolution](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-11-deadlock-detection/v0.48-deadlock-resolution.md)**: Select a victim transaction and abort it to release locks.
