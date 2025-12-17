#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".context/ConnectionContext.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct ConnectionErrorBannerProps {
    std::optional<std::string> className;
};

void ConnectionErrorBanner();

} // namespace elizaos
