# MiniVectorDB Testing Strategy

To build a reliable database engine, strict testing is mandatory at every level of the system. This document outlines the testing strategy, test suites, and validation guidelines for MiniVectorDB.

---

## 1. Storage Engine Verification

### Page Correctness & Slotted Layouts
- **Value Bounds**: Test boundaries when inserting documents that exactly fill, slightly underfill, or overflow the 4096-byte page size.
- **Slotted Alignment**: Verify that slotted pages maintain alignment (e.g. 4-byte boundaries for integers and offsets) on both x86 and ARM platforms.
- **Fragmentation & Shift**: Write tests that insert records $A, B, C$, delete $B$, and insert a larger record $D$. Ensure the page shifts remaining records correctly to reclaim fragmented free space.

### Persistence & Corruption Detection
- **Page Header Integrity**: Manually overwrite page bytes in the output file and ensure the page manager detects invalid headers or mismatching page types.
- **Checksum Validation**: Write unit tests to compute and inject random bit-flips into the page payload. Verify that `DiskManager::ReadPage` flags the error using CRC32 validation.

---

## 2. Buffer Pool Manager Verification

### State & Pinning Semantics
- **Eviction Lock**: Verify that pinned pages are *never* evicted. Fill the buffer pool with active pinned pages, attempt to fetch a new page, and assert that the manager returns an out-of-resources error/blocks until pages are unpinned.
- **Dirty Page Tracking**: Write records to Page 1, mark it dirty, and unpin it. Trigger an eviction. Check that Page 1 is written to disk *before* its frame is reclaimed.
- **Concurrent Access**: Launch 10 threads aggressively pinning, reading, writing, and unpinning the same subset of pages. Ensure no race conditions occur and that page references are tracked correctly.

---

## 3. B+ Tree Index Verification

### Tree Structural Invariants
- **Iterative Split & Merge**: Insert sequentially increasing keys ($1 \dots 1000$) and check that the tree splits correctly at each level. Then delete keys ($1000 \dots 1$) and verify that nodes merge or redistribute keys.
- **Range Scans**: Insert a random range of keys, establish an iterator, and verify that leaf-level sibling pointers yield correct, sorted lists.
- **Persistent B+ Tree Integration**: Ensure B+ Tree node allocations go through the `BufferPoolManager` using temporary page allocations instead of raw memory `new` operators.

---

## 4. Transaction & Isolation Verification

### Concurrency Anomalies Tests
Write explicit tests using multi-threaded execution to verify that isolation rules block or resolve anomalies:
1. **Dirty Read (G1a)**: Transaction 1 (T1) updates a document value. T2 reads the value. T1 aborts. Ensure T2 is either blocked or reads the original value (depending on isolation level).
2. **Non-Repeatable Read (G1b)**: T1 reads value $X$. T2 updates $X$ to $Y$ and commits. T1 reads $X$ again. Under `Repeatable Read` and `Serializable`, T1 *must* read $X$. Under `Read Committed`, T1 reads $Y$.
3. **Write Skew**: T1 reads $A$ and $B$, then updates $A$ based on $B$. Concurrent T2 reads $A$ and $B$, then updates $B$ based on $A$. Under `Serializable`, one transaction *must* fail to commit due to serialization conflicts.

### Lock Manager & Deadlocks
- **Lock Acquisition Matrix**: Verify that S-locks are shared, but X-locks block all other acquisitions.
- **Deadlock Cycle Detection**: Set up two threads:
  - Thread 1: Acquire X-lock on Page A, sleep, request X-lock on Page B.
  - Thread 2: Acquire X-lock on Page B, sleep, request X-lock on Page A.
- Verify that the transaction manager detects the cycle, aborts one thread, releases its locks, and allows the other to proceed.

---

## 5. WAL & Crash Recovery Verification

To guarantee durability, we simulate failures at critical phases:

```text
Crash Simulation Flow:
1. Begin Transaction -> Log Txn Start
2. Modify Page A -> Log Page Change -> Pin Page A (Dirty in Buffer Pool)
3. Commit Transaction -> Write Commit Log -> Flush Log Buffer to Disk
4. System Crash (Kill process or clear memory state)
5. Restart -> Parse WAL -> Execute REDO & UNDO phases
```

### Crash Recovery Tests
Write automated test cases using a mock disk system:
- **Crash Before Commit**: Write changes to a page, do *not* commit, simulate a crash. On restart, verify that the recovery manager runs the **UNDO** phase and rolls back changes in the database file.
- **Crash After Commit**: Commit a transaction, simulate a crash before the dirty pages are flushed. Verify that the recovery manager runs the **REDO** phase and applies the changes to the database file.
- **Partial Page Write**: Crash the system mid-page write. Ensure the recovery engine detects checksum corruption, restores the page from the last clean backup/checkpoint, and replays the WAL.

---

## 6. Vector Search Verification

### Distance Correctness
- Compare calculated L2, Cosine, and Dot Product distance results against simple C++ double-precision arrays to ensure SIMD instructions yield exact results.

### Index Validation (HNSW vs. Brute-Force)
- **Ground Truth Evaluation**: Create a dataset of 10,000 vectors. Run a brute-force scan to find the exact top-10 neighbors for a set of queries.
- **Recall Metric**: Run the same queries on the HNSW index and compute recall:
  $$\text{Recall} = \frac{|\text{HNSW Top-K} \cap \text{Brute-Force Top-K}|}{K}$$
- **Recall Target**: Assert that the HNSW index achieves at least **95% recall** under standard construction parameters ($M = 16, \text{efConstruction} = 64$).
