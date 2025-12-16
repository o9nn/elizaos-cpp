#include ".utils/axios.hpp"
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



struct Partner {
    double trustScore;
    std::optional<std::string> avatarUrl;
    std::optional<double> rank;
    std::optional<std::string> id;
    std::optional<std::string> name;
};

struct DashboardData {
    std::vector<Partner> partners;
};


} // namespace elizaos
