#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "sessions-client.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Workflow Utilities for Defensive Error Handling
 * 
 * This module provides utilities for GitHub Actions workflows to handle
 * common failure scenarios gracefully, particularly for session logging
 * and external API integrations.
 * 
 * Fixes GitHub Issue #27: https://github.com/ZoneCog/elizaos-cpp/issues/27
 */

/**
 * Environment configuration for workflow error handling
 */
struct WorkflowConfig {
    std::optional<std::string> sessionLogsEndpoint;
    std::optional<bool> enableSessionLogging;
    std::optional<bool> failOnSessionLogError;
    std::optional<double> retryCount;
    std::optional<double> timeoutMs;
};

/**
 * Workflow context information
 */
struct WorkflowContext {
    std::optional<std::string> workflowName;
    std::optional<std::string> runId;
    std::optional<std::string> actor;
    std::optional<std::string> repository;
    std::optional<std::string> event;
    std::optional<std::string> ref;
};

/**
 * Main utility class for defensive workflow operations
 */
class WorkflowUtils {
public:
    WorkflowUtils(WorkflowConfig config = {});
    WorkflowContext extractWorkflowContext();
    std::future<bool> uploadSessionLogs(const std::vector<std::any>& logs, std::optional<std::string> sessionId, std::optional<std::unordered_map<std::string, std::any>> metadata);
    std::future<bool> checkSessionLogHealth();
    void setOutput(const std::string& name, const std::string& value);
    void setEnvVar(const std::string& name, const std::string& value);
    void logWarning(const std::string& message, std::optional<std::string> file, std::optional<double> line);
    void logError(const std::string& message, std::optional<std::string> file, std::optional<double> line);
    std::future<void> exitGracefully(double exitCode = 0, std::optional<std::vector<std::any>> finalLogs, std::optional<std::string> reason);

/**
 * Factory function to create a WorkflowUtils instance with environment-based configuration
 */
WorkflowUtils createWorkflowUtils();

// Export types and classes

// Default default WorkflowUtils;
} // namespace elizaos
