# Change Log

All notable changes to this project will be documented in this file.

## [2026-08-20] - Ignore Markdown Documentation

### Modified

## [2026-08-20] - Untrack Documentation Folder

### Modified
- [MODIFY] [`.gitignore`](.gitignore): Keep the local `docs/` directory and Markdown files excluded from future Git tracking.
- [REMOVE FROM INDEX] `docs/`: Removed the documentation folder from Git tracking without deleting local files.

## [2026-08-20] - MinGW Debug Build Compatibility

### Modified
- [MODIFY] [`cmake/StandardCompilerFlags.cmake`](cmake/StandardCompilerFlags.cmake): Avoided enabling AddressSanitizer and UndefinedBehaviorSanitizer for MinGW builds whose GCC runtime does not provide `libasan` and `libubsan`.
- [MODIFY] [`Decisions.md`](Decisions.md): Recorded the sanitizer compatibility decision and its trade-off.
- [MODIFY] [`Flow.md`](Flow.md): Documented the Debug sanitizer flag path and MinGW bypass.
- [MODIFY] [`docs/cmake-guide.md`](docs/cmake-guide.md): Clarified that Debug sanitizers require supported non-MSVC, non-MinGW runtimes.
- [MODIFY] [`docs/phases/phase-01-foundation/v0.1-project-setup.md`](docs/phases/phase-01-foundation/v0.1-project-setup.md): Documented the MinGW sanitizer exception.

## [2026-08-15] - Disk Manager Implementation & Documentation Traceability

### Added
- [NEW] [`src/storage/disk_manager.cpp`](file:///d:/C++/Projects/mini-vector-db/src/storage/disk_manager.cpp): Implemented the full `DiskManager` lifecycle for opening, creating, closing, reading pages, writing pages, and flushing the file stream.

### Modified
- [MODIFY] [`include/storage/disk_manager.hpp`](file:///d:/C++/Projects/mini-vector-db/include/storage/disk_manager.hpp): Kept the public interface aligned with the storage abstraction used by the project.
- [MODIFY] [`src/storage/disk_manager.cpp`](file:///d:/C++/Projects/mini-vector-db/src/storage/disk_manager.cpp): Added validation for empty paths, parent-directory creation, binary file access, page-aligned reads/writes, and runtime exceptions on IO failures.

## [2026-08-15] - Disk Manager Persistence Validation

### Added
- [NEW] [`tests/foundation_test.cpp`](file:///d:/C++/Projects/mini-vector-db/tests/foundation_test.cpp): Added a real persistence test that writes a 4 KB page, reopens the manager, reads it back, and asserts byte-for-byte equality across the reopen.

### Modified
- [MODIFY] [`include/storage/disk_manager.hpp`](file:///d:/C++/Projects/mini-vector-db/include/storage/disk_manager.hpp): Explicitly made the destructor public and grouped private state under a `private:` section so the manager can be instantiated in tests and cleaned up safely.
- [MODIFY] [`docs/phases/phase-01-foundation/v0.3-serialization.md`](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-01-foundation/v0.3-serialization.md): Fixed Mermaid subgraph labels by quoting names that include numeric prefixes, periods, and parentheses so the diagram parses without the `Expecting ... got 'PS'` syntax error.

## [2026-08-15] - Mermaid Diagram Syntax Correction

## [2026-08-14] - Document Serialization & Foundation Refinements

### Added
- [NEW] [`include/minivectordb.hpp`](file:///d:/C++/Projects/mini-vector-db/include/minivectordb.hpp): Created an umbrella header to simplify includes for library consumers.
- [NEW] [`src/document/serializer.cpp`](file:///d:/C++/Projects/mini-vector-db/src/document/serializer.cpp): Implemented the binary serialization/deserialization logic for `Document` and `Value`.

### Modified
- [MODIFY] [`include/minivectordb/document/document.hpp`](file:///d:/C++/Projects/mini-vector-db/include/minivectordb/document/document.hpp): Added `#pragma once` guard to prevent class redefinition compiler errors.
- [MODIFY] [`include/minivectordb/document/serializer.hpp`](file:///d:/C++/Projects/mini-vector-db/include/minivectordb/document/serializer.hpp): Declared `Serialize` and `Deserialize` static methods in the `DocumentSerializer` class.
- [MODIFY] [`include/minivectordb/document/value.hpp`](file:///d:/C++/Projects/mini-vector-db/include/minivectordb/document/value.hpp): Moved the template function implementation `Value::As<T>()` inline to the header and included `<stdexcept>` to resolve compilation and linkage issues across different compilation units.
- [MODIFY] [`src/document/value.cpp`](file:///d:/C++/Projects/mini-vector-db/src/document/value.cpp): Removed the out-of-line template definition of `Value::As<T>()`.
- [MODIFY] [`src/CMakeLists.txt`](file:///d:/C++/Projects/mini-vector-db/src/CMakeLists.txt): Added `document/document.cpp` and `document/serializer.cpp` to the library source files list for building target `minivectordb`.
- [MODIFY] [`apps/main.cpp`](file:///d:/C++/Projects/mini-vector-db/apps/main.cpp): Updated to write a minimal program showing document creation, binary serialization, printing bytes in hex format, deserialization, and round-trip verification.
- [MODIFY] [`tests/foundation_test.cpp`](file:///d:/C++/Projects/mini-vector-db/tests/foundation_test.cpp): Added test cases for round-trip validation, determinism, safety checks (correctly catching buffer overflow exceptions on truncated buffers), and empty document handling.
- [MODIFY] [`docs/phases/phase-01-foundation/v0.2-document-model.md`](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-01-foundation/v0.2-document-model.md): Added a Mermaid class diagram showing class relationships.
- [MODIFY] [`docs/phases/phase-01-foundation/v0.3-serialization.md`](file:///d:/C++/Projects/mini-vector-db/docs/phases/phase-01-foundation/v0.3-serialization.md): Added a Mermaid flow diagram and visual block table representing the byte layout format.
