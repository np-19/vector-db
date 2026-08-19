#include<utility>
#include "minivectordb/document/document.hpp"


namespace DB{
    Document::Document() : id_(0) {}
    Document::Document(uint64_t id) : id_(id) {}

    Document::Document(uint64_t id, VectorType embeddings, MapType fields)
        : id_(id),
        embeddings_(std::move(embeddings)),
        fields_(std::move(fields)) {}



    void Document::setEmbeddings(VectorType embeddings) {
        embeddings_ = std::move(embeddings);
    }

    void Document::setFields(MapType fields) {
        fields_ = std::move(fields);
    }
}