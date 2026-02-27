#include "elizaos/core.hpp"
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
 * Validation constants for job requests
 */

/**
 * Request to create a new job
 */
struct CreateJobRequest {
    std::optional<UUID> agentId;
    UUID userId;
    std: content;
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
    UUID id;
    std: content;
    UUID authorId;
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
    UUID agentId;
    UUID userId;
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
    std::optional<double> interval;
    std::optional<double> maxAttempts;
    std::optional<double> timeout;
    std::optional<[&](status: JobDetailsResponse, attempt) { return void> onProgress; };
};

/**
 * Health check response with metrics
 */
struct JobHealthResponse {
    bool healthy;
    double timestamp;
    double totalJobs;
    { statusCounts;
    double pending;
    double processing;
    double completed;
    double failed;
    double timeout;
    { metrics;
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
