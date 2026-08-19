# MiniVectorDB Performance Engineering Strategy

A serious database engine must optimize for hardware execution paths. This document outlines how MiniVectorDB measures, profiles, and optimizes key resource domains: CPU, RAM, and Disk I/O.

---

## 1. Storage & Disk Throughput

Disk operations are orders of magnitude slower than memory lookups. Optimization here centers on sequential access patterns and batching.

### Target Performance Metrics
- **Sequential Write Speed**: Measure write throughput when inserting a batch of 100,000 documents.
- **Random Read Latency**: Measure average lookup time for point queries on document records.
- **Buffer Pool Hit Rate**:
  $$\text{Hit Rate} = \frac{\text{Cache Hits}}{\text{Cache Hits} + \text{Cache Misses}} \times 100$$
- **Eviction Overhead**: Measure time spent finding victim frames and flushing dirty pages during a cache miss.

### Design Guidelines
- **Sequential Disk Writes**: Minimize random seek times on mechanical/solid-state storage by logging modifications sequentially to the Write-Ahead Log (WAL).
- **Aligned Page Allocation**: Match physical pages to system block sizes (4096 bytes) to perform single-sector aligned disk read/writes, preventing write amplification.

---

## 2. Lock Contention & Transaction Throughput

In concurrent operations, thread synchronization bottlenecks often limit scalability.

### Target Performance Metrics
- **Commit Latency**: Time from executing a `COMMIT` statement to the WAL sync operation returning.
- **Lock Acquisition Queue Length**: Average number of transactions blocked waiting for resource locks.
- **Throughput (TPS)**: Total Transactions Per Second executed under a write-heavy workload with multiple concurrent threads.

### Design Guidelines
- **Fine-Grained Locking**: Acquire locks on page levels or record levels (RIDs) rather than locking the entire database or collection.
- **Lock Escapes & Fast-Paths**: Avoid context switching. For short-lived read locks, use spinning atomic operations before allocating wait-queue mutex structures.
- **Latch-Free Read Snapshots**: Use MVCC visibility lists for read operations. Readers read older versions of records without acquiring locks, avoiding blockages between readers and writers.

---

## 3. Vector Calculation & Graph Traversal

Vector engines are CPU and memory bound. Performance depends on layout caching and compiler optimizations.

### Target Performance Metrics
- **Brute-Force KNN Latency**: Query time on datasets from $10^3$ to $10^6$ vectors.
- **HNSW Latency vs. Recall**: Measure queries per second (QPS) at 90%, 95%, and 99% recall targets.
- **HNSW Build & Indexing Time**: Time required to construct the graph structure for 100,000 vectors.
- **Index Memory Footprint**: Average bytes consumed per vector in HNSW graphs.

### Design Guidelines
- **SIMD Vectorization**: Accelerate distance equations (L2, Cosine) using SIMD intrinsics (AVX2, AVX-512, NEON) to compute distance operations in parallel.
- **Cache-Friendly Traversal**: Place neighboring vector descriptors sequentially in memory. When traversing HNSW graph nodes, ensure neighbor vectors are prefetched to avoid CPU cache misses.
- **Avoiding Allocations in Inner Loops**: Pre-allocate search queues, distance arrays, and visitor tables once per query execution instead of executing `std::vector` allocations during graph routing steps.

---

## 4. Benchmarking Infrastructure

MiniVectorDB integrates **Google Benchmark** to run micro-benchmarks on critical functions.

### Run Procedures
To run the benchmarks, compile the repository under the `Release` build configuration and execute:
```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/benchmarks/benchmark_storage
./build/benchmarks/benchmark_vector
```

### Profiling Tools
Use the following tools to diagnose bottlenecks:
- **Windows**: Use Windows Performance Toolkit (WPT) or Visual Studio Profiler to identify hot code paths and lock contention.
- **Linux/macOS**: Use `perf` or `instruments` to examine cache-miss rates, instruction-retired rates, and system call frequencies.
