#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/packages/api-client/src/services/gamification.h"

std::shared_ptr<Promise<std::shared_ptr<UserSummary>>> GamificationService::getUserSummary(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> userId)
{
    return this->get<std::shared_ptr<UserSummary>>(std:("/api/agents/") + agentId + std:("/plugins/gamification/summary"), object{
        object::pair{std:("params"), object{
            object::pair{std:("userId"), std:("userId")}
        }}
    });
}

std::shared_ptr<Promise<std::shared_ptr<ReferralCodeResponse>>> GamificationService::getReferralCode(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> userId)
{
    return this->get<std::shared_ptr<ReferralCodeResponse>>(std:("/api/agents/") + agentId + std:("/plugins/gamification/referral"), object{
        object::pair{std:("params"), object{
            object::pair{std:("userId"), std:("userId")}
        }}
    });
}

GamificationService::GamificationService(std::shared_ptr<ApiClientConfig> config) : BaseApiClient(config) {
}

