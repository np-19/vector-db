# Execution and Data Flow Maps

## 0. Compiler Flag Selection

```mermaid
flowchart TD
    configure[CMake configure] --> warnings[Apply compiler warnings]
    configure --> debug{Debug build?}
    debug -->|No| done[Build targets]
    debug -->|Yes| toolchain{MSVC or MinGW?}
    toolchain -->|Yes| done
    toolchain -->|No| sanitizers[Apply ASan, UBSan, and frame-pointer flags]
    sanitizers --> done
```

## 1. Document Serialization Pipeline

The pipeline below shows how a C++ `Document` object is converted into a sequential byte array:

```mermaid
graph TD
    doc[Document Object] --> serializer[DocumentSerializer::Serialize]
    serializer --> header[Write Header]
    serializer --> vector[Write Vector Segment]
    serializer --> metadata[Write Metadata Segment]

    subgraph Header Write
        header --> write_id[Write 8-Byte ID]
        header --> write_dim[Write 4-Byte Dimension]
        header --> write_count[Write 2-Byte Metadata Count]
    end

    subgraph Vector Write
        vector --> write_floats[Iterate & Write coordinates: 4 bytes/float]
    end

    subgraph Metadata Write
        metadata --> loop_fields[Iterate Metadata Map]
        loop_fields --> write_key[Write Key: 2B Length + Chars]
        loop_fields --> write_type[Write Value Type: 1B Type ID]
        loop_fields --> write_payload[Write Value Payload: 1B/8B/Var-length String]
    end

    write_id --> buffer[uint8_t Byte Buffer]
    write_dim --> buffer
    write_count --> buffer
    write_floats --> buffer
    write_payload --> buffer
```

---

## 2. Document Deserialization Pipeline

The pipeline below maps how a raw byte stream is parsed back into a C++ `Document` object:

```mermaid
graph TD
    bytes[Serialized uint8_t Vector] --> deserializer[DocumentSerializer::Deserialize]
    deserializer --> read_header[Read 14-byte Header]
    read_header --> read_vector[Read Vector Segment]
    read_vector --> read_meta[Read Metadata Segment]

    subgraph Header Reading
        read_header --> parse_id[Read uint64_t ID]
        read_header --> parse_dim[Read uint32_t Dimension]
        read_header --> parse_count[Read uint16_t Metadata Count]
    end

    subgraph Vector Reading
        read_vector --> parse_floats[Read 'Dimension' floats]
    end

    subgraph Metadata Reading
        read_meta --> loop_meta[Loop 'Metadata Count' times]
        loop_meta --> parse_key[Read Key: 2B Length + Chars]
        loop_meta --> parse_type[Read 1B Type]
        loop_meta --> parse_payload[Read Value Payload based on Type]
    end

    parse_id --> construct[Reconstruct Document]
    parse_floats --> construct
    parse_payload --> construct
```

---

## 3. Dependency Relationships

```mermaid
graph TD
    CLI[apps/main.cpp] --> Umbrella[include/minivectordb.hpp]
    Tests[tests/foundation_test.cpp] --> Umbrella
    Umbrella --> SerializerHPP[include/minivectordb/document/serializer.hpp]
    Umbrella --> DocumentHPP[include/minivectordb/document/document.hpp]
    Umbrella --> ValueHPP[include/minivectordb/document/value.hpp]
    Umbrella --> TypesHPP[include/minivectordb/common/types.hpp]

    SerializerHPP --> DocumentHPP
    DocumentHPP --> ValueHPP
    ValueHPP --> TypesHPP

    SerializerCPP[src/document/serializer.cpp] --> SerializerHPP
    DocumentCPP[src/document/document.cpp] --> DocumentHPP
    ValueCPP[src/document/value.cpp] --> ValueHPP
```

---

## 4. Documentation Diagram Validation

Architecture docs are not only prose; they are part of the project interface. When Mermaid diagrams include labels with punctuation or numeric prefixes, the parser can reject the entire graph even though the content is otherwise correct. The project uses explicit quoting for these labels so diagrams remain renderable and the documentation still communicates the same design intent.

---

## 5. Storage File Lifecycle

```mermaid
graph TD
    app["Caller / Storage Subsystem"] --> open["DiskManager::Open"]
    open --> ensure_dir["Ensure parent directory exists"]
    open --> create_file["Create or reopen binary database file"]
    create_file --> ready["File ready for page IO"]

    ready --> read_node["DiskManager::ReadPage"]
    ready --> write_node["DiskManager::WritePage"]
    read_node --> seekg_node["seekg(offset)"]
    write_node --> seekp_node["seekp(offset)"]
    seekg_node --> read_bytes["read PAGE_SIZE bytes"]
    seekp_node --> write_bytes["write PAGE_SIZE bytes"]
    read_bytes --> validate["Check stream state"]
    write_bytes --> validate
    validate --> sync_node["DiskManager::Sync"]
    sync_node --> flush_node["flush()"]
    flush_node --> close_node["DiskManager::Close"]
```

The storage pipeline is intentionally simple: open a file, operate on fixed-size 4096-byte pages, validate stream state after each transfer, and flush before close or durability-sensitive operations. That keeps page I/O deterministic and debuggable across the rest of the database engine.

