#include <array>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <iomanip>
#include "minivectordb.hpp"

namespace fs = std::filesystem;

void PrintDocument(const DB::Document& doc) {
    std::cout << "Document ID: " << doc.getID() << "\n";
    std::cout << "Embeddings: [";
    const auto& embed = doc.getEmbeddings();
    for (size_t i = 0; i < embed.size(); ++i) {
        std::cout << embed[i] << (i + 1 < embed.size() ? ", " : "");
    }
    std::cout << "]\n";
    std::cout << "Metadata Fields:\n";
    for (const auto& [key, val] : doc.getFields()) {
        std::cout << "  - " << key << ": ";
        switch (val.getType()) {
            case DB::Type::NIL:
                std::cout << "NIL\n";
                break;
            case DB::Type::BOOL:
                std::cout << (val.As<bool>() ? "true" : "false") << " (BOOL)\n";
                break;
            case DB::Type::INT64:
                std::cout << val.As<int64_t>() << " (INT64)\n";
                break;
            case DB::Type::DOUBLE:
                std::cout << val.As<double>() << " (DOUBLE)\n";
                break;
            case DB::Type::STRING:
                std::cout << "\"" << val.As<std::string>() << "\" (STRING)\n";
                break;
            default:
                std::cout << "Unknown type\n";
                break;
        }
    }
}

int main() {
    std::cout << "--- Creating Document ---\n";
    DB::VectorType embeddings = {0.15f, -0.82f, 1.34f, -0.09f};
    DB::MapType fields;
    fields["title"] = DB::Value(std::string("Antigravity Database"));
    fields["version"] = DB::Value(static_cast<int64_t>(1));
    fields["is_experimental"] = DB::Value(true);
    fields["threshold"] = DB::Value(0.985);
    fields["empty_val"] = DB::Value();

    DB::Document original_doc(101, embeddings, fields);
    PrintDocument(original_doc);

    std::cout << "\n--- Serializing Document ---\n";
    std::vector<uint8_t> serialized = DB::DocumentSerializer::Serialize(original_doc);
    std::cout << "Serialized Size: " << serialized.size() << " bytes\n";
    std::cout << "Byte Content (Hex):\n";
    for (size_t i = 0; i < serialized.size(); ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(serialized[i]) << " ";
        if ((i + 1) % 16 == 0) std::cout << "\n";
    }
    std::cout << std::dec << "\n";

    std::cout << "\n--- Deserializing Document ---\n";
    DB::Document reconstructed_doc = DB::DocumentSerializer::Deserialize(serialized);
    PrintDocument(reconstructed_doc);

    std::cout << "\n--- Verification ---\n";
    if (original_doc.getID() == reconstructed_doc.getID() &&
        original_doc.getEmbeddings() == reconstructed_doc.getEmbeddings() &&
        original_doc.getFields().size() == reconstructed_doc.getFields().size()) {
        std::cout << "SUCCESS: Document round-trip matches exactly!\n";
    } else {
        std::cout << "FAILURE: Reconstructed document does not match the original!\n";
    }

    // Write the actual serialized document into a cleared database.bin file.
    std::cout << "\n--- Document Persistence to File ---\n";

    const fs::path db_path = fs::path("data") / "database.db";
    std::error_code ec;
    fs::create_directories(db_path.parent_path(), ec);
    if (ec) {
        std::cerr << "Failed to create data directory: " << ec.message() << "\n";
        return 1;
    }

    // Clear any old content before writing the new document.
    fs::remove(db_path);

    std::array<char, DB::PAGE_SIZE> page_buffer{};
    std::array<char, DB::PAGE_SIZE> read_back{};

    std::memcpy(page_buffer.data(), serialized.data(), serialized.size());

    try {
        DB::DiskManager disk_manager;
        disk_manager.Open(db_path);
        disk_manager.WritePage(0, page_buffer.data());
        disk_manager.Sync();
        disk_manager.Close();

        DB::DiskManager reopened_manager;
        reopened_manager.Open(db_path);
        reopened_manager.ReadPage(0, read_back.data());
        reopened_manager.Close();

        const bool success = (std::memcmp(page_buffer.data(), read_back.data(), DB::PAGE_SIZE) == 0);
        std::cout << "Document persistence: " << (success ? "SUCCESS" : "FAILURE") << "\n";
        if (!success) {
            std::cerr << "Persisted document does not match the bytes written.\n";
            return 2;
        }

        std::cout << "\n--- Hex Dump of database.bin (page 0) ---\n";
        for (size_t i = 0; i < serialized.size(); ++i) {
            std::cout << std::hex << std::setw(2) << std::setfill('0')
                      << static_cast<unsigned int>(static_cast<unsigned char>(read_back[i])) << ' ';
            if ((i + 1) % 16 == 0) {
                std::cout << '\n';
            }
        }
        std::cout << std::dec << '\n';
    } catch (const std::exception& ex) {
        std::cerr << "Document persistence failed: " << ex.what() << "\n";
        return 3;
    }

    std::cout << "Database file cleared and the serialized document was written to: " << db_path << "\n";
    return 0;
}
