#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Helper function to send auth token in headers

  return fetch(url, newOptions);

struct AuthStatus {
    bool authenticated;
    std::optional<std::vector<std::string>> privileges;
    std::string address;
    double points;
    std::optional<double> solBalance;
};


} // namespace elizaos
