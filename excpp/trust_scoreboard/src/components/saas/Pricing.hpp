#include "GetStarted.hpp"
#include "Pricing.module.css.hpp"
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



struct PricingTier {
    std::string name;
    double price;
    std::string description;
    std::vector<std::string> features;
    std::string requestLimit;
    std::optional<bool> highlighted;
};


} // namespace elizaos
