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



struct ProjectNotificationProps {
    { project;
    std::string name;
    std::string description;
    std::string author;
    std::string url;
};

void ProjectNotification(ProjectNotificationProps { project });


} // namespace elizaos
