cmake_minimum_required(VERSION 3.20)

project(MiniVectorDB VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) #disable compiler extensions

include(CTest)
option(MINIVECTORDB_BUILD_BENCHMARKS "Build benchmark targets" ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(StandardCompilerFlags)
include(Dependencies)

add_subdirectory(src)
add_subdirectory(apps)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

if(MINIVECTORDB_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()
