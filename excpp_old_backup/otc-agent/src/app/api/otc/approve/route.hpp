#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



// Helper to safely read from contract bypassing viem's strict authorizationList requirement
using ReadContractFn = (params: unknown) => Promise<unknown>;

std::future<void> POST(NextRequest request);

} // namespace elizaos
