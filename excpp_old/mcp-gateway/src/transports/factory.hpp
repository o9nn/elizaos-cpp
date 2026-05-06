#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".types/index.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class TransportFactory {
public:
    Transport create(McpServerConfig config);
    TransportConfig normalizeTransportConfig(McpServerConfig config);
    std::vector<std::string> validateConfig(McpServerConfig config);
    std::string getTransportDisplayName(McpServerConfig config);
};


} // namespace elizaos
