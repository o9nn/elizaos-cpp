#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Progress management for batch runs
 */

//   // Currently unused

/**
 * Interface for tracking instance statistics
 * Currently unused but may be needed in future
 */
/* struct InstanceStats {
 */

/**
 * Progress manager for batch runs
 */
struct SpinnerTask {
    std::string status;
    double startTime;
};

class RunBatchProgressManager {
public:
    RunBatchProgressManager(double _numInstances, std::optional<std::string> yamlReportPath);
    double nCompleted() const;
    void updateExitStatusTable();
    std::string shortenStr(const std::string& s, double maxLen, boolean = false shortenLeft);
    void updateInstanceStatus(const std::string& instanceId, const std::string& message);
    void onInstanceStart(const std::string& instanceId);
    void onInstanceEnd(const std::string& instanceId, const std::optional<std::string>& exitStatus);
    void onUncaughtException(const std::string& instanceId, Error exception);
    void printReport();
    std::unordered_map<std::string, std::any> getOverviewData();
    void saveOverviewDataYaml(const std::string& filePath);
};


} // namespace elizaos
