#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".agent/agents.hpp"
#include ".environment/swe-env.hpp"
#include ".types.hpp"
#include ".utils/config.hpp"
#include ".utils/log.hpp"
#include "batch-instances.hpp"
#include "common.hpp"
#include "hooks.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Run batch of instances
 * Converted from sweagent/run/run_batch.py
 */

/**
 * Configuration schema
 */

/**
 * Progress manager for batch runs
 */
class BatchProgressManager {
public:
    BatchProgressManager(double numInstances);
    void onInstanceStart(const std::string& instanceId);
    void onInstanceEnd(const std::string& instanceId, const std::string& exitStatus);
    void onUncaughtException(const std::string& instanceId, Error error);
    void printReport();

private:
    double totalInstances_;
    AgentLogger logger_;
};

/**
 * Run batch of instances
 */
class RunBatch {
public:
    RunBatch(std::optional<std::any> config);
    RunBatch fromConfig(RunBatchConfig config);
    void addHook(RunHook hook);
    std::future<void> main();
    std::future<void> mainSingleWorker();
    std::future<void> mainMultiWorker();
    std::future<void> worker(const std::vector<BatchInstance>& queue, double _workerId);
    std::future<void> runInstance(BatchInstance instance);
    std::future<AgentRunResult> _runInstance(BatchInstance instance);
    bool shouldSkip(BatchInstance instance);

private:
    std::vector<BatchInstance> instances_;
    AgentConfig agentConfig_;
    std::string outputDir_;
    std::vector<RunHook> hooks_;
    bool raiseExceptions_;
    bool redoExisting_;
    double numWorkers_;
    bool _progressBar_;
    double randomDelayMultiplier_;
    AgentLogger logger_;
    BatchProgressManager progressManager_;
};

/**
 * Run from configuration
 */
std::future<void> runBatchFromConfig(RunBatchConfig config);

} // namespace elizaos
