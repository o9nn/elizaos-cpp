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



struct ExaSearchResponse {
    std::string title;
    std::string url;
    std::string snippet;
    double score;
};

class ExaSearchPlugin {
public:
    ExaSearchPlugin(ExaPluginConfig config);
    void if(auto !response.ok);
    void catch(auto error);

private:
    ExaPluginConfig config_;
};


} // namespace elizaos
