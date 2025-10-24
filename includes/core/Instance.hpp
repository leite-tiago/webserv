/**
 * Instance.hpp
 * Singleton pattern implementation for managing global instances
 */
#pragma once

#include <map>
#include <cstddef>
#include <typeinfo>

class Instance {
private:
    static std::map<std::size_t, void*> instances_;

public:
    /**
     * Get singleton instance of type T
     * Creates instance if it doesn't exist
     * @return: Pointer to singleton instance of type T
     */
    template<typename T>
    static T* Get() {
        std::size_t type_hash = reinterpret_cast<std::size_t>(&typeid(T));

        std::map<std::size_t, void*>::iterator it = instances_.find(type_hash);
        if (it != instances_.end()) {
            return static_cast<T*>(it->second);
        }

        // Create new instance
        T* instance = new T();
        instances_[type_hash] = instance;
        return instance;
    }
};
