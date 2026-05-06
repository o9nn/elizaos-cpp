#pragma once
#include <algorithm>
#include <any>
#include <chrono>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "services/csv.hpp"
#include "services/database.hpp"
#include "services/solana.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct PrefetchConfig {
    double batchSize;
    double delayBetweenBatches;
    double maxRetries;
    bool resumeFromFailures;
    bool forceRefresh;
    std::optional<double> limit;
};

struct PrefetchStats {
    double totalWallets;
    double processed;
    double successful;
    double failed;
    double skipped;
    std::chrono::system_clock::time_point startTime;
    std::optional<std::chrono::system_clock::time_point> endTime;
    std::vector<std::string> errors;
};

class PrefetchService {
public:
    PrefetchService(std::optional<PrefetchConfig> config = {});
    std::future<void> handleShutdown();
    void printProgress();
    void printFinalStats();
    std::future<void> processWalletBatch(const std::vector<std::any>& wallets);
    std::future<void> refreshPrices();
    std::future<void> run();
    std::future<void> runHealthCheck();

// CLI interface
std::future<void> main();

// Run the script


} // namespace elizaos
