#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Types are now imported from ../types

/**
 * Typed EventEmitter for project status events
 */
struct ProjectStatusEvents {
};

/**
 * Extended EventEmitter with typed events
 */
class TypedEventEmitter {
public:
};

/**
 * Service to manage and broadcast project status updates
 */
class ProjectStatusManager {
public:
    ProjectStatusManager();
    std::string capabilityDescription() const;
    static std::future<Service> start(IAgentRuntime runtime);
    std::future<void> stop();
    std::string createProject(const std::string& name, ProjectType type);
    void updateStatus(const std::string& projectId, const std::optional<ProjectStatusUpdate>& updates);
    void updateStep(const std::string& projectId, const std::string& step, std::optional<std::string> message);
    void updateFileStatus(const std::string& projectId, const std::string& filePath, const std::string& status);
    void updateValidation(const std::string& projectId, const std::string& type, bool passed, std::optional<std::vector<std::string>> errors);
    ProjectStatusUpdate getProject(const std::string& projectId);
    std::vector<ProjectStatusUpdate> getActiveProjects();
    std::vector<ProjectHistory> getHistory(double limit = 50);
    void moveToHistory(ProjectStatusUpdate project);
    void broadcastUpdate(ProjectStatusUpdate project);
     getStatusSummary();
};


} // namespace elizaos
