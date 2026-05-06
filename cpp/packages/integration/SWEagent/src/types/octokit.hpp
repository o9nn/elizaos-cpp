#pragma once
#include <algorithm>
#include <any>
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

  class Octokit {
public:
    Promise< get(const std::any& params);
    Promise< create(const std::unordered_map<std::string, std::any>& params);
};


} // namespace elizaos
