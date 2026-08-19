# Phase 4: Buffer Pool Manager

In this phase, you will implement the core memory management subsystem of a database engine: the **Buffer Pool Manager**. You will build an in-memory cache layer that holds physical database pages in RAM, reducing slow disk reads and writes.

---

## 1. High-Level Vision: Memory vs. Disk

Reading and writing files on disk is thousands of times slower than accessing variables in RAM. A database engine cannot load a page from disk every time it wants to read a record, nor can it write directly to disk on every single record insert:

```text
  [ Higher Layers (B+ Tree, Query Engine) ]
                     │
         Page Requests (PageID)
                     ▼
       ┌───────────────────────────┐
       │   Buffer Pool Manager     │  ◄── Manages pages in memory (RAM)
       └─────────────┬─────────────┘
                     │
               Cache Miss / Flush
                     ▼
       ┌───────────────────────────┐
       │      Disk Manager         │  ◄── Reads/writes physical blocks
       └───────────────────────────┘
```

The **Buffer Pool Manager** allocates a fixed chunk of RAM (the cache pool) consisting of pre-allocated page "frames".
1. **Fetch Page**: If a layer requests a Page ID, we check if it is already in memory. If yes (cache hit), we return it instantly. If not (cache miss), we load it from disk into a frame and cache it.
2. **Eviction**: If our cache is full of pages and we need to load a new page, we must select an inactive page to "evict" (remove from memory) to make room. If the evicted page was modified (dirty), we must write it to disk first.

---

## 2. Recommended Roadmap

1. **[v0.13 — Buffer Pool Structure](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-04-buffer-pool/v0.13-buffer-pool.md)**: Pre-allocate the memory array of frames and set up the page mapping table.
2. **[v0.14 — Pinning & Unpinning](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-04-buffer-pool/v0.14-pin-unpin.md)**: Manage page pins so that active pages are locked in memory and cannot be evicted while in use.
3. **[v0.15 — Dirty Page Tracking](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-04-buffer-pool/v0.15-dirty-pages.md)**: Track which pages have been modified in RAM, ensuring they are flushed to disk before eviction.
4. **[v0.16 — Clock Eviction Policy](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-04-buffer-pool/v0.16-clock-lru-eviction.md)**: Implement the Clock Sweep algorithm to decide which pages to evict when the pool is full.
