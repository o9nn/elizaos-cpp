#include "utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string ensureLogsDirectory() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto __filename = fileURLToPath(import.meta.url);
    const auto __dirname = path.dirname(__filename);
    const auto logsDir = path.join(__dirname, 'logs');

    if (!fs.existsSync(logsDir)) {
        fs.mkdirSync(logsDir, { recursive: true });
        std::cout << "Created logs directory at " + std::to_string(logsDir) << std::endl;
    }

    return logsDir;

}

std::string generateTimestamp() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return new Date().toISOString().replace(/:/g, '-');

}

std::string createScalabilityAnalysisHeader() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    "AGENT SCALABILITY ANALYSIS\n";
    "================\n";
    "Started: " + std::to_string(new Date().toISOString()) + "\n\n"
    "This test suite runs progressively larger load tests to identify breaking points:\n"
    "- 2 agents (baseline)\n";
    "- 5 agents (small group)\n";
    "- 10 agents (medium group)\n";
    "- 50 agents (large group)\n";
    "- 100 agents (very large group)\n";
    "- 250 agents (mass scale)\n";
    "- 500 agents (extreme scale)\n";
    "- 1000 agents (maximum load)\n";
    "- 2000 agents (breaking point test)\n\n";
    "OBJECTIVE: Identify thresholds and optimize for scalability\n\n"
    "TESTING LOG:\n"
    "========\n\n";
    );

}

std::string createLoadTestSummaryHeader() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    'Scale Testing Summary - Finding Breaking Points\n' +;
    '=============================\n' +;
    "Test Started: " + std::to_string(new Date().toISOString()) + "\n\n"
    'Configuration | Agents | Messages | Success Rate | Avg Response | Memory | Throughput | Errors\n' +;
    '-------------|--------|----------|--------------|--------------|--------|------------|-------\n';
    );

}

std::string createDetailedTestLogHeader(ScaleConfig config) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    "Load Test: " + std::to_string(config.description) + "\n"
    "===========" + std::to_string('='.repeat(config.description.length)) + "\n";
    "Agents: " + std::to_string(config.agents) + "\n"
    "Messages: " + std::to_string(config.messageCount) + "\n"
    "Started: " + std::to_string(new Date().toISOString()) + "\n\n"
    'DETAILED TEST LOG:\n' +
    '============\n\n';
    );

}

std::string formatBreakingPointNotification(LoadTestMetrics metrics, ThresholdLevel thresholdLevel) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto thresholds = THRESHOLD_CONFIGS[thresholdLevel];
    return (;
    "\n*** BREAKING POINT IDENTIFIED WITH " + std::to_string(thresholdLevel) + " THRESHOLD ***\n";
    "Threshold configuration: " + std::to_string(thresholdLevel) + "\n"
    "Response time threshold: " + std::to_string(thresholds.responseTimeThreshold) + "ms (actual: " + std::to_string(metrics.avgResponseTime.toFixed(2)) + "ms)\n"
    "Error rate threshold: " + std::to_string(thresholds.errorRateThreshold) + "% (actual: " + std::to_string(((metrics.errorCount / metrics.messagesSent) * 100).toFixed(2)) + "%)\n"
    "Success rate threshold: " + std::to_string(thresholds.successRateThreshold) + "% (actual: " + std::to_string(metrics.successRate.toFixed(2)) + "%)\n"
    "Memory threshold: " + std::to_string(thresholds.memoryThreshold) + "MB (peak: " + std::to_string(metrics.peakMemoryUsage.toFixed(2)) + "MB)\n"
    );

}

std::string formatScalabilityConclusions(const std::string& conclusions) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    "\n\nSCALABILITY CONCLUSIONS\n";
    "===============\n\n";
    conclusions +;
    '\n\n' +;
    "For detailed metrics on each test configuration, see the individual test logs.\n";
    );

}

std::string formatVisualizationGuide() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    "\n\nVISUALIZATION\n";
    "=========\n";
    "To visualize the scalability results, you can plot the following metrics:\n"
    "1. Success Rate vs. Agent Count\n";
    "2. Response Time vs. Agent Count\n";
    "3. Throughput vs. Agent Count\n";
    "4. Error Rate vs. Agent Count\n\n";
    "The optimal configuration is typically just before the breaking point where:\n"
    "- Success rate is still high (>95%)\n";
    "- Response times remain reasonable (<1000ms)\n";
    "- Throughput is maximized\n\n";
    "Test completed at: " + std::to_string(new Date().toISOString()) + "\n"
    "============\n\n";
    );

}

