#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PACKAGES_API_CLIENT_SRC_TYPES_RUNS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_OTAKU_SRC_PACKAGES_API_CLIENT_SRC_TYPES_RUNS_H
#include "core.hpp"
// External dependency removed

typedef any RunStatus;

typedef any RunEventType;

class RunCounts;
class RunSummary;
class RunEvent;
class RunDetail;
class ListRunsParams;

class RunCounts : public object, public std::enable_shared_from_this<RunCounts> {
public:
    using std::enable_shared_from_this<RunCounts>::shared_from_this;
    double actions;

    double modelCalls;

    double errors;

    double evaluators;
};

class RunSummary : public object, public std::enable_shared_from_this<RunSummary> {
public:
    using std::enable_shared_from_this<RunSummary>::shared_from_this;
    std::shared_ptr<UUID> runId;

    RunStatus status;

    any startedAt;

    any endedAt;

    any durationMs;

    std::shared_ptr<UUID> messageId;

    std::shared_ptr<UUID> roomId;

    std::shared_ptr<UUID> entityId;

    std::shared_ptr<RunCounts> counts;

    Record<string, any> metadata;
};

class RunEvent : public object, public std::enable_shared_from_this<RunEvent> {
public:
    using std::enable_shared_from_this<RunEvent>::shared_from_this;
    RunEventType type;

    double timestamp;

    Record<string, any> data;
};

class RunDetail : public object, public std::enable_shared_from_this<RunDetail> {
public:
    using std::enable_shared_from_this<RunDetail>::shared_from_this;
    std::shared_ptr<RunSummary> summary;

    array<std::shared_ptr<RunEvent>> events;
};

class ListRunsParams : public object, public std::enable_shared_from_this<ListRunsParams> {
public:
    using std::enable_shared_from_this<ListRunsParams>::shared_from_this;
    std::shared_ptr<UUID> roomId;

    any status;

    double limit;

    double from;

    double to;
};

#endif
