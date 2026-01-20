#pragma once
#include <algorithm>
#include <any>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Threshold configurations for breaking point detection
 */
enum ThresholdLevel {
}

struct ThresholdConfig {
    double responseTimeThreshold;
    double errorRateThreshold;
    double successRateThreshold;
    double memoryThreshold;
    double cpuThreshold;
};

/**
 * Ensures the logs directory exists and returns the path
 */
std::string ensureLogsDirectory();

/**
 * Create a timestamp string for file naming
 */
std::string generateTimestamp();

/**
 * Generate a scalability analysis header for log files
 */
std::string createScalabilityAnalysisHeader();

/**
 * Create a summary file header for load test results
 */
std::string createLoadTestSummaryHeader();

/**
 * Create a detailed test log header
 */
std::string createDetailedTestLogHeader(ScaleConfig config);

/**
 * Format a breaking point notification
 */
std::string formatBreakingPointNotification(LoadTestMetrics metrics, ThresholdLevel thresholdLevel);

/**
 * Format a scalability conclusions section
 */
std::string formatScalabilityConclusions(const std::string& conclusions);

/**
 * Format a visualization guide section
 */
std::string formatVisualizationGuide();

/**
 * Format an error message for the log file
 */
std::string formatErrorMessage(const std::any& error);

/**
 * Check if a breaking point is reached based on metrics and threshold level
 */
bool isBreakingPoint(LoadTestMetrics metrics, ThresholdLevel thresholdLevel);

/**
 * Format detailed metrics for a log file
 */
std::string formatDetailedMetrics(ScaleConfig config, LoadTestMetrics metrics);

/**
 * Format a summary log line
 */
std::string formatSummaryLogLine(ScaleConfig config, LoadTestMetrics metrics);

// Export all functions for both TS and JS usage

} // namespace elizaos
