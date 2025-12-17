#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "use-authentication.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct User {
    std::string address;
    double points;
    std::optional<double> solBalance;
};

void useUser();

} // namespace elizaos
