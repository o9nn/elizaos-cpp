#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "prisma.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class Analytics {
public:
    void trackProjectView(const std::string& projectId, std::optional<std::string> userId);
    void trackRelatedProjectClick(const std::string& sourceProjectId, const std::string& targetProjectId, std::optional<std::string> userId);
};
 
} // namespace elizaos
