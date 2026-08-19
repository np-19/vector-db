# CMake Guide for MiniVectorDB

This is a beginner guide to the build system in this repository. You do not need to memorize CMake. At first, learn what it does, which commands you run, and where to add files when the project grows.

## The problem CMake solves

C++ source code is not directly runnable. A compiler must turn every `.cpp` file into an object file, then a linker combines object files and libraries into an executable. A non-trivial project also needs include paths, compiler options, test libraries, and rules for rebuilding only changed files.

CMake does not compile C++ itself. It reads `CMakeLists.txt` files and generates instructions for another build tool. Here that build tool is Ninja.

```text
CMake files ──configure──> Ninja build files ──build──> .exe files
     ^                                  ^                  ^
     |                                  |                  |
source project                     build/ folder      apps/tests/benchmarks
```

## The three commands you will use

Run these from the project root (`mini-vector-db`):

```powershell
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

The first command configures. The second builds every normal target. The third runs tests; it does not compile anything.

Usually you do this after changing a `.cpp`, `.hpp`, or CMake file:

```powershell
cmake --build build
ctest --test-dir build --output-on-failure
```

Run the configure command again after changing CMake files, adding a source file, changing build options, or when CMake says it must reconfigure.

## Important words

| Term | Meaning in this project |
|---|---|
| source directory | The project files you edit: `mini-vector-db/`. |
| build directory | Generated files and executables: `mini-vector-db/build/`. Do not edit these files. |
| target | A thing CMake builds or applies settings to, such as a library or executable. |
| library | Reusable compiled code. `minivectordb` is the database library. |
| executable | A runnable program, such as `minivectordb_cli.exe`. |
| link | Combine an executable with libraries it uses. |
| include directory | A folder searched for C++ headers used with `#include`. |
| dependency | External code this project needs, such as GoogleTest. |
| configure | CMake reads project rules and creates build instructions. |

## Complete project flow

```text
You run: cmake -S . -B build
             |
             v
root CMakeLists.txt
  |-- loads compiler rules
  |-- downloads/sets up optional dependencies
  |-- reads src/CMakeLists.txt
  |     └─ creates minivectordb library
  |-- reads apps/CMakeLists.txt
  |     └─ creates minivectordb_cli executable using the library
  |-- reads tests/CMakeLists.txt (when tests are enabled)
  |     └─ creates foundation_test using library + GoogleTest
  └-- reads benchmarks/CMakeLists.txt (when enabled)
        └─ creates foundation_bench using library + Google Benchmark
             |
             v
You run: cmake --build build
             |
             v
Ninja compiles changed files and links executables
```

`add_subdirectory(folder)` is the instruction that makes one CMake file read another. It is not a second independent build. There is one configuration and one build folder.

## Root CMakeLists.txt

The root [CMakeLists.txt](../CMakeLists.txt) is the controller. It sets the project name, C++ version, test and benchmark options, and includes every project component.

```cmake
cmake_minimum_required(VERSION 3.20)
```

Requires a sufficiently recent CMake version.

```cmake
project(MiniVectorDB VERSION 0.1.0 LANGUAGES CXX)
```

Names this project and declares that it uses C++.

```cmake
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
```

Requests standard C++20, disallows silently falling back to an older version, and avoids non-standard compiler extensions.

```cmake
include(CTest)
```

Enables CTest and creates the `BUILD_TESTING` option. Tests are normally enabled.

```cmake
option(MINIVECTORDB_BUILD_BENCHMARKS "Build benchmark targets" ON)
```

Creates an on/off setting. You can set it at configure time with `-DMINIVECTORDB_BUILD_BENCHMARKS=OFF`.

```cmake
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(StandardCompilerFlags)
include(Dependencies)
```

Adds the project `cmake/` folder to CMake's search path, then imports `cmake/StandardCompilerFlags.cmake` and `cmake/Dependencies.cmake`.

Finally, the root uses `add_subdirectory` for `src`, `apps`, and enabled optional folders. This is how all internal CMake files are connected.

## cmake/StandardCompilerFlags.cmake

This file creates `minivectordb_compiler_flags`, an **INTERFACE library**. It contains settings but no C++ code and no output `.lib` file.

