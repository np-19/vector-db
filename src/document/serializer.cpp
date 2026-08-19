#include "minivectordb/document/serializer.hpp"
#include <stdexcept>
#include <cstring>

namespace DB {

namespace {

//function to write uint8 to buffer in little endian 
inline void WriteUint8(std::vector<uint8_t>& buf, uint8_t val) {
    buf.push_back(val);
}

//function to write uint16 to buffer in little endian 
inline void WriteUint16(std::vector<uint8_t>& buf, uint16_t val) {
    buf.push_back(static_cast<uint8_t>(val & 0xFF));
    buf.push_back(static_cast<uint8_t>((val >> 8) & 0xFF));
}

//function to write uint32 to buffer in little endian 
inline void WriteUint32(std::vector<uint8_t>& buf, uint32_t val) {
    buf.push_back(static_cast<uint8_t>(val & 0xFF));
    buf.push_back(static_cast<uint8_t>((val >> 8) & 0xFF));
    buf.push_back(static_cast<uint8_t>((val >> 16) & 0xFF));
    buf.push_back(static_cast<uint8_t>((val >> 24) & 0xFF));
}

//function to write uint64 to buffer in little endian 
inline void WriteUint64(std::vector<uint8_t>& buf, uint64_t val) {
    for (int i = 0; i < 8; ++i) {
        buf.push_back(static_cast<uint8_t>((val >> (i * 8)) & 0xFF));
    }
}

//function to write float to buffer in little endian 
inline void WriteFloat(std::vector<uint8_t>& buf, float val) {
    uint32_t bytes;
    std::memcpy(&bytes, &val, sizeof(float));
    WriteUint32(buf, bytes);
}

inline void WriteDouble(std::vector<uint8_t>& buf, double val) {
    uint64_t bytes;
    std::memcpy(&bytes, &val, sizeof(double));
    WriteUint64(buf, bytes);
}

inline void CheckBounds(size_t cursor, size_t size, size_t needed) {
    if (cursor + needed > size) {
        throw std::runtime_error("Deserialization error: buffer overflow");
    }
}

inline uint8_t ReadUint8(const std::vector<uint8_t>& buf, size_t& cursor) {
    CheckBounds(cursor, buf.size(), 1);
    uint8_t val = buf[cursor];
    cursor += 1;
    return val;
}

inline uint16_t ReadUint16(const std::vector<uint8_t>& buf, size_t& cursor) {
    CheckBounds(cursor, buf.size(), 2);
    uint16_t val = buf[cursor] | (static_cast<uint16_t>(buf[cursor + 1]) << 8);
    cursor += 2;
    return val;
}

inline uint32_t ReadUint32(const std::vector<uint8_t>& buf, size_t& cursor) {
    CheckBounds(cursor, buf.size(), 4);
    uint32_t val = buf[cursor] |
                   (static_cast<uint32_t>(buf[cursor + 1]) << 8) |
                   (static_cast<uint32_t>(buf[cursor + 2]) << 16) |
                   (static_cast<uint32_t>(buf[cursor + 3]) << 24);
    cursor += 4;
    return val;
}

inline uint64_t ReadUint64(const std::vector<uint8_t>& buf, size_t& cursor) {
    CheckBounds(cursor, buf.size(), 8);
    uint64_t val = 0;
    for (int i = 0; i < 8; ++i) {
        val |= (static_cast<uint64_t>(buf[cursor + i]) << (i * 8));
    }
    cursor += 8;
    return val;
}

inline float ReadFloat(const std::vector<uint8_t>& buf, size_t& cursor) {
    uint32_t bytes = ReadUint32(buf, cursor);
    float val;
    std::memcpy(&val, &bytes, sizeof(float));
    return val;
}

inline double ReadDouble(const std::vector<uint8_t>& buf, size_t& cursor) {
    uint64_t bytes = ReadUint64(buf, cursor);
    double val;
    std::memcpy(&val, &bytes, sizeof(double));
    return val;
}

} // namespace

std::vector<uint8_t> DocumentSerializer::Serialize(const Document& doc) {
    std::vector<uint8_t> buf;

    // Header:
    // - Document ID (8 bytes)
    // - Embedding Dimensions (4 bytes)
    // - Metadata Dictionary count (2 bytes)
    WriteUint64(buf, doc.getID());
    
    const auto& embeddings = doc.getEmbeddings();
    WriteUint32(buf, static_cast<uint32_t>(embeddings.size()));

    const auto& fields = doc.getFields();
    if (fields.size() > 65535) {
        throw std::runtime_error("Too many metadata fields");
    }
    WriteUint16(buf, static_cast<uint16_t>(fields.size()));

    // Vector Embedding Segment
    for (float f : embeddings) {
        WriteFloat(buf, f);
    }

    // Metadata Fields Segment
    for (const auto& [key, val] : fields) {
        if (key.size() > 65535) {
            throw std::runtime_error("Metadata key length exceeds 65535 characters");
        }
        WriteUint16(buf, static_cast<uint16_t>(key.size()));
        for (char c : key) {
            WriteUint8(buf, static_cast<uint8_t>(c));
        }

        Type val_type = val.getType();
        WriteUint8(buf, static_cast<uint8_t>(val_type));

        switch (val_type) {
            case Type::NIL:
                break;
            case Type::BOOL:
                WriteUint8(buf, val.As<bool>() ? 1 : 0);
                break;
            case Type::INT64:
                WriteUint64(buf, static_cast<uint64_t>(val.As<int64_t>()));
                break;
            case Type::DOUBLE:
                WriteDouble(buf, val.As<double>());
                break;
            case Type::STRING: {
                const std::string& str = val.As<std::string>();
                if (str.size() > 65535) {
                    throw std::runtime_error("Metadata string value length exceeds 65535 characters");
                }
                WriteUint16(buf, static_cast<uint16_t>(str.size()));
                for (char c : str) {
                    WriteUint8(buf, static_cast<uint8_t>(c));
                }
                break;
            }
            default:
                throw std::runtime_error("Unsupported metadata value type for serialization");
        }
    }

    return buf;
}

Document DocumentSerializer::Deserialize(const std::vector<uint8_t>& bytes) {
    size_t cursor = 0;

    // Read header
    uint64_t id = ReadUint64(bytes, cursor);
    uint32_t dimensions = ReadUint32(bytes, cursor);
    uint16_t fields_count = ReadUint16(bytes, cursor);

    // Read embeddings
    VectorType embeddings;
    embeddings.reserve(dimensions);
    for (uint32_t i = 0; i < dimensions; ++i) {
        embeddings.push_back(ReadFloat(bytes, cursor));
    }

    // Read metadata
    MapType fields;
    for (uint16_t i = 0; i < fields_count; ++i) {
        uint16_t key_len = ReadUint16(bytes, cursor);
        CheckBounds(cursor, bytes.size(), key_len);
        std::string key(reinterpret_cast<const char*>(&bytes[cursor]), key_len);
        cursor += key_len;

        uint8_t type_byte = ReadUint8(bytes, cursor);
        Type val_type = static_cast<Type>(type_byte);

        Value val;
        switch (val_type) {
            case Type::NIL:
                val = Value();
                break;
            case Type::BOOL: {
                uint8_t b = ReadUint8(bytes, cursor);
                val = Value(b != 0);
                break;
            }
            case Type::INT64: {
                uint64_t val_i64 = ReadUint64(bytes, cursor);
                val = Value(static_cast<int64_t>(val_i64));
                break;
            }
            case Type::DOUBLE: {
                double val_d = ReadDouble(bytes, cursor);
                val = Value(val_d);
                break;
            }
            case Type::STRING: {
                uint16_t str_len = ReadUint16(bytes, cursor);
                CheckBounds(cursor, bytes.size(), str_len);
                std::string str(reinterpret_cast<const char*>(&bytes[cursor]), str_len);
                cursor += str_len;
                val = Value(str);
                break;
            }
            default:
                throw std::runtime_error("Unsupported metadata value type during deserialization");
        }

        fields[key] = val;
    }

    // Return the Document
    return Document(id, std::move(embeddings), std::move(fields));
}

} // namespace DB
