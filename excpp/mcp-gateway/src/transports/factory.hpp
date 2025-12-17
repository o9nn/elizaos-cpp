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
#include "types/index.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class TransportFactory {
public:
    static Transport create(McpServerConfig config);
    static TransportConfig normalizeTransportConfig(McpServerConfig config);
    static std::vector<std::string> validateConfig(McpServerConfig config);
    static std::string getTransportDisplayName(McpServerConfig config);
};


} // namespace elizaos
