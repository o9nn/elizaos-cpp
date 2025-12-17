#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".utils/files.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Terminal-based trajectory inspector
 * This is a simplified version - a full implementation would use a terminal UI library
 */

struct TrajectoryStep {
    std::optional<std::string> thought;
    std::optional<std::string> action;
    std::optional<std::string> observation;
    std::optional<std::string> response;
    std::optional<double> execution_time;
    std::string content;
};

struct TrajectoryData {
    std::vector<TrajectoryStep> trajectory;
};

/**
 * Inspector CLI class
 */
class TrajectoryInspector {
public:
    TrajectoryInspector(const std::string& trajPath, std::optional<std::string> dataPath);
    double nSteps() const;
    void showStep();
    void showInfo();
    void showHelp();
    std::future<void> run();

private:
    TrajectoryData trajData_;
};

/**
 * Find trajectory files in a directory
 */
std::vector<std::string> findTrajFiles(const std::string& dir);

/**
 * Main function for inspector CLI
 */
std::future<void> inspectorCli(string = '::' trajectoryPath, std::optional<std::string> dataPath);

} // namespace elizaos
