#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct WalletAddressBadgeProps {
    std::string address;
    ReactNode icon;
    std::string label;
    std::optional<std::string> className;
};

      // You could add a user-facing error message here if desired


} // namespace elizaos
