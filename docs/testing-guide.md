# Testing MiniVectorDB

A test is a small program that checks whether code behaves as expected. Write the check once, then run it whenever your code changes. MiniVectorDB uses **GoogleTest** to write tests and **CTest** to run them.

## The testing flow

```text
tests/foundation_test.cpp → compiled by CMake → foundation_test.exe
                                                   ↓
                                            CTest runs it
                                                   ↓
                                             pass or fail
```

The root `CMakeLists.txt` includes `tests/`. `tests/CMakeLists.txt` creates the `foundation_test` program, links your database library and GoogleTest to it, and registers its individual tests with CTest. Do not run CMake separately inside `tests/`.

## Build and run existing tests

From the project root, run:

```powershell
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

The first command configures the project and downloads GoogleTest automatically if needed. The second compiles your code and the test program. The third runs every registered test and prints the reason if one fails.

To run the test program directly:

```powershell
.\build\tests\foundation_test.exe
```

CTest is normally better because it automatically runs every test in the project.

## Read the existing test

```cpp
TEST(FoundationTest, SanityCheck) {
    EXPECT_EQ(DB::INVALID_PAGE_ID, -1);
    EXPECT_EQ(sizeof(DB::PageID), 8U);
    EXPECT_EQ(sizeof(DB::Offset), 2U);
    EXPECT_EQ(1 + 1, 2);
}
```

`TEST(FoundationTest, SanityCheck)` gives the test a group name (`FoundationTest`) and an individual name (`SanityCheck`).

`EXPECT_EQ(left, right)` means “these two values should be equal.” If not, GoogleTest reports the file, line number, expected value, and actual value. `sizeof` checks a type’s byte size, which matters because a database writes data to disk.

## Add a test for new code

Suppose you add this function:

```cpp
int Add(int left, int right) {
    return left + right;
}
```

Write a test in an existing test file or create `tests/math_test.cpp`:

```cpp
#include <gtest/gtest.h>

TEST(MathTest, AddsTwoNumbers) {
    EXPECT_EQ(Add(2, 3), 5);
}
```

If you create a new `.cpp` test file, add it to the executable in `tests/CMakeLists.txt`:

```cmake
add_executable(foundation_test
    foundation_test.cpp
    math_test.cpp
)
```

Then build and test again.

## Useful expectations

```cpp
EXPECT_EQ(actual, expected);       // values are equal
EXPECT_NE(actual, unexpected);     // values are not equal
EXPECT_TRUE(condition);            // condition is true
EXPECT_FALSE(condition);           // condition is false
EXPECT_THROW(code, ExceptionType); // code throws the expected exception
```

For each feature, write a normal case, an edge case, and an invalid-input case where applicable.

## A simple habit

1. Write a small test describing the behavior you want.
2. Implement the feature.
3. Run the tests.
4. Fix failures before moving on.

Tests are separate programs, not part of your released application. They use the same MiniVectorDB library as your app.

## What happens when you run the commands

When you run `cmake -S . -B build`, `-S .` means “the source project is the current folder” and `-B build` means “place generated build files in the `build` folder.” CMake reads the root `CMakeLists.txt`, follows `add_subdirectory(tests)`, and reads `tests/CMakeLists.txt`.

That test CMake file creates a program called `foundation_test`. It links three things together:

```text
foundation_test.cpp + MiniVectorDB library + GoogleTest = foundation_test.exe
```

`cmake --build build` asks the selected build tool (Ninja on your machine) to compile only files that changed, then link the executable. `ctest --test-dir build` reads CMake's registered test list and starts each test executable.

## Your first real test: a useful pattern

Imagine a future function that checks whether a page ID is valid:

```cpp
bool IsValidPageID(DB::PageID id) {
    return id != DB::INVALID_PAGE_ID;
}
```

Test both expected outcomes:

```cpp
TEST(PageIDTest, AcceptsNormalPageID) {
    EXPECT_TRUE(IsValidPageID(42));
}

TEST(PageIDTest, RejectsInvalidPageID) {
    EXPECT_FALSE(IsValidPageID(DB::INVALID_PAGE_ID));
}
```

Each `TEST` should check one clear behavior. Short tests are easier to understand and easier to repair when they fail.

## EXPECT versus ASSERT

Most checks come in two forms:

```cpp
EXPECT_EQ(actual, expected); // records failure, then continues this test
ASSERT_EQ(actual, expected); // stops this test immediately if it fails
```

Use `EXPECT` for independent checks. Use `ASSERT` when later lines would be unsafe or meaningless after failure. For example, check that a returned pointer is not null with `ASSERT_NE(pointer, nullptr)` before using it.

## Test names and filtering

Test names appear as `SuiteName.TestName`. You can run one test directly:

```powershell
.\build\tests\foundation_test.exe --gtest_filter=FoundationTest.SanityCheck
```

Use this while developing one feature. Run the full `ctest` command before you consider the feature complete.

## If a test fails

Read the first failure, not just the final summary. GoogleTest normally shows:

1. the file and line containing the expectation;
2. the expected value;
3. the actual value.

Do not change an expected value merely to make a failing test pass. First decide whether the implementation is wrong or the original expectation was wrong.

## Common beginner mistakes

- Forgetting to include the header that declares the function being tested.
- Adding a new test `.cpp` file but forgetting to list it in `tests/CMakeLists.txt`.
- Testing private implementation details instead of public behavior.
- Making one test depend on another test having run first. Tests must work in any order.
- Not rebuilding after changing code. Use `cmake --build build` every time.
