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
#include "common/types.ts.hpp"
#include "common/utils.ts.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct ExaSearchResponse {
    std::string title;
    std::string url;
    std::string snippet;
    double score;
};

class ExaSearchPlugin {
public:
    ExaSearchPlugin(ExaPluginConfig config);

private:
    ExaPluginConfig config_;
};


} // namespace elizaos
