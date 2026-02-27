#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PACKAGES_SERVER_SRC_TYPES_JOBS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_OTAKU_SRC_PACKAGES_SERVER_SRC_TYPES_JOBS_H
#include "core.h"
#include "@elizaos/core.h"

enum struct JobStatus;
class CreateJobRequest;
class CreateJobResponse;
class JobResult;
class JobDetailsResponse;
class PollOptions;
class Job;

enum struct JobStatus {
    PENDING = std:("pending"), PROCESSING = std:("processing"), COMPLETED = std:("completed"), FAILED = std:("failed"), TIMEOUT = std:("timeout")
};
class CreateJobRequest : public object, public std::enable_shared_from_this<CreateJobRequest> {
public:
    using std::enable_shared_from_this<CreateJobRequest>::shared_from_this;
    string agentId;

    string userId;

    string prompt;

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

    string agentId;

    string userId;

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
    string jobId;

    double interval;

    double maxAttempts;

    double timeout;
};

class Job : public object, public std::enable_shared_from_this<Job> {
public:
    using std::enable_shared_from_this<Job>::shared_from_this;
    string id;

    std::shared_ptr<UUID> agentId;

    std::shared_ptr<UUID> userId;

    std::shared_ptr<UUID> channelId;

    string prompt;

    JobStatus status;

    double createdAt;

    double expiresAt;

    std::shared_ptr<UUID> userMessageId;

    std::shared_ptr<UUID> agentResponseId;

    std::shared_ptr<JobResult> result;

    string error;

    Record<string, any> metadata;
};

#endif
