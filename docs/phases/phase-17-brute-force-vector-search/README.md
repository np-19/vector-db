# Phase 17: Brute-Force Vector Search

In this phase, you will implement the baseline search engine of your vector database: **Brute-Force k-Nearest Neighbor (kNN) Search**. To find the most similar documents to a query embedding, you will build a flat scanning executor that computes exact distances to every vector in a collection. You will optimize the top-K selection using a **Bounded Max-Heap Priority Queue** to prevent expensive sorting overhead and write an observability collector to track search metrics.

---

## 1. High-Level Vision: The Baseline Ground Truth

To search for similar embeddings, we need to compare a query vector against all vectors stored in our database. 

1. **Brute-Force Flat Scan**: We iterate through every slotted page in a collection, extract the document vector, compute its exact distance from the query, and collect the results. This guarantees **100% Recall** (meaning we find the absolute closest vectors in the database, with no approximations), providing a baseline of ground truth.
2. **Top-K Heap Optimization**: If a collection has 100,000 vectors, sorting the entire list of 100,000 distances to find the top 5 closest vectors is a massive bottleneck ($O(N \log N)$ complexity). Instead, we use a **Bounded Max-Heap** of size $K$:

```text
  [ Scan Record ] ──► Compute distance (d = 0.45)
                           │
                           ▼
              (Check Max-Heap size is 5)
                           │
             ┌─────────────┴─────────────┐
             ▼ (Heap is full)            ▼ (Heap has space)
    Is d < heap.top()?                     Push directly to heap.
    Yes: Pop furthest (top) element, 
         Push new element (d).
```

By keeping a max-heap of size $K$, we discard far-away vectors immediately, reducing search sorting complexity to $O(N \log K)$.
3. **Observability Metrics**: We log the time taken for search sweeps and count the number of distance calculations to evaluate search efficiency.

---

## 2. Recommended Roadmap

1. **[v0.68 — Brute-Force kNN](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-17-brute-force-vector-search/v0.68-brute-force-knn.md)**: Scan all vectors in a collection to return the exact nearest neighbors.
2. **[v0.69 — Top-K Heap Optimization](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-17-brute-force-vector-search/v0.69-top-k-optimization.md)**: Build a bounded priority queue to prune far matches.
3. **[v0.70 — Search Metrics Profiling](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-17-brute-force-vector-search/v0.70-search-metrics.md)**: Record latency and distance calculations.
