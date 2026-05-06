#include "use-authentication.hpp"
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



struct User {
    std::string address;
    double points;
    std::optional<double> solBalance;
};

void useUser();

} // namespace elizaos
