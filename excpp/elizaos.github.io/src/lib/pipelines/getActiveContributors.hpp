#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".date-utils.hpp"
#include "queryHelpers.hpp"
#include "types.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



std::future<void> getActiveContributors(QueryParams params = {});

    // Create query params for the interval

    // Return the interval with active contributors for this time period

} // namespace elizaos
