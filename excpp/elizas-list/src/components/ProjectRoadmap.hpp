#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct RoadmapItem {
    std::string id;
    std::string title;
    std::string description;
    std::variant<'planned', 'in-progress', 'completed'> status;
    std::string date;
    std::string name;
    std::string avatar;
};

struct RoadmapProps {
    std::vector<RoadmapItem> items;
};

void ProjectRoadmap(RoadmapProps { items }); 
} // namespace elizaos
