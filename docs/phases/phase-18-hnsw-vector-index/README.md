# Phase 18: Hierarchical Navigable Small World (HNSW) Index

In this phase, you will implement the gold-standard vector index structure: the **Hierarchical Navigable Small World (HNSW)** index. To enable ultra-fast similarity searches over millions of vectors without linear scans, you will construct a layered proximity graph from scratch. You will write greedy routing algorithms to navigate the layers, build insertions with neighbor link pruning, handle node deletions, and map the graph nodes to database pages backed by the Buffer Pool.

---

## 1. High-Level Vision: Proximity Highway Networks

Brute-force kNN scans (Phase 17) take $O(N)$ linear time, which is too slow for large datasets. HNSW solves this by creating a multi-layer proximity graph. 

The graph acts like a highway system (similar to a skip-list):
* **Top Layers (Highways)**: Contain very few nodes with long-distance links. Search queries make large jumps to navigate quickly to the general neighborhood of the query.
* **Bottom Layers (Local roads)**: Contain all nodes with short-distance links. The search descends to these layers to perform fine-grained local routing and find the exact closest matches.

```text
  [ Layer 2 (Highway) ]    Node A ═════════════════════════════════► Node D
                             │                                         │
                             ▼ (Descend)                               ▼ (Descend)
  [ Layer 1 (Road) ]       Node A ──────────► Node B ────────────────► Node D
                             │                  │                      │
                             ▼ (Descend)        ▼ (Descend)            ▼ (Descend)
  [ Layer 0 (Street) ]     Node A ──► Node E ──► Node B ──► Node C ──► Node D
```

1. **Layered Data Model**: Each node (`HNSWNode`) holds a record `RID`, the raw vector embedding, and a list of neighbor links for each layer.
2. **Greedy Routing**: Start at the entry node on the top layer. Scan its neighbors, move to the neighbor closest to the query, and repeat. When no neighbor is closer, descend to the next layer and repeat, routing toward the local street level.
3. **Graph Construction**: During insertion, assign a node height using an exponential decay distribution. Search for its closest neighbors at each layer and establish bi-directional links, pruning connections to a maximum count $M$ to keep graph densities balanced.
4. **Graph Persistence**: Map graph nodes to physical database pages, allowing the index to survive restarts and scale beyond RAM using the Buffer Pool Manager.

---

## 2. Recommended Roadmap

1. **[v0.71 — HNSW Data Model](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-18-hnsw-vector-index/v0.71-hnsw-data-model.md)**: Define layered graph node link structures.
2. **[v0.72 — Graph Construction](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-18-hnsw-vector-index/v0.72-graph-construction.md)**: Generate probabilistic decay heights and connect layers.
3. **[v0.73 — Search & Greedy Routing](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-18-hnsw-vector-index/v0.73-search.md)**: Implement fast approximate search traversing layers.
4. **[v0.74 — Node Insertion](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-18-hnsw-vector-index/v0.74-insert.md)**: Insert vectors, establish links, and prune neighbors.
5. **[v0.75 — Node Deletion](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-18-hnsw-vector-index/v0.75-delete.md)**: Manage soft tombstones and reconnect orphaned neighbors.
6. **[v0.76 — Persistent HNSW](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-18-hnsw-vector-index/v0.76-persistent-hnsw.md)**: Map pointer-heavy graphs to disk pages.
