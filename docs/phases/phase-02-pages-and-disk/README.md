# Phase 2: Pages and Disk Storage

In this phase, you will transition the database from a simple sequential file writer into a structured, block-based storage engine. You will implement the concepts of physical database **Pages** and a **Disk Manager** that handles reading and writing pages.

---

## 1. High-Level Vision: Block-Based Storage

In Phase 1, we wrote serialized documents sequentially to a raw file. However, in a real database, files are managed in fixed-size blocks called **Pages** (commonly 4KB):

```text
 [ DiskFile: test_database.db ]
 ┌─────────────────┬─────────────────┬─────────────────┬─────────────────┐
 │  Page 0 (4KB)   │  Page 1 (4KB)   │  Page 2 (4KB)   │  Page 3 (4KB)   │
 │  Metadata Page  │  Data Page      │  B+ Tree Leaf   │  Free Page      │
 └─────────────────┴─────────────────┴─────────────────┴─────────────────┘
```

By organizing the file into fixed-size slots:
1. We can load and write parts of the file (pages) individually without reading or rewriting the entire file.
2. We can cache pages in memory (Buffer Pool) and swap them in and out as needed.
3. We align our reads and writes with physical SSD block sizes for maximum hardware performance.

---

## 2. Recommended Roadmap

1. **[v0.5 — Page Abstraction](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-02-pages-and-disk/v0.5-page-abstraction.md)**: Define the C++ structure representing a physical 4KB database page and its header metadata.
2. **[v0.6 — Disk Manager](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-02-pages-and-disk/v0.6-disk-manager.md)**: Build the manager that maps logical page IDs to physical byte offsets in our database file.
3. **[v0.7 — Page Allocation](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-02-pages-and-disk/v0.7-page-allocation.md)**: Track which pages are in use and which pages have been deleted and are free to reuse.
4. **[v0.8 — Page Validation/Checksums](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-02-pages-and-disk/v0.8-page-validation.md)**: Inject CRC32 checksums into page headers to detect file corruption.
