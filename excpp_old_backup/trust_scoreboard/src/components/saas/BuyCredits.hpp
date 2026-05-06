#include "BuyCredits.module.css.hpp"
#include "GetStarted.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct CreditPackage {
    std::string name;
    double credits;
    double price;
    std::string description;
    std::vector<std::string> features;
    std::optional<bool> highlighted;
};


} // namespace elizaos
