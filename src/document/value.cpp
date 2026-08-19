#include "minivectordb/document/value.hpp"


namespace DB{
    Type Value::getType() const{
    if(std::holds_alternative<std::monostate>(data_)) return Type::NIL;
    if(std::holds_alternative<bool>(data_)) return Type::BOOL;
    if(std::holds_alternative<int64_t>(data_)) return Type::INT64;
    if(std::holds_alternative<double>(data_)) return Type::DOUBLE;
    if(std::holds_alternative<std::string>(data_)) return Type::STRING;
    if(std::holds_alternative<VectorType>(data_)) return Type::VECTOR;
    if(std::holds_alternative<ArrayType>(data_)) return Type::ARRAY;
    if(std::holds_alternative<MapType>(data_)) return Type::MAP;
    return Type::NIL;
}

}