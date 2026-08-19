#pragma once

#include "minivectordb/document/value.hpp"  

namespace DB{

    class Document{
        private:
        uint64_t id_;
        VectorType embeddings_;
        MapType fields_;
    
        public:
        Document();
        explicit Document(uint64_t id);
        explicit Document(uint64_t id, VectorType embeddings, MapType fields);
        
        uint64_t getID() const { return id_; }
        const VectorType& getEmbeddings() const { return embeddings_; }
        const MapType& getFields() const { return fields_;}

        void setEmbeddings(VectorType embeddings);
        void setFields(MapType fields);

    };

}