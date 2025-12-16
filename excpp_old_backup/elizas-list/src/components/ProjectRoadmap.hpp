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



struct RoadmapItem {
    std::string id;
    std::string title;
    std::string description;
    'planned' | 'in-progress' | 'completed' status;
    std::string date;
    std::optional<{> assignee;
    std::string name;
    std::string avatar;
};

struct RoadmapProps {
    std::vector<RoadmapItem> items;
};

void ProjectRoadmap(RoadmapProps { items }); 
} // namespace elizaos
