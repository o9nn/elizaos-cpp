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
#include "cron.hpp"
#include "db.hpp"
#include "logger.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



std::future<void> setLastProcessedSlot(double slot);

std::future<double> findSlotAtOrBeforeTime(Connection connection, double targetTs, double low, double high);
std::future<void> processSlot(double slot, Connection connection);

// Scan blocks from the last processed slot up to the current slot
std::future<void> processMissedEvents(Connection connection);

} // namespace elizaos
