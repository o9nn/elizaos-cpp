#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".agent/problem-statement.hpp"
#include ".environment/deployment.hpp"
#include ".environment/swe-env.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Batch instance handling
 * Converted from sweagent/run/batch_instances.py
 */

/**
 * Abstract instance source
 */

/**
 * A single instance in a batch
 */
struct BatchInstance {
    EnvironmentConfig env;
    ProblemStatementConfig problemStatement;
};

/**
 * Simple batch instance for benchmarking
 */
struct SimpleBatchInstance {
    std::string imageName;
    std::string problemStatement;
    std::string instanceId;
    std::optional<std::string> repoName;
    std::optional<std::string> baseCommit;
};

/**
 * Convert simple instance to full batch instance
 */
BatchInstance simpleToFullBatchInstance(SimpleBatchInstance simple, DeploymentConfig deployment);

/**
 * Slice specification to slice object
 */

/**
 * Simple seeded random number generator
 */
std::function<double()> seededRandom(double seed);

/**
 * Filter batch items
 */
std::vector<BatchInstance> filterBatchItems(const std::vector<BatchInstance>& instances, std::any options = {});

/**
 * Load instances from file
 */
class InstancesFromFile {
public:
    InstancesFromFile(std::optional<std::any> config);
    std::vector<BatchInstance> getInstanceConfigs();
    std::string id() const;

private:
    std::string path_;
    std::string _filter_;
    std::string _slice_;
    bool _shuffle_;
    DeploymentConfig _deployment_;
};

/**
 * Convert SWE-bench instance to SimpleBatchInstance
 */
SimpleBatchInstance fromSWEBench(const std::unordered_map<std::string, unknown>& sweBenchInstance);

/**
 * SWE-bench instances
 */
class SWEBenchInstances {
public:
    SWEBenchInstances(std::optional<std::any> config);
    std::string getDatasetPath();
    std::vector<BatchInstance> getInstanceConfigs();
    std::string id() const;
    bool isEvaluationEnabled() const;

private:
    std::string _filter_;
    std::string _slice_;
    bool _shuffle_;
    DeploymentConfig _deployment_;
};

/**
 * Create instance source from config
 */
AbstractInstanceSource createInstanceSource(BatchInstanceSourceConfig config);

} // namespace elizaos
