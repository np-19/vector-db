#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <variant>
#include <vector>
#include <stdexcept>

namespace DB {

struct Value;

using MapType = std::map<std::string, Value>;
using ArrayType = std::vector<Value>;
using VectorType = std::vector<float>;

enum class Type { NIL, BOOL, INT64, DOUBLE, STRING, VECTOR, ARRAY, MAP };

struct Value {
    std::variant<
        std::monostate,
        bool,
        std::int64_t,
        double,
        std::string,
        VectorType,
        ArrayType,
        MapType
    >
    data_;

    Value() : data_(std::monostate{}) {}
    explicit Value(bool data) : data_(data) {}
    explicit Value(std::int64_t data) : data_(data) {}
    explicit Value(double data) : data_(data) {}
    explicit Value(const std::string& data) : data_(data) {}
    explicit Value(const VectorType& data) : data_(data) {}
    explicit Value(const ArrayType& data) : data_(data) {}
    explicit Value(const MapType& value) : data_(value) {}

    Type getType() const;

    template <typename T>
    const T& As() const {
        try {
            return std::get<T>(data_);
        }
        catch (const std::bad_variant_access&) {
            throw std::runtime_error("Invalid Type Access");
        }
    }
};

}  // namespace DB
