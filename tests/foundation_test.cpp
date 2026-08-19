#include <gtest/gtest.h>

#include <array>
#include <filesystem>

#include "minivectordb/common/types.hpp"
#include "minivectordb/document/serializer.hpp"
#include "storage/disk_manager.hpp"

// Unit tests for the foundation
TEST(FoundationTest, SanityCheck) {
    EXPECT_EQ(DB::INVALID_PAGE_ID, -1);
    EXPECT_EQ(sizeof(DB::PAGE_ID), 8U);
    EXPECT_EQ(sizeof(DB::OFFSET), 2U);
    EXPECT_EQ(1 + 1, 2);
}

// Unit tests for the document serializer
TEST(DocumentSerializerTest, RoundTrip) {
    DB::VectorType embeddings = {1.0f, -2.5f, 3.14f};
    DB::MapType fields;
    fields["name"] = DB::Value(std::string("John Doe"));
    fields["age"] = DB::Value(static_cast<int64_t>(30));
    fields["is_active"] = DB::Value(true);
    fields["score"] = DB::Value(95.5);
    fields["null_field"] = DB::Value(); // NIL type

    DB::Document doc(42, embeddings, fields);

    std::vector<uint8_t> serialized = DB::DocumentSerializer::Serialize(doc);
    DB::Document deserialized = DB::DocumentSerializer::Deserialize(serialized);

    EXPECT_EQ(deserialized.getID(), 42);
    EXPECT_EQ(deserialized.getEmbeddings(), embeddings);
    
    const auto& res_fields = deserialized.getFields();
    EXPECT_EQ(res_fields.size(), fields.size());
    EXPECT_EQ(res_fields.at("name").As<std::string>(), "John Doe");
    EXPECT_EQ(res_fields.at("age").As<int64_t>(), 30);
    EXPECT_EQ(res_fields.at("is_active").As<bool>(), true);
    EXPECT_EQ(res_fields.at("score").As<double>(), 95.5);
    EXPECT_EQ(res_fields.at("null_field").getType(), DB::Type::NIL);
}

TEST(DocumentSerializerTest, Determinism) {
    DB::VectorType embeddings = {0.1f, -0.2f, 0.3f, -0.4f};
    DB::MapType fields;
    fields["k1"] = DB::Value(std::string("v1"));
    fields["k2"] = DB::Value(static_cast<int64_t>(100));

    DB::Document doc(999, embeddings, fields);

    std::vector<uint8_t> s1 = DB::DocumentSerializer::Serialize(doc);
    std::vector<uint8_t> s2 = DB::DocumentSerializer::Serialize(doc);

    EXPECT_EQ(s1, s2);
}

TEST(DocumentSerializerTest, BufferSafetyOverflow) {
    DB::VectorType embeddings = {1.0f, 2.0f};
    DB::MapType fields;
    fields["key"] = DB::Value(std::string("value"));

    DB::Document doc(123, embeddings, fields);
    std::vector<uint8_t> serialized = DB::DocumentSerializer::Serialize(doc);

    // Try deserializing truncated versions of the serialized buffer
    for (size_t i = 0; i < serialized.size(); ++i) {
        std::vector<uint8_t> truncated(serialized.begin(), serialized.begin() + i);
        EXPECT_THROW(DB::DocumentSerializer::Deserialize(truncated), std::runtime_error);
    }
}

TEST(DocumentSerializerTest, EmptyDocRoundTrip) {
    DB::Document doc(777); // Empty embeddings and fields
    
    std::vector<uint8_t> serialized = DB::DocumentSerializer::Serialize(doc);
    DB::Document deserialized = DB::DocumentSerializer::Deserialize(serialized);

    EXPECT_EQ(deserialized.getID(), 777);
    EXPECT_TRUE(deserialized.getEmbeddings().empty());
    EXPECT_TRUE(deserialized.getFields().empty());
}

// Unit tests for the DiskManager
TEST(DiskManagerTest, PagePersistenceAcrossReopen) {
    namespace fs = std::filesystem;

    const fs::path db_path = fs::temp_directory_path() / "mini_vector_db_persistence_test.bin";
    fs::remove(db_path);

    {
        DB::DiskManager manager;
        manager.Open(db_path);

        std::array<char, DB::PAGE_SIZE> write_buffer{};
        for (size_t i = 0; i < write_buffer.size(); ++i) {
            write_buffer[i] = static_cast<char>((i * 17) % 251);
        }

        manager.WritePage(3, write_buffer.data());
        manager.Sync();
        manager.Close();

        DB::DiskManager reopened_manager;
        reopened_manager.Open(db_path);

        std::array<char, DB::PAGE_SIZE> read_buffer{};
        reopened_manager.ReadPage(3, read_buffer.data());
        reopened_manager.Close();

        EXPECT_EQ(read_buffer, write_buffer);
    }

    fs::remove(db_path);
}
