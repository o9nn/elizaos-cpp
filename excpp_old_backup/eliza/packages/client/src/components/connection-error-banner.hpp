#include ".context/ConnectionContext.hpp"
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



struct ConnectionErrorBannerProps {
    std::optional<std::string> className;
};

void ConnectionErrorBanner(ConnectionErrorBannerProps { className });

} // namespace elizaos
