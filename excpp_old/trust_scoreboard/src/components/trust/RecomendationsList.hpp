#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "RecomendationsList.module.css.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct Transaction {
    double id;
    std::string token;
    std::variant<'buy', 'sell'> type;
    double price;
    double performance;
    std::string date;
    std::variant<'success', 'pending', 'failed'> status;
};

  // Add more mock transactions as needed


} // namespace elizaos
