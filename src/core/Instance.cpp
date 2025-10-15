/**
 * Instance.cpp
 * Implementation of Instance singleton pattern
 */
#include "includes/core/Instance.hpp"

// Static member definition
std::map<std::size_t, void*> Instance::instances_;
