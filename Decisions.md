# Architectural Decisions Log

## 6. MinGW Debug Sanitizer Compatibility
### Context
The configured UCRT MinGW GCC installation accepts sanitizer flags during compilation but does not ship the `libasan` and `libubsan` libraries required at link time.

### Decision
Apply Debug AddressSanitizer and UndefinedBehaviorSanitizer flags for non-MSVC, non-MinGW toolchains only. MinGW Debug builds retain strict warnings and build without sanitizer instrumentation.

### Rationale
This keeps the default Windows build usable with the available toolchain while preserving sanitizers on GCC and Clang environments that provide their runtime libraries. Enabling the flags unconditionally causes every Debug executable link to fail before tests can run.

## 1. Document Layout Format Design (v0.3)
### Context
We needed to serialize a dynamically-typed `Document` containing a primary key `id_` (`uint64_t`), vector embeddings (`std::vector<float>`), and a metadata map (`std::map<std::string, Value>`).

### Decision
Standardize on a binary format with the following segments:
1. **Fixed-Size Header (14 bytes)**:
   - Document ID: 8 bytes (`uint64_t`)
   - Embedding Dimensions: 4 bytes (`uint32_t`)
   - Metadata Count: 2 bytes (`uint16_t`)
2. **Vector Segment**: `dimensions * 4` bytes.
3. **Metadata Segment**: Repeated entries, each with `key_len` (2 bytes), key characters (raw ASCII), `type_byte` (1 byte), and type-specific value payload.

### Rationale
* **Compactness**: A binary representation avoids JSON/XML text serialization parsing overhead and significantly reduces disk usage.
* **Header-First Layout**: Storing dimensions and field counts first allows the deserializer to pre-allocate vectors (`reserve()`) and map nodes, improving speed and preventing unnecessary heap reallocations.

### Endianness
* We standardized on **Little-Endian** for all integer fields and copied raw floats/doubles using the system's native format (little-endian on x86/ARM). This ensures portability of the database files across different hardware systems while matching the native CPU format of modern architectures for maximum speed.

---

## 2. Moving `Value::As<T>()` Template Method to the Header
### Context
The template method `Value::As<T>()` was originally defined inside `src/document/value.cpp`. When `serializer.cpp` or `foundation_test.cpp` attempted to use `val.As<bool>()` or `val.As<std::string>()`, compile/link errors occurred due to undefined symbol references at link time.

### Decision
Move the definition of `Value::As<T>()` directly inline into `include/minivectordb/document/value.hpp` and include `<stdexcept>` in the header file.

### Rationale
In C++, template code must be visible to the compiler when it instantiates the template (unless explicit instantiation is used in the implementation file). Inline headers are the standard, robust way to ensure that any compilation unit using the template compiles and links successfully.

---

## 3. Creating an Umbrella Header `minivectordb.hpp`
### Context
As the project grows in source files, requiring clients to include multiple headers (`document.hpp`, `value.hpp`, `serializer.hpp`) increases cognitive overhead and clutter.

### Decision
Create a single public umbrella header at `include/minivectordb.hpp` that includes all core headers.

### Rationale
Simplifies user integration. Consumers can just `#include "minivectordb.hpp"` to initialize and use any class in the namespace `DB`.

---

## 4. Mermaid Diagram Syntax Hygiene
### Context
Several architecture diagrams in the documentation used bare `subgraph` names such as `1. Document Header Details` and `3. Metadata Field (Repeated)`. Mermaid treats those names as syntax tokens rather than labels, which triggers parse errors during rendering.

### Decision
Always quote subgraph labels and node labels when they contain numeric prefixes, punctuation, or parentheses. This keeps the diagram source machine-readable while preserving clear human-readable captions.

### Rationale
Documentation diagrams are part of the developer interface. A single malformed label breaks the entire rendering pipeline and obscures the architecture being explained. Defensive quoting ensures the docs remain valid across Mermaid versions and renderers.

---

## 5. Disk Manager Ownership and File Semantics
### Context
The database needs a durable file abstraction that can create or reopen the backing database file, read/write fixed-size pages, and flush OS/runtime buffers to stable storage without leaking file handles or corrupting the binary byte stream.

### Decision
Implement `DiskManager` as a thin file controller over a single `std::fstream` handle. `Open()` creates parent directories and the file when missing, reopens in binary read/write mode, and throws `std::runtime_error` on failures. `ReadPage()` and `WritePage()` use `PAGE_SIZE` and `seekg`/`seekp` with strict validation to avoid partial transfer issues.

### Rationale
A database engine depends on fixed-size page writes and file durability. Using one explicit file manager encapsulates the OS details and keeps storage code predictable, auditable, and easier to test than scattering raw file reads across other subsystems.
