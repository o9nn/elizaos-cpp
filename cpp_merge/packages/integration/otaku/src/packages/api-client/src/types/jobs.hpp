#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PACKAGES_API_CLIENT_SRC_TYPES_JOBS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PACKAGES_API_CLIENT_SRC_TYPES_JOBS_H
#include "core.hpp"
// External dependency removed

enum struct JobStatus;
class CreateJobRequest;
class CreateJobResponse;
class JobResult;
class JobDetailsResponse;
class PollOptions;
class JobHealthResponse;
class JobListResponse;
class ListJobsParams;
class PollResult;

enum struct JobStatus {
    PENDING = std:("pending"), PROCESSING = std:("processing"), COMPLETED = std:("completed"), FAILED = std:("failed"), TIMEOUT = std:("timeout")
};
extern object JobValidation;
class CreateJobRequest : public object, public std::enable_shared_from_this<CreateJobRequest> {
public:
    using std::enable_shared_from_this<CreateJobRequest>::shared_from_this;
    std::shared_ptr<UUID> agentId;

    std::shared_ptr<UUID> userId;

    string content;

    Record<string, any> metadata;

    double timeoutMs;
};

class CreateJobResponse : public object, public std::enable_shared_from_this<CreateJobResponse> {
public:
    using std::enable_shared_from_this<CreateJobResponse>::shared_from_this;
    string jobId;

    JobStatus status;

    double createdAt;

    double expiresAt;
};

class JobResult : public object, public std::enable_shared_from_this<JobResult> {
public:
    using std::enable_shared_from_this<JobResult>::shared_from_this;
    object message;

    double processingTimeMs;
};

class JobDetailsResponse : public object, public std::enable_shared_from_this<JobDetailsResponse> {
public:
    using std::enable_shared_from_this<JobDetailsResponse>::shared_from_this;
    string jobId;

    JobStatus status;

    std::shared_ptr<UUID> agentId;

    std::shared_ptr<UUID> userId;

    string prompt;

    double createdAt;

    double expiresAt;

    std::shared_ptr<JobResult> result;

    string error;

    Record<string, any> metadata;
};

class PollOptions : public object, public std::enable_shared_from_this<PollOptions> {
public:
    using std::enable_shared_from_this<PollOptions>::shared_from_this;
    double interval;

    double maxAttempts;

    double timeout;

    std::function<void(std::shared_ptr<JobDetailsResponse>, double)> onProgress;
};

class JobHealthResponse : public object, public std::enable_shared_from_this<JobHealthResponse> {
public:
    using std::enable_shared_from_this<JobHealthResponse>::shared_from_this;
    boolean healthy;

    double timestamp;

    double totalJobs;

    object statusCounts;

    object metrics;

    double maxJobs;
};

class JobListResponse : public object, public std::enable_shared_from_this<JobListResponse> {
public:
    using std::enable_shared_from_this<JobListResponse>::shared_from_this;
    array<std::shared_ptr<JobDetailsResponse>> jobs;

    double total;

    double filtered;
};

class ListJobsParams : public object, public std::enable_shared_from_this<ListJobsParams> {
public:
    using std::enable_shared_from_this<ListJobsParams>::shared_from_this;
    double limit;

    JobStatus status;
};

class PollResult : public object, public std::enable_shared_from_this<PollResult> {
public:
    using std::enable_shared_from_this<PollResult>::shared_from_this;
    boolean success;

    std::shared_ptr<JobDetailsResponse> job;

    double attempts;

    double timeMs;
};

#endif
