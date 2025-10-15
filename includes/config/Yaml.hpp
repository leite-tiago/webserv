/**
 * Yaml.hpp
 * Implementação básica de YAML.
 * Implementa Nodes e parsing de arquivos.
 * Suporta escalares, nulos, sequências e mapas.
 * Baseado na especificação YAML 1.2.
 * Design da classe baseado em https://github.com/jbeder/yaml-cpp
 * Possui alguns testes básicos para garantir que nenhuma funcionalidade esteja quebrada.
 */
#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstdlib>

namespace YAML {
    // Forward declarations
    class Node;

    // YAML Types enumeration
    namespace Types {
        enum Value {
            Null,
            Scalar,
            Sequence,
            Map
        };
    }

    /**
     * Basic YAML Node implementation
     */
    class Node {
    private:
        Types::Value type_;
        std::string scalar_value_;
        std::vector<Node> sequence_;
        std::map<std::string, Node> map_;
        bool valid_;

    public:
        // Constructors
        Node() : type_(Types::Null), valid_(true) {}
        Node(const std::string& value) : type_(Types::Scalar), scalar_value_(value), valid_(true) {}

        // Type checking methods
        bool isValid() const { return valid_; }
        bool isNull() const { return type_ == Types::Null; }
        bool isScalar() const { return type_ == Types::Scalar; }
        bool isSequence() const { return type_ == Types::Sequence; }
        bool isMap() const { return type_ == Types::Map; }

        template<Types::Value T>
        bool is() const {
            return type_ == T;
        }

        // Access methods
        template<typename T>
        T as() const {
            if (!valid_) {
                throw std::runtime_error("Invalid YAML node");
            }
            // This is a simplified implementation
            // In a real implementation, you'd need proper type conversion
            return T();
        }

        // Specializations for specific types
        std::string asString() const {
            if (!valid_) {
                throw std::runtime_error("Invalid YAML node");
            }
            return scalar_value_;
        }

        int asInt() const {
            if (!valid_) {
                throw std::runtime_error("Invalid YAML node");
            }
            return atoi(scalar_value_.c_str());
        }

        bool asBool() const {
            if (!valid_) {
                throw std::runtime_error("Invalid YAML node");
            }
            return scalar_value_ == "true" || scalar_value_ == "yes" || scalar_value_ == "1";
        }

        // Map access
        const Node& operator[](const std::string& key) const {
            static Node invalid_node;
            invalid_node.valid_ = false;

            if (type_ != Types::Map) {
                return invalid_node;
            }

            std::map<std::string, Node>::const_iterator it = map_.find(key);
            if (it != map_.end()) {
                return it->second;
            }
            return invalid_node;
        }

        Node& operator[](const std::string& key) {
            if (type_ == Types::Null) {
                type_ = Types::Map;
            }
            if (type_ == Types::Map) {
                return map_[key];
            }
            throw std::runtime_error("Node is not a map");
        }

        // Sequence access
        const Node& operator[](size_t index) const {
            static Node invalid_node;
            invalid_node.valid_ = false;

            if (type_ != Types::Sequence || index >= sequence_.size()) {
                return invalid_node;
            }
            return sequence_[index];
        }

        // Check if key/index exists
        bool has(const std::string& key) const {
            return type_ == Types::Map && map_.find(key) != map_.end();
        }

        bool has(size_t index) const {
            return type_ == Types::Sequence && index < sequence_.size();
        }

        // Get methods (alternative to operator[])
        const Node& get(const std::string& key) const {
            return (*this)[key];
        }

        const Node& get(size_t index) const {
            return (*this)[index];
        }

        // Size methods
        size_t size() const {
            if (type_ == Types::Sequence) return sequence_.size();
            if (type_ == Types::Map) return map_.size();
            return 0;
        }
    };

    // Function declarations (implementations in Yaml.cpp)
    Node LoadFile(const std::string& filename);
    void RunTests();

    // Template specializations for Node::as method
    template<>
    inline std::string Node::as<std::string>() const {
        return asString();
    }

    template<>
    inline int Node::as<int>() const {
        return asInt();
    }

    template<>
    inline bool Node::as<bool>() const {
        return asBool();
    }
}



