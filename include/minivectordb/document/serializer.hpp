#pragma once

#include<cstdint>
#include<string>
#include<vector>
#include "minivectordb/common/types.hpp"
#include "minivectordb/document/document.hpp"

namespace DB{

    class DocumentSerializer{
    public:
        static std::vector<uint8_t> Serialize(const Document& doc);
        static Document Deserialize(const std::vector<uint8_t>& bytes);
    };
}