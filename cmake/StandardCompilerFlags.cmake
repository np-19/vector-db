add_library(minivectordb_compiler_flags INTERFACE)

if(MSVC)
    target_compile_options(minivectordb_compiler_flags INTERFACE /W4 /WX)
else()
    target_compile_options(minivectordb_compiler_flags INTERFACE -Wall -Wextra -Wpedantic -Werror)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT MSVC AND NOT MINGW)
    target_compile_options(minivectordb_compiler_flags INTERFACE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(minivectordb_compiler_flags INTERFACE -fsanitize=address,undefined)
endif()
