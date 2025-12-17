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



// Type-safe wrapper for readContract with dynamic ABIs
struct ReadContractParams {
    Address address;
    Abi abi;
    std::string functionName;
    std::optional<std::vector<readonly unknown>> args;
};

// Use type assertion to bypass viem's strict authorizationList requirement

std::future<void> POST(NextRequest request);

std::future<void> GET(NextRequest request);

} // namespace elizaos