```cmake
add_library(minivectordb_compiler_flags INTERFACE)
```

The later `target_compile_options` lines store strict warning options in that settings target. MSVC gets `/W4 /WX`; GCC/Clang get `-Wall -Wextra -Wpedantic -Werror`.

The Debug-only sanitizer lines add checks for memory errors and undefined behavior on non-MSVC, non-MinGW GCC/Clang toolchains that provide the sanitizer runtimes. They must be present during both compilation and linking, which is why CMake uses both `target_compile_options` and `target_link_options`.

## cmake/Dependencies.cmake

This file imports CMake's built-in `FetchContent` module:

```cmake
include(FetchContent)
```

`FetchContent_Declare` states where a dependency can be downloaded. It does not build it by itself. `FetchContent_MakeAvailable` downloads it when absent, configures it, and makes its targets usable.

For GoogleTest, the available target used by this project is `GTest::gtest_main`. For Google Benchmark it is `benchmark::benchmark`.

The test dependency is inside `if(BUILD_TESTING)`, so it is skipped when tests are off. The benchmark dependency is inside `if(MINIVECTORDB_BUILD_BENCHMARKS)`, so it is skipped when benchmarks are off.

## src/CMakeLists.txt

```cmake
add_library(minivectordb document/value.cpp)
```

Creates the central library from source files. Every future implementation `.cpp` file belongs in this list.

```cmake
target_include_directories(minivectordb PUBLIC .../include)
```

Says that C++ code using the library can find public headers under `include/`. This is why application and test code can write:

```cpp
#include "minivectordb/common/types.hpp"
```

`PUBLIC` means the library needs this path and code that links to the library needs it too.

```cmake
target_link_libraries(minivectordb PRIVATE minivectordb_compiler_flags)
```

Applies strict compiler settings to the database library. `PRIVATE` means consumers of the library do not automatically inherit these flags.

The final alias line creates the conventional name `MiniVectorDB::minivectordb`, used by the app, tests, and benchmarks.

## apps/CMakeLists.txt

```cmake
add_executable(minivectordb_cli main.cpp)
target_link_libraries(minivectordb_cli PRIVATE MiniVectorDB::minivectordb)
```

Creates the application program and links it to the database library. When you add another application, create another `add_executable` target here and link it to the same library.

## tests/CMakeLists.txt

```cmake
add_executable(foundation_test foundation_test.cpp)
target_link_libraries(foundation_test PRIVATE MiniVectorDB::minivectordb GTest::gtest_main)
```

Creates a test program and links it to both your code and GoogleTest. `GTest::gtest_main` supplies the required C++ `main()` function.

```cmake
include(GoogleTest)
gtest_discover_tests(foundation_test)
```

Asks GoogleTest/CMake to discover every `TEST(...)` macro in the program and register it for the `ctest` command.

## benchmarks/CMakeLists.txt

```cmake
add_executable(foundation_bench foundation_bench.cpp)
target_link_libraries(foundation_bench PRIVATE benchmark::benchmark MiniVectorDB::minivectordb)
```

Creates the benchmark program and links the database library plus Google Benchmark. `BENCHMARK_MAIN()` in the `.cpp` file supplies that program's C++ `main()` function.

## Adding a new source file

For a new implementation `src/storage/page.cpp` and public header `include/minivectordb/storage/page.hpp`:

1. Create the header and source file.
2. Add `storage/page.cpp` to the `add_library(minivectordb ...)` list in `src/CMakeLists.txt`.
3. Run `cmake -S . -B build` because the source list changed.
4. Run `cmake --build build`.
5. Add and run a test.

## Common CMake issues

| Problem | Usual solution |
|---|---|
| CMake cannot find a new `.cpp` file | Add it to the appropriate `add_library` or `add_executable` list, then configure again. |
| `#include` file not found | Put public headers below `include/minivectordb/` and include them with the `minivectordb/...` path. |
| GoogleTest download fails | Check your internet connection, then rerun configure. |
| Build uses stale setup | Run `cmake -S . -B build` again. |
| Need a clean rebuild | Delete only the project's `build/` folder, then configure and build again. |
