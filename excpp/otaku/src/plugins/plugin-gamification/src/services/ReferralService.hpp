#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".constants.hpp"
#include ".schema.hpp"
#include "GamificationService.hpp"
#include "elizaos/core.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class ReferralService {
public:
    DatabaseAdapter getDb();
    std::future<ReferralService> start(IAgentRuntime runtime);
    Promise< getOrCreateCode(UUID userId);
    void if(auto existing);
    std::future<ReferralStats> getReferralStats(UUID userId);
    std::future<void> stop();
    std::future<void> processReferralSignup(UUID userId, const std::string& referralCode);
    std::future<void> processReferralActivation(UUID userId);
    std::string generateReferralCode(UUID userId);
};


} // namespace elizaos
