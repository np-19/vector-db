# Benchmarking MiniVectorDB

A benchmark measures how long code takes to run. Tests answer “is this correct?” Benchmarks answer “how fast is this?” A benchmark can succeed even if code is slow; it measures performance, not correctness.

MiniVectorDB uses **Google Benchmark** for measurements.

## Benchmark flow

```text
benchmarks/foundation_bench.cpp → compiled by CMake → foundation_bench.exe
                                                        ↓
                                           repeatedly runs your function
                                                        ↓
                                           time results in the terminal
```

The root `CMakeLists.txt` includes `benchmarks/` when `MINIVECTORDB_BUILD_BENCHMARKS` is enabled. `benchmarks/CMakeLists.txt` creates `foundation_bench` and links it to the database library and Google Benchmark. Do not run CMake inside `benchmarks/`.

## Build and run the existing benchmark

From the project root:

```powershell
cmake -S . -B build
cmake --build build
.\build\benchmarks\foundation_bench.exe
```

CMake downloads Google Benchmark automatically during configuration if it is missing. The benchmark program repeatedly runs the registered function and reports timings.

To skip benchmarks during normal development:

```powershell
cmake -S . -B build -DMINIVECTORDB_BUILD_BENCHMARKS=OFF
```

Enable them again with:

```powershell
cmake -S . -B build -DMINIVECTORDB_BUILD_BENCHMARKS=ON
```

## Read the existing benchmark

```cpp
static void BM_Sanity(benchmark::State& state) {
    for (auto _ : state) {
        int x = 1 + 1;
        benchmark::DoNotOptimize(x);
    }
}

BENCHMARK(BM_Sanity);
BENCHMARK_MAIN();
```

`BM_Sanity` is the measurement function. Google Benchmark controls the loop and runs its body many times for a reliable time.

`benchmark::DoNotOptimize(x)` tells the compiler that `x` matters. Without it, the compiler might remove the unused calculation and give a meaningless timing.

`BENCHMARK(BM_Sanity)` registers the function. `BENCHMARK_MAIN()` creates the usual C++ `main()` function, so this folder does not need a separate `main.cpp`.

## Add a benchmark for new code

First make a feature correct with tests. Benchmark it only if performance matters.

For example, after implementing a vector distance function:

```cpp
static void BM_Distance(benchmark::State& state) {
    const DB::VectorType left(768, 1.0F);
    const DB::VectorType right(768, 2.0F);

    for (auto _ : state) {
        const auto result = Distance(left, right);
        benchmark::DoNotOptimize(result);
    }
}

BENCHMARK(BM_Distance);
```

Put setup that should not be measured before the loop. Put only the operation you want to measure inside the loop. Here vector creation is excluded, so the timing focuses on `Distance`.

## Interpret results carefully

Results vary with laptop workload, power mode, compiler, and build type. Compare results on the same machine, preferably in Release mode:

```powershell
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release
.\build-release\benchmarks\foundation_bench.exe
```

Do not optimize code until it is correct and a benchmark shows what needs improvement.

## What Google Benchmark is doing for you

Timing one function call is unreliable: your operating system may switch tasks, the CPU may change speed, and a clock has limited precision. Google Benchmark runs the loop many times, chooses an appropriate number of iterations, and reports an average-like timing.

The range-for line is special:

```cpp
for (auto _ : state) {
    // code being measured
}
```

Do not replace it with a normal `for (int i = 0; ...)` loop. `state` is how Google Benchmark knows which work should be timed.

## What a result means

Typical output has columns like this:

```text
------------------------------------------------
Benchmark             Time             CPU   Iterations
------------------------------------------------
BM_Sanity          1.20 ns         1.20 ns    500000000
```

- **Time**: elapsed real-world time per operation.
- **CPU**: CPU time used per operation. On a quiet machine it is often close to `Time`.
- **Iterations**: how many times Google Benchmark ran the loop.

The exact numbers will differ across computers. Compare an old and new version on the same computer, with the same build type, while other demanding programs are closed.

## Benchmark parameters

You can measure several input sizes using `Range`:

```cpp
static void BM_Insert(benchmark::State& state) {
    const auto count = static_cast<std::size_t>(state.range(0));
    for (auto _ : state) {
        // measure inserting `count` items
    }
}

BENCHMARK(BM_Insert)->Range(8, 8192);
```

Google Benchmark runs the same function repeatedly with increasing values from 8 to 8192. This is useful for database operations because their speed may change as collections or vectors become larger.

## Setup, teardown, and realistic work

Code before the `for (auto _ : state)` loop is setup and is not measured. Put allocation, sample documents, or test vectors there if you only want to measure the operation itself.

If creation is actually part of the operation you want users to experience, place it inside the loop. State clearly in the benchmark name or comments what is included. A benchmark is only useful when its measured work is unambiguous.

## Benchmark safely

- First write and run correctness tests.
- Keep benchmark input deterministic; avoid random data unless you intentionally control the random seed.
- Do not print to the terminal inside the measured loop; printing dominates timing.
- Do not compare Debug and Release numbers. Debug builds intentionally prioritize debugging over speed.
- Keep old benchmark results before changing an optimization, so you can tell whether it improved anything.

## Useful command-line options

Run only matching benchmarks:

```powershell
.\build\benchmarks\foundation_bench.exe --benchmark_filter=Distance
```

Write results to JSON for later comparison:

```powershell
.\build\benchmarks\foundation_bench.exe --benchmark_format=json --benchmark_out=result.json
```

The command-line options are passed to the benchmark executable, not to CMake.
