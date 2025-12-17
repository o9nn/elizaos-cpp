#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "partners.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

// pages/api/leaderboard.ts

struct TokenPrice {
    std::string address;
    double usd;
};

struct TokenPrices {
    double usd;
};

struct ErrorResponse {
    std::string message;
    double statusCode;
};

std::future<double> getTokenPrice(const std::string& address);


} // namespace elizaos
