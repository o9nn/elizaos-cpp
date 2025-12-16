#pragma once
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use


/**
 * Job status enumeration
 */
enum JobStatus {
}

/**
 * Request to create a new job
 */
struct CreateJobRequest {
    std::optional<std::string> agentId;
    std::optional<std::string> userId;
    std::string prompt;
    std::optional<double> timeoutMs;
};

/**
 * Response when creating a job
 */
struct CreateJobResponse {
    std::string jobId;
    JobStatus status;
    double createdAt;
    double expiresAt;
};

/**
 * Job result structure
 */
struct JobResult {
    std::string id;
    std::string content;
    std::string authorId;
    double createdAt;
    double processingTimeMs;
};

/**
 * Job details response
 */
struct JobDetailsResponse {
    std::string jobId;
    JobStatus status;
    std::string agentId;
    std::string userId;
    std::string prompt;
    double createdAt;
    double expiresAt;
    std::optional<JobResult> result;
    std::optional<std::string> error;
};

/**
 * Poll options for checking job status
 */
struct PollOptions {
    std::string jobId;
    std::optional<double> interval;
    std::optional<double> maxAttempts;
    std::optional<double> timeout;
};

/**
 * Internal job storage structure
 */
struct Job {
    std::string id;
    UUID agentId;
    UUID userId;
    UUID channelId;
    std::string prompt;
    JobStatus status;
    double createdAt;
    double expiresAt;
    std::optional<UUID> userMessageId;
    std::optional<UUID> agentResponseId;
    std::optional<JobResult> result;
    std::optional<std::string> error;
};


} // namespace elizaos
