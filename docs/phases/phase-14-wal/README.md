# Phase 14: Write-Ahead Logging (WAL)

In this phase, you will implement the core durability subsystem of your database: **Write-Ahead Logging (WAL)**. You will build binary log records, track changes using **Log Sequence Numbers (LSNs)**, design a buffered **WAL Writer**, and enforce the **WAL Rule** to guarantee that data modifications are securely logged to disk before their corresponding pages are flushed from RAM.

---

## 1. High-Level Vision: Write Logs First

If a database writes modified pages directly to the database file on disk, a sudden power failure mid-write will corrupt the slotted page blocks. 

To prevent this, databases use **Write-Ahead Logging (WAL)**. Instead of writing heavy 4KB page files immediately, we write a small, sequential stream of binary change records (red/undo entries) to a separate log file (`minivectordb.wal`):

```text
      [ Insert/Update Query ] 
                 │
                 ▼  (Generate sequential LSN)
     [ LogRecord (LSN=42) ] ──► Append to RAM Log Buffer
                 │
           (Page Write)
                 ▼
  [ Data Page in RAM (PageLSN=42) ]
                 │
         (Fuzzy Checkpoint / Eviction)
                 ▼
        ┌────────────────────────────────────────────────────────┐
        │                 The CRITICAL WAL Rule                  │
        ├────────────────────────────────────────────────────────┤
        │ We must flush the WAL log file to disk (up to LSN 42)  │
        │ BEFORE we are allowed to write the data page to disk!  │
        └───────────────────────────┬────────────────────────────┘
                                    │
                                    ▼
                         [ Data Page Written to Disk ]
```

1. **Log Sequence Numbers (LSNs)**: Every log record gets an incremental ID called an LSN. Every page in memory also stores the LSN of the last modification that touched it (`page.header.pageLSN`).
2. **Log Serialization**: We convert structural record operations (like inserts, updates, and deletes) into serialized binary byte frames.
3. **The WAL Rule**: Before the Buffer Pool Manager writes a dirty page to disk, it compares the page LSN with the flushed LSN of the WAL. If the log record has not reached disk yet, it forces the WAL Writer to flush the log buffer *first*, ensuring we never have unlogged changes on disk.
4. **Group Commits**: To optimize I/O, commits append a `COMMIT` record to the WAL buffer and flush the log buffer to disk, ensuring Durability (ACID).

---

## 2. Recommended Roadmap

1. **[v0.57 — WAL Abstraction](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-14-wal/v0.57-wal-abstraction.md)**: Establish the WAL file layout and LSN generation structures.
2. **[v0.58 — Log Records](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-14-wal/v0.58-log-records.md)**: Define binary log record formats and serialize change properties.
3. **[v0.59 — WAL Writer](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-14-wal/v0.59-wal-writer.md)**: Build the double-buffered log flusher enforcing the WAL rule.
4. **[v0.60 — Commit Durability](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-14-wal/v0.60-commit-durability.md)**: Flush logs on commit to guarantee transaction persistence.
