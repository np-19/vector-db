#include "storage/disk_manager.hpp"

#include <stdexcept>
#include <system_error>

namespace DB {

void DiskManager::Open(const fs::path &path) {
    if (path.empty()) {
        throw std::runtime_error("DiskManager: empty file path");
    }

    file_path = path;

    const fs::path parent = file_path.parent_path();
    if (!parent.empty()) {
        std::error_code ec;
        fs::create_directories(parent, ec);
        if (ec) {
            throw std::runtime_error("DiskManager: unable to create parent directory: " + ec.message());
        }
    }

    db_file.open(file_path, std::ios::in | std::ios::out | std::ios::binary);
    if (!db_file.is_open()) {
        db_file.clear();
        db_file.open(file_path, std::ios::out | std::ios::binary);
        if (!db_file.is_open()) {
            throw std::runtime_error("DiskManager: unable to create database file: " + file_path.string());
        }
        db_file.close();

        db_file.open(file_path, std::ios::in | std::ios::out | std::ios::binary);
    }

    if (!db_file.is_open()) {
        throw std::runtime_error("DiskManager: unable to open database file: " + file_path.string());
    }

    is_open = true;
}

void DiskManager::Close() {
    if (db_file.is_open()) {
        db_file.flush();
        db_file.close();
    }
    is_open = false;
}

void DiskManager::ReadPage(PAGE_ID page_id, char *buffer) {
    if (!is_open || !db_file.is_open()) {
        throw std::runtime_error("DiskManager: file is not open");
    }
    if (buffer == nullptr) {
        throw std::runtime_error("DiskManager: read buffer is null");
    }

    const auto offset = static_cast<std::streamoff>(page_id) * static_cast<std::streamoff>(PAGE_SIZE);
    db_file.seekg(offset, std::ios::beg);
    if (!db_file) {
        throw std::runtime_error("DiskManager: seek failed while reading page " + std::to_string(page_id));
    }

    db_file.read(buffer, static_cast<std::streamsize>(PAGE_SIZE));
    if (!db_file) {
        throw std::runtime_error("DiskManager: failed to read full page " + std::to_string(page_id));
    }
}

void DiskManager::WritePage(PAGE_ID page_id, const char *buffer) {
    if (!is_open || !db_file.is_open()) {
        throw std::runtime_error("DiskManager: file is not open");
    }
    if (buffer == nullptr) {
        throw std::runtime_error("DiskManager: write buffer is null");
    }

    const auto offset = static_cast<std::streamoff>(page_id) * static_cast<std::streamoff>(PAGE_SIZE);
    db_file.seekp(offset, std::ios::beg);
    if (!db_file) {
        throw std::runtime_error("DiskManager: seek failed while writing page " + std::to_string(page_id));
    }

    db_file.write(buffer, static_cast<std::streamsize>(PAGE_SIZE));
    if (!db_file) {
        throw std::runtime_error("DiskManager: failed to write full page " + std::to_string(page_id));
    }
}

void DiskManager::Sync() {
    if (!is_open || !db_file.is_open()) {
        throw std::runtime_error("DiskManager: file is not open");
    }

    db_file.flush();
    if (!db_file) {
        throw std::runtime_error("DiskManager: flush failed");
    }
}

}  // namespace DB
