#include "actions.hpp"
#include <iostream>
#include <stdexcept>
#include <random>
#include <algorithm>

namespace elizaos {

std::string formatActionNames(const std::vector<Action>& actions) {
    // NOTE: Auto-converted from TypeScript - manually refined for C++

    if (actions.empty()) return "";

    // Create a shuffled copy instead of mutating the original array
    std::vector<Action> shuffledActions = actions;
    
    // Shuffle the actions
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(shuffledActions.begin(), shuffledActions.end(), gen);
    
    // Build comma-separated string of action names
    std::string result;
    for (size_t i = 0; i < shuffledActions.size(); ++i) {
        result += shuffledActions[i].name;
        if (i < shuffledActions.size() - 1) {
            result += ", ";
        }
    }
    
    return result;
}

std::string formatActions(const std::vector<Action>& actions) {
    // NOTE: Auto-converted from TypeScript - manually refined for C++

    if (actions.empty()) return "";

    // Create a shuffled copy instead of mutating the original array
    std::vector<Action> shuffledActions = actions;
    
    // Shuffle the actions
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(shuffledActions.begin(), shuffledActions.end(), gen);
    
    // Build formatted string with action names and descriptions
    std::string result;
    for (size_t i = 0; i < shuffledActions.size(); ++i) {
        result += shuffledActions[i].name + ": " + shuffledActions[i].description;
        if (i < shuffledActions.size() - 1) {
            result += ",\n";
        }
    }
    
    return result;
}

} // namespace elizaos
