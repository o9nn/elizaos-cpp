#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
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
    ProjectStatusManager(IAgentRuntime protected runtime);
    std::string capabilityDescription() const;
    std::future<Service> start(IAgentRuntime runtime);
    std::future<void> start();
    std::future<void> stop();
    std::string createProject(const std::string& name, ProjectType type);
    void updateStatus(const std::string& projectId, const std::optional<ProjectStatusUpdate>& updates);
    void updateStep(const std::string& projectId, const std::string& step, std::optional<std::string> message);
    void updateFileStatus(const std::string& projectId, const std::string& filePath, const std::variant<'pending', 'generating', 'complete', 'error'>& status);
    void updateValidation(const std::string& projectId, const std::variant<'lint', 'typeCheck', 'tests', 'build'>& type, bool passed, std::optional<std::vector<std::string>> errors);
    ProjectStatusUpdate getProject(const std::string& projectId);
    std::vector<ProjectStatusUpdate> getActiveProjects();
    std::vector<ProjectHistory> getHistory(number = 50 limit);
    void moveToHistory(ProjectStatusUpdate project);
    void broadcastUpdate(ProjectStatusUpdate project);
     getStatusSummary();
};


} // namespace elizaos
