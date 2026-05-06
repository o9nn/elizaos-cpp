#pragma once

#include "sessions-client.hpp"
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

namespace elizaos {

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
    std::optional<int> retryCount;
    std::optional<int> timeoutMs;
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
 * Helper std::function to get environment variable
 */
inline std::optional<std::string> getEnvVar(const std::string& name) {
    const char* value = std::getenv(name.c_str());
    if (value) {
        return std::string(value);
    }
    return std::nullopt;
}

/**
 * Helper std::function to get environment variable with default
 */
inline std::string getEnvVarOrDefault(const std::string& name, const std::string& defaultValue) {
    auto value = getEnvVar(name);
    return value.value_or(defaultValue);
}

/**
 * Helper std::function to parse integer from environment
 */
inline int parseIntEnv(const std::string& name, int defaultValue) {
    auto value = getEnvVar(name);
    if (value.has_value()) {
        try {
            return std::stoi(value.value());
        } catch (...) {
            return defaultValue;
        }
    }
    return defaultValue;
}

/**
 * Main utility class for defensive workflow operations
 */
class WorkflowUtils {
private:
    WorkflowConfig config_;
    WorkflowContext context_;
    std::unique_ptr<SessionsClient> sessionsClient_;

public:
    /**
     * Constructor
     * @param config - Workflow configuration
     */
    explicit WorkflowUtils(const WorkflowConfig& config = {})
        : config_(config) {
        // Set defaults
        if (!config_.enableSessionLogging.has_value()) {
            config_.enableSessionLogging = true;
        }
        if (!config_.failOnSessionLogError.has_value()) {
            config_.failOnSessionLogError = false;
        }
        if (!config_.retryCount.has_value()) {
            config_.retryCount = 3;
        }
        if (!config_.timeoutMs.has_value()) {
            config_.timeoutMs = 10000;
        }

        context_ = extractWorkflowContext();
        
        SessionUploadOptions sessionOptions;
        sessionOptions.retries = config_.retryCount;
        sessionOptions.timeout = config_.timeoutMs;
        sessionOptions.failOnError = config_.failOnSessionLogError;
        
        sessionsClient_ = std::make_unique<SessionsClient>(
            config_.sessionLogsEndpoint.value_or(""),
            sessionOptions
        );
    }

    /**
     * Extract workflow context from environment variables
     */
    WorkflowContext extractWorkflowContext() {
        WorkflowContext ctx;
        ctx.workflowName = getEnvVar("GITHUB_WORKFLOW");
        ctx.runId = getEnvVar("GITHUB_RUN_ID");
        ctx.actor = getEnvVar("GITHUB_ACTOR");
        ctx.repository = getEnvVar("GITHUB_REPOSITORY");
        ctx.event = getEnvVar("GITHUB_EVENT_NAME");
        ctx.ref = getEnvVar("GITHUB_REF");
        return ctx;
    }

    /**
     * Safely upload session logs without failing the workflow
     * @param logData - The session log data
     * @return true if successful or gracefully handled
     */
    bool uploadSessionLogs(const SessionLogData& logData) {
        if (!config_.enableSessionLogging.value_or(true)) {
            return true;
        }
        return sessionsClient_->uploadLogs(logData);
    }

    /**
     * Check if the session logging service is available
     */
    bool isSessionLoggingAvailable() {
        return sessionsClient_->healthCheck();
    }

    /**
     * Set GitHub Action output safely
     * @param name - Output name
     * @param value - Output value
     */
    void setOutput(const std::string& name, const std::string& value) {
        // GitHub Actions output format
        std::cout << "::set-output name=" << name << "::" << value << std::endl;
    }

    /**
     * Set GitHub Action environment variable safely
     * @param name - Variable name
     * @param value - Variable value
     */
    void setEnv(const std::string& name, const std::string& value) {
        auto githubEnv = getEnvVar("GITHUB_ENV");
        if (githubEnv.has_value()) {
            // Write to GITHUB_ENV file
            // In practice, this would append to the file
        }
    }

    /**
     * Log warning with GitHub Actions annotation
     * @param message - Warning message
     */
    void logWarning(const std::string& message) {
        std::cout << "::warning::" << message << std::endl;
    }

    /**
     * Log error with GitHub Actions annotation
     * @param message - Error message
     */
    void logError(const std::string& message) {
        std::cerr << "::error::" << message << std::endl;
    }

    /**
     * Exit gracefully with proper session log cleanup
     * @param exitCode - Exit code
     */
    void exitGracefully(int exitCode) {
        // Perform std::any cleanup
        std::exit(exitCode);
    }

    /**
     * Wrap a std::function with defensive error handling
     * @param fn - Function to wrap
     * @param maxRetries - Maximum retry attempts
     * @return Result of the std::function or nullopt on failure
     */
    template<typename Func>
    auto withRetry(Func fn, int maxRetries = 3) -> std::optional<decltype(fn())> {
        for (int attempt = 0; attempt < maxRetries; ++attempt) {
            try {
                return fn();
            } catch (const std::exception& e) {
                if (attempt < maxRetries - 1) {
                    int delayMs = static_cast<int>(std::pow(2, attempt) * 1000);
                    std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
                }
            }
        }
        return std::nullopt;
    }

    /**
     * Get the workflow context
     */
    const WorkflowContext& getContext() const {
        return context_;
    }

    /**
     * Get the configuration
     */
    const WorkflowConfig& getConfig() const {
        return config_;
    }
};

/**
 * Factory std::function to create a WorkflowUtils instance with environment-based configuration
 */
WorkflowUtils createWorkflowUtils();

} // namespace elizaos
