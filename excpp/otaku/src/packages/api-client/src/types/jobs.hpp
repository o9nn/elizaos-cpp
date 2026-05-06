#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
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



/**
 * Job status enumeration
 */
enum JobStatus {
}

/**
 * Validation constants for job requests
 */

/**
 * Request to create a new job
 */
struct CreateJobRequest {
    std::optional<UUID> agentId;
    UUID userId;
    std::string content;
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
    UUID id;
    std::string content;
    UUID authorId;
    double createdAt;
    double processingTimeMs;
};

/**
 * Job details response
 */
struct JobDetailsResponse {
    std::string jobId;
    JobStatus status;
    UUID agentId;
    UUID userId;
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
    std::optional<double> interval;
    std::optional<double> maxAttempts;
    std::optional<double> timeout;
};

/**
 * Health check response with metrics
 */
struct JobHealthResponse {
    bool healthy;
    double timestamp;
    double totalJobs;
    double pending;
    double processing;
    double completed;
    double failed;
    double timeout;
    double averageProcessingTimeMs;
    double successRate;
    double failureRate;
    double timeoutRate;
    double maxJobs;
};

/**
 * Job list response
 */
struct JobListResponse {
    std::vector<JobDetailsResponse> jobs;
    double total;
    double filtered;
};

/**
 * Parameters for listing jobs
 */
struct ListJobsParams {
    std::optional<double> limit;
    std::optional<JobStatus> status;
};

/**
 * Poll result wrapper
 */
struct PollResult {
    bool success;
    JobDetailsResponse job;
    double attempts;
    double timeMs;
};


} // namespace elizaos
