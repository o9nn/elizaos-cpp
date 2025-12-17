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



struct RelatedProjectsProps {
    Project currentProject;
};

struct CachedProjects {
    std::vector<Project> projects;
    double timestamp;
};

void RelatedProjects(); 
} // namespace elizaos
