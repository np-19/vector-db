add_library(minivectordb
    document/document.cpp
    document/value.cpp
    document/serializer.cpp
    storage/disk_manager.cpp
)

target_include_directories(minivectordb
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(minivectordb PRIVATE minivectordb_compiler_flags)

add_library(MiniVectorDB::minivectordb ALIAS minivectordb)
