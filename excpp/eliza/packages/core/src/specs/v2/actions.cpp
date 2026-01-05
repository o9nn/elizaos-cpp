#include "actions.hpp"
#include <iostream>
#include <stdexcept>
#include <sstream>

namespace elizaos {

std::string formatActionNames(const std::vector<Action>& actions) {
    // NOTE: Auto-converted from TypeScript - manually refined for C++
    
    if (actions.empty()) {
        return "";
    }
    
    std::ostringstream oss;
    for (size_t i = 0; i < actions.size(); ++i) {
        oss << actions[i].name;
        if (i < actions.size() - 1) {
            oss << ", ";
        }
    }
    
    return oss.str();
}

std::string formatActions(const std::vector<Action>& actions) {
    // NOTE: Auto-converted from TypeScript - manually refined for C++
    
    if (actions.empty()) {
        return "";
    }
    
    std::ostringstream oss;
    for (size_t i = 0; i < actions.size(); ++i) {
        oss << actions[i].name << ": " << actions[i].description;
        if (i < actions.size() - 1) {
            oss << "\n";
        }
    }
    
    return oss.str();
}

} // namespace elizaos
