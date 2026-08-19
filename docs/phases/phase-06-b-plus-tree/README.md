# Phase 6: B+ Tree Indexing

In this phase, you will implement the core indexing structure used by modern database systems: the **B+ Tree Index**. You will build a balanced search tree that sits inside 4KB database pages, allowing you to execute fast point lookups and range scans on your document metadata without reading every page from disk.

---

## 1. High-Level Vision: The Search Highway

Without an index, finding a document with `age = 25` requires scanning the entire database file from start to finish (a full table scan). As database files grow, this becomes incredibly slow.

A **B+ Tree Index** acts as a multi-lane highway system:

```text
                     [ Root Node Page ]
                     │  Key: [ 50 ]   │
                     └───┬────────┬───┘
                         │ < 50   │ >= 50
                         ▼        ▼
              [ Internal Node ]  [ Internal Node ]
              │  Key: [ 25 ]  │  │  Key: [ 75 ]  │
              └───┬───────┬───┘  └───┬───────┬───┘
                  │ < 25  │ >= 25    │ < 75  │ >= 75
                  ▼       ▼          ▼       ▼
               [Leaf]   [Leaf]    [Leaf]   [Leaf]  ◄── Linked in a chain (nextPageId)
               [RIDs]   [RIDs]    [RIDs]   [RIDs]      for fast range scans
```

1. **Wide Fan-Out**: Unlike binary trees (which have only 2 children per node), a database B+ Tree node has a "fan-out" of hundreds of keys per 4KB page. This means even a database containing millions of records can locate any record in only 3 to 4 disk page reads.
2. **Page-Backed Nodes**: B+ Tree nodes are stored directly inside physical database pages. Instead of memory pointers, node branches contain logical **Page IDs**.
3. **Leaf Sibling Links**: The bottom "Leaf" nodes are linked together in a sequential chain. This allows fast range scans (e.g. "Find all records with age between 20 and 30") by finding the start key and then walking the chain of sibling pages directly.

---

## 2. Recommended Roadmap

1. **[v0.20 — B+ Tree Fundamentals](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-06-b-plus-tree/v0.20-b-tree-fundamentals.md)**: Define the physical layout of internal nodes (keys and page branch IDs) and leaf nodes (keys and RIDs).
2. **[v0.21 — Search Operations](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-06-b-plus-tree/v0.21-search.md)**: Implement tree traversal from the root node down to the leaf nodes.
3. **[v0.22 — Insertions & Splits](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-06-b-plus-tree/v0.22-insert-and-split.md)**: Add new keys to leaf pages, splitting nodes when they reach capacity.
4. **[v0.23 — Deletions & Merges](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-06-b-plus-tree/v0.23-delete-and-merge.md)**: Remove keys, borrowing from or merging with sibling nodes when pages underflow.
5. **[v0.24 — Disk-Backed B+ Tree](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-06-b-plus-tree/v0.24-persistent-b-tree.md)**: Save node page buffers directly to database files.
6. **[v0.25 — Buffer-Backed B+ Tree](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-06-b-plus-tree/v0.25-buffer-pool-backed-b-tree.md)**: Cache B+ Tree node traversals through the Buffer Pool, using "crabbing" page pins.
