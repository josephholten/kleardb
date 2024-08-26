#ifndef SCHEMA_HPP
#define SCHEMA_HPP

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <optional>
#include <cinttypes>
#include <stdexcept>
#include <fmt/core.h>

#include "serialize.hpp"

namespace kleardb {

// need to be in same order!
using SchemaVariant = std::variant<
    uint16_t,
    uint32_t,
    uint64_t,
    std::string
>;
enum class SchemaType {
    U16,
    U32,
    U64,
    String,
};

static std::map<std::string, SchemaType> ImplementationTypes = {
    {"U16", SchemaType::U16},
    {"U32", SchemaType::U32},
    {"U64", SchemaType::U64},
    {"UInt", SchemaType::U64},
    {"String", SchemaType::String},
};

static std::vector<std::string> SchemaTypeNames = {
    "U16",
    "U32",
    "U64",
    "String",
};

SchemaVariant SchemaVariantDefaultConstruct(size_t type) {
    if (type == (size_t)SchemaType::U16)
        return (uint16_t)0;
    else if (type == (size_t)SchemaType::U32)
        return (uint32_t)0;
    else if (type == (size_t)SchemaType::U64)
        return (uint64_t)0;
    else if (type == (size_t)SchemaType::String)
        return "";
    else {
        throw std::invalid_argument(
            fmt::format("Internal Error in module Schema of kleardb: index '{}' does not match any SchemaType", type)
        );
    }
}

class Schema {
public:
    static Schema ParseSchema(std::vector<std::tuple<std::string, std::string>> columns) {
        Schema schema;
        for (auto& [name, type] : columns) {
            schema.m_row_indices[name] = schema.m_types.size();
            auto it = ImplementationTypes.find(type);
            if (it == ImplementationTypes.end()) {
                throw std::invalid_argument(fmt::format("'{}' is not a valid type", type));
            } else {
                schema.m_types.push_back(it->second);
            }
        }
        return schema;
    }

    SchemaType get_type(const std::string& name) const {
        auto it = m_row_indices.find(name);
        return m_types[it->second];
    }

    SchemaType get_type(size_t index) const {
        return m_types[index];
    }

    size_t columns() const {
        return m_types.size();
    }

private:
    std::map<std::string, size_t> m_row_indices;
    std::vector<SchemaType> m_types;
};

class Row {
public:
    static Row ParseRow(Schema schema, std::vector<std::tuple<std::string, SchemaVariant>> object) {
        Row row;
        // TODO: "factory method" should verify wether the entries in the object fit the schema and parse them to the correct types
        for (auto& [name, value] : object) {
            if (value.index() != (size_t)schema.get_type(name)) { 
                std::visit([&](auto&& el){
                    throw std::invalid_argument(
                        fmt::format(
                            "element '{}' of type '{}' does not match expected type '{}'", 
                            el, 
                            SchemaTypeNames[value.index()],
                            SchemaTypeNames[(size_t)schema.get_type(name)]
                        )
                    );
                }, value);
            }
            else {
                row.m_data.push_back(value);
            }
        }
        return row;
    }

    static Row Deserialize(Schema schema, uint8_t* ptr) {
        Row row;
        for (int i = 0; i < schema.columns(); i++){
            SchemaVariant var = SchemaVariantDefaultConstruct((size_t)schema.get_type(i));

            std::visit([&](auto&& x) { // within visit x = var but with the true type (`[&]` transfers outer variables by refference)
                size_t offset = deserialize(x, ptr);
                ptr += offset;
            }, var);

            row.m_data.push_back(var);
        }

        return row;
    }

    void Serialize(uint8_t* ptr) {
        for (auto& element : m_data) {
            std::visit([&](auto&& el) { // within visit el = element but with the true type (`[&]` transfers outer variables by refference)
                size_t offset = serialize(el, ptr);
                ptr += offset;
            }, element);
        }
    }

private:
    std::vector<SchemaVariant> m_data;
};

}

#endif