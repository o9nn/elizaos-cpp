#pragma once
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "elizaos/core.hpp"
#include "services/defillama.service.hpp"
#include "utils/action-helpers.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



      // Parse compact parameter (default to true for efficiency)

      // Apply downsampling and calculate summary in compact mode

      // Calculate summary statistics for the full limited series (before downsampling)

using TvlSummary = {

using ProtocolHistoryResponse = {

std::string determineSlug(ProtocolSummary summary);


} // namespace elizaos
