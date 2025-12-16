#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".utils/log.hpp"
#include "runtime.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Deployment abstractions
 * Handles Docker and other deployment strategies
 */

// Re-runtime types for convenience

/**
 * Abstract deployment interface
 */

/**
 * Docker deployment configuration
 */

using DockerDeploymentConfig = z::infer<typeof DockerDeploymentConfigSchema>;

// Type aliases for Docker types
using DockerContainer = std::any; // Docker.Container type not exported properly
using DockerExec = std::any; // Docker.Exec type not exported properly

/**
 * Docker runtime implementation
 */
class DockerRuntime {
public:
    DockerRuntime(Docker _docker);
    void setContainer(DockerContainer container);
    std::future<void> createSession(CreateBashSessionRequest request);
    std::future<BashActionResult> runInSession(const std::variant<BashAction, BashInterruptAction>& action);
    std::future<CommandResult> execute(Command command);
    std::future<ReadFileResponse> readFile(ReadFileRequest request);
    std::future<void> writeFile(WriteFileRequest request);
    std::future<void> upload(UploadRequest request);
};

/**
 * Docker-based deployment
 */
class DockerDeployment {
public:
    DockerDeployment(DockerDeploymentConfig config);
    std::future<void> start();
    std::future<void> stop();

private:
    Docker docker_;
    DockerDeploymentConfig config_;
    DockerRuntime runtime_;
};

/**
 * Union type for all deployment configurations
 */
  // Add other deployment types here (e.g., local, kubernetes, etc.)

using DeploymentConfig = z::infer<typeof DeploymentConfigSchema>;

/**
 * Factory function to create deployment from config
 */
AbstractDeployment getDeployment(DeploymentConfig config);

} // namespace elizaos
