#ifndef _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_RUN_INSPECTOR_CLI_H
#define _HOME_RUNNER_WORK_ELIZAOS_CPP_ELIZAOS_CPP_SWEAGENT_SRC_RUN_INSPECTOR_CLI_H
#include "core.hpp"
#include <string>
#include "path.hpp"
#include "fs.hpp"
#include "readline.hpp"
#include "js-yaml.h"
#include "../utils/files.h"

class TrajectoryStep;
class TrajectoryData;
class TrajectoryInspector;

class TrajectoryStep : public object, public std::enable_shared_from_this<TrajectoryStep> {
public:
    using std::enable_shared_from_this<TrajectoryStep>::shared_from_this;
    string thought;

    string action;

    string observation;

    string response;

    double execution_time;

    Record<string, any> state;

    array<object> query;

    Record<string, any> extraInfo;
};

class TrajectoryData : public object, public std::enable_shared_from_this<TrajectoryData> {
public:
    using std::enable_shared_from_this<TrajectoryData>::shared_from_this;
    array<std::shared_ptr<TrajectoryStep>> trajectory;

    Record<string, any> info;

    array<object> history;

    Record<string, any> replay_config;
};

class TrajectoryInspector : public object, public std::enable_shared_from_this<TrajectoryInspector> {
public:
    using std::enable_shared_from_this<TrajectoryInspector>::shared_from_this;
    std::shared_ptr<TrajectoryData> trajData;

    double currentStep = -1;

    boolean showFull = false;

    string goldPatch;

    TrajectoryInspector(string trajPath, string dataPath = std::nullopt);
    virtual double get_nSteps();
    virtual void showStep();
    virtual void showInfo();
    virtual void showHelp();
    virtual std::shared_ptr<Promise<void>> run();
};

array<string> findTrajFiles(string dir);

std::shared_ptr<Promise<void>> inspectorCli(string trajectoryPath = std::string("."), string dataPath = std::nullopt);

#endif
