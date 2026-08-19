#pragma once

#include <cstdint>

namespace DB {

using PAGE_ID = std::int64_t;
using TXN_ID = std::int64_t;
using FRAME_ID = std::int64_t;
using OFFSET = std::int16_t;
using LSN = std::int64_t;

inline constexpr PAGE_ID INVALID_PAGE_ID = -1;
inline constexpr TXN_ID INVALID_TXN_ID = -1;
inline constexpr FRAME_ID INVALID_FRAME_ID = -1;
inline constexpr OFFSET INVALID_OFFSET = -1;
inline constexpr LSN INVALID_LSN = -1;

constexpr size_t PAGE_SIZE = 4096;

}  // namespace DB
