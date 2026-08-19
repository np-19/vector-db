# AI-Assisted Coding Guidelines

When writing or modifying code in this project, you must maintain and update the following three tracking files to ensure transparency, traceability, and architectural clarity.

---

## 1. `ChangeLog.md`

**Purpose:** Tracks *what* changes were made to the codebase.

- **Requirement:** Update this file whenever any code or asset is modified, added, or deleted.
- **Goal:** Keeps the developer fully informed about all incremental changes happening across the project.
- **What to include:**
  - Date and timestamp (or commit reference).
  - List of modified, added, or deleted files.
  - Brief description of the exact code changes made.

---

## 2. `Decisions.md`

**Purpose:** Explains *why* specific technical decisions were made.

- **Requirement:** Log significant design choices, architectural decisions, and rationale.
- **Goal:** Provides transparency into the reasoning behind code design and implementation details.
- **What to include:**
  - Why a specific language, library, or framework was selected.
  - Rationale for chosen patterns, algorithms, or test strategies.
  - Trade-offs considered and rejected alternatives.

---

## 3. `Flow.md`

**Purpose:** Maps *how* the execution and data flow through the codebase.

- **Requirement:** Maintain a high-level and detailed structural flow of functions, modules, and pipeline interactions.
- **Goal:** Helps trace bugs quickly by clarifying whether an issue stems from upstream inputs or downstream dependencies.
- **What to include:**
  - Component and function execution pipelines.
  - Data flow between modules/services.
  - Dependency relationships across the codebase.