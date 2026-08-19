# Phase 1: Database Foundation

Welcome to the starting line of building your own vector database! 

In this first phase, you will set up your programming workspace and build the foundational layers that let your database represent, convert, and store records. By the end of this phase, you will have a working build pipeline that can compile C++ code, run tests, and write/read structured documents to a raw file on your hard drive.

---

## 1. High-Level Vision: The Storage Pipeline

Before we look at the code, let's understand how a document flows from your C++ program down to your physical storage disk:

```text
  [ C++ Object ] ──(v0.2 Document Model)──► In-memory representation (ID, Vector, metadata)
        │
        ▼
  [ Serialization ] ──(v0.3 Binary Marshaller)──► Flat sequence of bytes (std::vector<uint8_t>)
        │
        ▼
  [ Disk Manager ] ──(v0.4 Raw File descriptor)──► Physical file storage (.db file on disk)
```

1. **v0.1 — Project Setup**: We configure compiler warning flags, link test/benchmark suites (GoogleTest/Google Benchmark), and set up the Ninja build system so we can compile C++20 code quickly and safely.
2. **v0.2 — Document Model**: We create a C++ type system that can hold any JSON-like data structure dynamically. This is our database's "in-memory" document representation.
3. **v0.3 — Serialization**: Computer memory uses pointers to scattered addresses. Physical disks need sequential streams of data. We write code to serialize our in-memory documents into contiguous byte arrays.
4. **v0.4 — Persistent DB File**: We bypass standard, slow C++ file streams and write directly to disk files using low-level operating system APIs (raw file descriptors), ensuring data is safely flushed to physical hardware.

---

## 2. Educational Blueprint: What You Will Learn

### 📚 Database Engineering Concepts:
* **Storage Engines**: How database layout formats differ from standard file formats.
* **Schema Flexibility**: How document databases (like MongoDB) manage records without strict SQL-style tables.
* **Disk I/O Guarantees**: Why databases bypass standard OS caching to prevent corruption during sudden power failures.

### 💻 Modern C++ Concepts:
* **The Build System**: How CMake coordinates source files into executable programs.
* **Variant Types (`std::variant`)**: How to write type-safe variables that can hold different data types (strings, integers, floats, lists) dynamically.
* **Memory Layout & Pointers**: How to read and write bytes directly using raw memory buffers.
* **System Programming APIs**: How to talk to your operating system kernel (Windows/Linux) using C++ file APIs.

---

## 3. Recommended Roadmap

We recommend implementing these versions in order:
1. **[v0.1 — Project Setup](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-01-foundation/v0.1-project-setup.md)**
2. **[v0.2 — Document Model](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-01-foundation/v0.2-document-model.md)**
3. **[v0.3 — Serialization](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-01-foundation/v0.3-serialization.md)**
4. **[v0.4 — Persistent Database File](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-01-foundation/v0.4-persistent-db-file.md)**

Let's begin by opening **[v0.1 — Project Setup](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-01-foundation/v0.1-project-setup.md)**!
