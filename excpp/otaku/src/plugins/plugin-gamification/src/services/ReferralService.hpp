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
#include "GamificationService.hpp"
#include "constants.hpp"
#include "elizaos/core.hpp"
#include "schema.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class ReferralService {
public:
    DatabaseAdapter getDb();
    static std::future<ReferralService> start(IAgentRuntime runtime);
    Promise< getOrCreateCode(UUID userId);
    std::future<ReferralStats> getReferralStats(UUID userId);
    std::future<void> stop();
    std::future<void> processReferralSignup(UUID userId, const std::string& referralCode);
    std::future<void> processReferralActivation(UUID userId);
    std::string generateReferralCode(UUID userId);
};


} // namespace elizaos
