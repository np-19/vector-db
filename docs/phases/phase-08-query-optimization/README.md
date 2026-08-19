# Phase 8: Query Optimization

In this phase, you will build the query optimizer of your database. You will transition from directly executing queries to compiling them through **Logical Plans** and **Physical Plans**, analyzing cost trade-offs with a **Cost Model**, and implementing the **EXPLAIN** interface to visualize query plans.

---

## 1. High-Level Vision: The Query Optimizer

In a production database, we don't just execute the query filter exactly as written. Instead, the query goes through a compile and optimization phase before running:

```text
  [ User Query ] ──► {"age": {"$gt": 21}}
                           │
                           ▼  (Parser)
                     [ AST Tree ]
                           │
                           ▼  (Logical Planner)
                    [ Logical Plan ]  ──► WHAT to do (LogicalScan, LogicalFilter)
                           │
                           ▼  (Cost-Based Optimizer)
                    [ Physical Plan ] ──► HOW to do (PhysicalIndexScan vs. PhysicalSeqScan)
                           │
                           ▼  (Executor compiler)
                 [ Volcano Executors ] ──► Next() streams results
```

1. **Logical vs. Physical Decoupling**: A **Logical Plan** describes the relational algebra of the query (e.g., *"We want to scan collection 'users' and apply a filter"*). A **Physical Plan** specifies the exact algorithms to perform those steps (e.g., *"We will use a B+ Tree index scan"*).
2. **Cost-Based Optimization (CBO)**: Instead of always using an index, the optimizer estimates the physical cost (page reads + CPU work) of both options. If the table is tiny (under 5 pages), scanning it sequentially is faster than loading index pages and dereferencing RIDs, so the optimizer will choose a `SeqScan`!
3. **Observability (EXPLAIN)**: We implement the `EXPLAIN` command, formatting query execution plans into structured ASCII trees so developers can inspect planning decisions.

---

## 2. Recommended Roadmap

1. **[v0.32 — Logical Plans](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-08-query-optimization/v0.32-logical-plans.md)**: Represent abstract relational operations in memory.
2. **[v0.33 — Physical Plans](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-08-query-optimization/v0.33-physical-plans.md)**: Define executor-mapping nodes.
3. **[v0.34 — Index Selection Rules](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-08-query-optimization/v0.34-index-selection.md)**: Implement rules to check index availability.
4. **[v0.35 — Cost-Based Optimizer](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-08-query-optimization/v0.35-basic-cost-model.md)**: Estimate page reads and CPU cycles to select plans.
5. **[v0.36 — EXPLAIN Interface](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-08-query-optimization/v0.36-explain.md)**: Compile physical plans to readable ASCII trees.
