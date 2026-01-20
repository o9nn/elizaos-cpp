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



struct RelatedProjectsProps {
    Project currentProject;
    (project: Project) => void onProjectClick;
};

struct CachedProjects {
    std::vector<Project> projects;
    double timestamp;
};

void RelatedProjects(auto { currentProject, RelatedProjectsProps onProjectClick }); 
} // namespace elizaos
