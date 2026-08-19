# Phase 19: Metadata + Vector Hybrid Search

In this phase, you will implement the hybrid querying engine of your database: **Metadata + Vector Hybrid Search**. Users rarely search vectors in isolation; they want to find similar items filtered by structured metadata constraints (e.g. *“Find the nearest articles where category = 'database'”*). You will implement pre-filtering and post-filtering execution strategies, write a single-stage **In-Graph Filter Pushdown** algorithm, and build a Cost-Based Optimizer to choose the most efficient plan based on filter selectivity.

---

## 1. High-Level Vision: Combining Structured & Unstructured Data

In vector databases, executing metadata filters alongside vector similarity is a core challenge. We have three query strategies:

```text
  [ Strategy 1: Pre-Filtering ]  ──► Scan metadata index (e.g. B+ Tree) first
                                      Build a set of allowed Record IDs (RIDs)
                                      Perform vector search only on this set.
                                      * Best when filter matches very few records (Selectivity < 5%).
                                 
  [ Strategy 2: Post-Filtering ] ──► Perform normal vector search first
                                      Filter out results that don't match the metadata criteria.
                                      * Worst strategy: if filter is rare, we get 0 matching results.
                                 
  [ Strategy 3: In-Graph Push ]  ──► Traverse HNSW graph normally.
                                      Evaluate metadata filter on candidate nodes BEFORE
                                      computing vector distance, skipping incompatible paths.
                                      * Best when filter matches many records.
```

1. **Pre-Filtering (Set Matching)**: Scans a B+ Tree index on the metadata column, gathers a set of allowed document RIDs, and restricts the vector similarity comparisons to this list.
2. **In-Graph Filter Pushdown**: Evaluates the metadata expression directly on the candidate nodes during HNSW graph traversal routing, avoiding fetching data pages for non-matching records.
3. **Hybrid Optimizer (Cost-Based Planning)**: Computes the filter selectivity (fraction of rows matching the criteria). If selectivity is low (highly restrictive filter, e.g. < 5% matches), it selects the **Pre-Filtering** plan. Otherwise, it selects the **In-Graph Pushdown** plan.

---

## 2. Recommended Roadmap

1. **[v0.77 — Metadata Filtering](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-19-hybrid-search/v0.77-metadata-filtering.md)**: Implement pre-filtering and post-filtering execution.
2. **[v0.78 — In-Graph Filter Pushdown](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-19-hybrid-search/v0.78-hybrid-search.md)**: Evaluate expressions during HNSW graph routing.
3. **[v0.79 — Hybrid Query Optimizer](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-19-hybrid-search/v0.79-query-optimization.md)**: Choose the cheapest plan dynamically based on selectivity statistics.
