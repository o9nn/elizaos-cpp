#ifndef _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_RUN_PROGRESS_H
#define _HOME_RUNNER_WORK_ELIZAOS-CPP_ELIZAOS-CPP_SWEAGENT_SRC_RUN_PROGRESS_H
#include "core.h"
#include "fs.h"
#include "js-yaml.h"

class SpinnerTask;
class RunBatchProgressManager;

class SpinnerTask : public object, public std::enable_shared_from_this<SpinnerTask> {
public:
    using std::enable_shared_from_this<SpinnerTask>::shared_from_this;
    std::string status;

    double startTime;
};

class RunBatchProgressManager : public object, public std::enable_shared_from_this<RunBatchProgressManager> {
public:
    using std::enable_shared_from_this<RunBatchProgressManager>::shared_from_this;
    std::shared_ptr<Map<std::string, std::shared_ptr<SpinnerTask>>> spinnerTasks = std::make_shared<Map>();

    std::shared_ptr<Map<std::any, array<string>>> instancesByExitStatus = std::make_shared<Map>();

    std::string yamlReportPath;

    RunBatchProgressManager(double _numInstances, std::string yamlReportPath = undefined);
    virtual double get_nCompleted();
    virtual void updateExitStatusTable();
    virtual std::string shortenStr(std::string s, double maxLen, boolean shortenLeft = false);
    virtual void updateInstanceStatus(std::string instanceId, std::string message);
    virtual void onInstanceStart(std::string instanceId);
    virtual void onInstanceEnd(std::string instanceId, std::any exitStatus);
    virtual void onUncaughtException(std::string instanceId, std::shared_ptr<Error> std::exception);
    virtual void printReport();
    virtual Record<std::string, any> getOverviewData();
    virtual void saveOverviewDataYaml(std::string filePath);
};

#endif