std::string formatErrorMessage(const std::any& error) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    "\n\nERROR DURING TESTING:\n"
    "=============\n";
    std::to_string(error.stdout || '') + "\n";
    std::to_string(error.stderr || '') + "\n";
    std::to_string(error.message) + "\n\n";
    "Even though an error occurred, this might represent the actual breaking point.\n";
    "Examine the logs to determine the last successful configuration.\n\n";
    "Test failed at: " + std::to_string(new Date().toISOString()) + "\n"
    );

}

bool isBreakingPoint(LoadTestMetrics metrics, ThresholdLevel thresholdLevel) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto thresholds = THRESHOLD_CONFIGS[thresholdLevel];

    const auto errorRate = (metrics.errorCount / metrics.messagesSent) * 100;

    return (;
    errorRate > thresholds.errorRateThreshold ||;
    metrics.avgResponseTime > thresholds.responseTimeThreshold ||;
    metrics.successRate < thresholds.successRateThreshold ||;
    metrics.peakMemoryUsage > thresholds.memoryThreshold;
    );

}

std::string formatDetailedMetrics(ScaleConfig config, LoadTestMetrics metrics) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    auto output = '\n======= DETAILED METRICS =======\n';
    "Configuration: " + std::to_string(config.description) + "\n"
    "Agents: " + std::to_string(config.agents) + "\n"
    "Messages: " + std::to_string(config.messageCount) + "\n"
    "Test Duration: " + std::to_string((metrics.totalTime / 1000).toFixed(2)) + " seconds\n\n"

    output += 'PERFORMANCE METRICS:\n';
    "- Messages Sent: " + std::to_string(metrics.messagesSent) + "\n"
    "- Messages Processed: " + std::to_string(metrics.messagesProcessed) + "\n"
    "- Success Rate: " + std::to_string(metrics.successRate.toFixed(2)) + "%\n"
    "- Throughput: " + std::to_string(metrics.throughput.toFixed(2)) + " messages/second\n\n"

    output += 'RESPONSE TIME METRICS:\n';
    "- Average Response Time: " + std::to_string(metrics.avgResponseTime.toFixed(2)) + "ms\n"
    "- Minimum Response Time: " + std::to_string(metrics.minResponseTime) + "ms\n"
    "- Maximum Response Time: " + std::to_string(metrics.maxResponseTime) + "ms\n\n"

    output += 'ERROR METRICS:\n';
    "- Total Errors: " + std::to_string(metrics.errorCount) + "\n"
    "- Timeout Count: " + std::to_string(metrics.timeoutCount) + "\n"

    if (Object.keys(metrics.errorTypes).length > 0) {
        output += '- Error Types:\n';
        for (const int [type, count] of Object.entries(metrics.errorTypes)) {
            "  * " + std::to_string(type) + ": " + std::to_string(count) + "\n"
        }
    }

    output += '\nSYSTEM RESOURCE METRICS:\n';
    "- Memory Usage: " + std::to_string(metrics.memoryUsageStart.toFixed(2)) + "MB → " + std::to_string(metrics.memoryUsageEnd.toFixed(2)) + "MB\n"
    "- Peak Memory Usage: " + std::to_string(metrics.peakMemoryUsage.toFixed(2)) + "MB\n"
    output += '========================\n';

    return output;

}

std::string formatSummaryLogLine(ScaleConfig config, LoadTestMetrics metrics) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return (;
    std::to_string(config.description.padEnd(13)) + " | " + std::to_string(std::to_string(config.agents).padEnd(6)) + " | ";
    std::to_string(std::to_string(config.messageCount).padEnd(8)) + " | ";
    std::to_string(metrics.successRate.toFixed(2)) + "% | ";
    std::to_string(metrics.avgResponseTime.toFixed(2)) + "ms | ";
    std::to_string(metrics.peakMemoryUsage.toFixed(2)) + "MB | ";
    std::to_string(metrics.throughput.toFixed(2)) + "/s | ";
    std::to_string(metrics.errorCount) + "\n";
    );

}

} // namespace elizaos
