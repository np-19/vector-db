# Phase 5: Catalog and Metadata Management

In this phase, you will build the central brain of your database: the **System Catalog**. You will implement schema definitions and collection routing so that the database can manage multiple independent, named collections and coordinate their index pointers.

---

## 1. High-Level Vision: The Database Directory

Until now, we have written records directly into database pages, but we had no way of grouping them. If a client wants to create two separate collections (like `users` and `products`), we need a directory map (the **Catalog**) to track where each collection begins:

```text
       ┌────────────────────────────────────────────────────────┐
       │                  In-Memory Catalog                     │
       ├────────────────────────────────────────────────────────┤
       │  "users"    ──► First Page: Page 1  ──► Vector Dim: 128 │
       │  "products" ──► First Page: Page 5  ──► Vector Dim: 256 │
       └───────────────────────────▲────────────────────────────┘
                                   │
                              Bootstrapping
                                   │
       ┌───────────────────────────┴────────────────────────────┐
       │                 Database File (Page 0)                 │
       ├────────────────────────────────────────────────────────┤
       │  Metadata Page: Serialized Catalog Schema & Indexes    │
       └────────────────────────────────────────────────────────┘
```

1. **Collection Management**: Users should be able to create, list, and drop logical collections.
2. **Catalog Metadata**: We need to track collection properties (names, vector dimensions) and index properties (which fields have B+ Trees or HNSW graphs and where their index root pages live).
3. **Bootstrapping**: When the database starts up, it must read Page 0, deserialize the catalog metadata, and reconstruct the in-memory catalog directory so it knows where to find collection data.

---

## 2. Recommended Roadmap

1. **[v0.17 — Collection Manager](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-05-catalog-and-metadata/v0.17-collection-manager.md)**: Implement named collection logical isolation and namespace mappings.
2. **[v0.18 — System Catalog](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-05-catalog-and-metadata/v0.18-catalog.md)**: Manage index definitions and link database properties in a central registry.
3. **[v0.19 — Persistent Metadata](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-05-catalog-and-metadata/v0.19-persistent-metadata.md)**: Serialize the catalog to Page 0, allowing schemas and indexes to survive database restarts.
