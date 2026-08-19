# Phase 7: The Query Engine

In this phase, you will build the execution pipeline that parses, plans, and executes queries: the **Query Engine**. You will implement Abstract Syntax Trees (AST) to represent document filters, and write Volcano-style (iterator) query executors that stream results record-by-record, saving system memory.

---

## 1. High-Level Vision: The Query Pipeline

When a user submits a query like `{"age": {"$gt": 21}}`, the database processes it through a multi-stage pipeline:

```text
  [ JSON Query String ]
          │
          ▼  (v0.27 Query Parser)
  [ AST Expression Tree ] ──► Represents "age > 21" in memory
          │
          ▼  (v0.31 Query Planner)
  [ Physical Plan Tree ]  ──► Decides whether to use IndexScan or SeqScan
          │
          ▼  (v0.28 Query Executor)
  [ Volcano Stream ]      ──► Pulls records one-by-one via Next() to the client
```

1. **Query Representation & Parser**: We translate raw query JSON strings into in-memory **Abstract Syntax Trees (AST)**. An AST represents nested logic (like `AND`/`OR` filters) as a tree of expression objects.
2. **Volcano-Style Execution**: Instead of loading the entire collection into memory to filter it (which consumes massive RAM), databases use the **Volcano Iterator Model**. Each executor (Scan, Filter, Limit) implements a simple `Next()` method. Calling `Next()` on the root executor pulls the next matching record up from the child executors one-by-one.
3. **Query Planning**: The planner inspects the catalog. If the filter field has a B+ Tree index, it compiles the query into an **Index Scan** plan; otherwise, it falls back to a sequential **Collection Scan** plan.

---

## 2. Recommended Roadmap

1. **[v0.26 — Query Representation](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-07-query-engine/v0.26-query-representation.md)**: Define expression trees (`Expr` classes) to evaluate document fields.
2. **[v0.27 — Query Parser](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-07-query-engine/v0.27-query-parser.md)**: Compile JSON query structures into expression trees.
3. **[v0.28 — Query Executor](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-07-query-engine/v0.28-query-executor.md)**: Define the Volcano `AbstractExecutor` interface.
4. **[v0.29 — Collection Scan](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-07-query-engine/v0.29-collection-scan.md)**: Implement the sequential page scanner (`SeqScanExecutor`) for unindexed queries.
5. **[v0.30 — Index Scan](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-07-query-engine/v0.30-index-scan.md)**: Implement the B+ Tree scanner (`IndexScanExecutor`) for fast indexed queries.
6. **[v0.31 — Basic Query Planner](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-07-query-engine/v0.31-basic-query-planner.md)**: Compile ASTs into physical scan plan executors based on index availability.
