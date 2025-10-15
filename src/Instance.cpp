/**
 * Instance.cpp
 * Implementation of Instance singleton pattern
 */
#include "includes/Instance.hpp"

// Static member definition
std::map<std::size_t, void*> Instance::instances_;
