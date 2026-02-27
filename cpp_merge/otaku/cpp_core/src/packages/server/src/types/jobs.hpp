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


/**
 * Job status enumeration
 */
enum JobStatus {
  PENDING = 'pending',
  PROCESSING = 'processing',
  COMPLETED = 'completed',
  FAILED = 'failed',
  TIMEOUT = 'timeout',
}

/**
 * Request to create a new job
 */
struct CreateJobRequest {
    std::optional<std:> agentId;
    std::optional<std:> userId;
    std: prompt;
    std::optional<std::unordered_map<std:, unknown>> metadata;
    std::optional<double> timeoutMs;
};

/**
 * Response when creating a job
 */
struct CreateJobResponse {
    std: jobId;
    JobStatus status;
    double createdAt;
    double expiresAt;
};

/**
 * Job result structure
 */
struct JobResult {
    { message;
    std: id;
    std: content;
    std: authorId;
    double createdAt;
    std::optional<std::unordered_map<std:, unknown>> metadata;
    double processingTimeMs;
};

/**
 * Job details response
 */
struct JobDetailsResponse {
    std: jobId;
    JobStatus status;
    std: agentId;
    std: userId;
    std: prompt;
    double createdAt;
    double expiresAt;
    std::optional<JobResult> result;
    std::optional<std:> error;
    std::optional<std::unordered_map<std:, unknown>> metadata;
};

/**
 * Poll options for checking job status
 */
struct PollOptions {
    std: jobId;
    std::optional<double> interval;
    std::optional<double> maxAttempts;
    std::optional<double> timeout;
};

/**
 * Internal job storage structure
 */
struct Job {
    std: id;
    UUID agentId;
    UUID userId;
    UUID channelId;
    std: prompt;
    JobStatus status;
    double createdAt;
    double expiresAt;
    std::optional<UUID> userMessageId;
    std::optional<UUID> agentResponseId;
    std::optional<JobResult> result;
    std::optional<std:> error;
    std::optional<std::unordered_map<std:, unknown>> metadata;
};


} // namespace elizaos
