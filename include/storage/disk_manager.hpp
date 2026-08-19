#pragma once

#include "minivectordb/common/types.hpp"
#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

namespace DB {

class DiskManager {
public:
  DiskManager() = default;
  ~DiskManager() {
    if (is_open) {
      Sync();
      Close();
    }
  }

  void Open(const fs::path &file_path);
  void Close();
  void ReadPage(PAGE_ID page_id, char *buffer);
  void WritePage(PAGE_ID page_id, const char *buffer);
  void Sync();

private:
  std::fstream db_file;
  fs::path file_path;
  bool is_open = false;
};
} // namespace DB
