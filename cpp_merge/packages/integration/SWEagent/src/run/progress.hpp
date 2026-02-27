#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_RUN_PROGRESS_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_RUN_PROGRESS_H
#include "core.hpp"
#include "fs.hpp"
#include "js-yaml.h"

class SpinnerTask;
class RunBatchProgressManager;

class SpinnerTask : public object, public std::enable_shared_from_this<SpinnerTask> {
public:
    using std::enable_shared_from_this<SpinnerTask>::shared_from_this;
    string status;

    double startTime;
};

class RunBatchProgressManager : public object, public std::enable_shared_from_this<RunBatchProgressManager> {
public:
    using std::enable_shared_from_this<RunBatchProgressManager>::shared_from_this;
    std::shared_ptr<Map<string, std::shared_ptr<SpinnerTask>>> spinnerTasks = std::make_shared<Map>();

    std::shared_ptr<Map<any, array<string>>> instancesByExitStatus = std::make_shared<Map>();

    string yamlReportPath;

    RunBatchProgressManager(double _numInstances, string yamlReportPath = std::nullopt);
    virtual double get_nCompleted();
    virtual void updateExitStatusTable();
    virtual string shortenStr(string s, double maxLen, boolean shortenLeft = false);
    virtual void updateInstanceStatus(string instanceId, string message);
    virtual void onInstanceStart(string instanceId);
    virtual void onInstanceEnd(string instanceId, any exitStatus);
    virtual void onUncaughtException(string instanceId, std::shared_ptr<Error> exception);
    virtual void printReport();
    virtual Record<string, any> getOverviewData();
    virtual void saveOverviewDataYaml(string filePath);
};

#endif
