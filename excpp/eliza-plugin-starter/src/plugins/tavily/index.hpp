#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "..common/types.ts.hpp"
#include "..common/utils.ts.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct TavilySearchResponse {
    std::string title;
    std::string url;
    std::string content;
};

class TavilySearchPlugin {
public:
    TavilySearchPlugin(TavilyPluginConfig config);

private:
    TavilyPluginConfig config_;
};


} // namespace elizaos
