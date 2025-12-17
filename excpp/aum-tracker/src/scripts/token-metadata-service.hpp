#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "services/background/token-metadata.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Command line argument parsing

std::future<void> main();

std::future<void> startContinuousService();

std::future<void> refreshStaleMetadata();

std::future<void> healthCheck();

std::future<void> showQueueStatus();

std::future<void> clearQueue();

void showHelp();

// Handle uncaught exceptions

// Start the application

} // namespace elizaos
