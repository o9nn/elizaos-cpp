#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/packages/api-client/src/services/gamification.h"

std::shared_ptr<Promise<std::shared_ptr<UserSummary>>> GamificationService::getUserSummary(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> userId)
{
    return this->get<std::shared_ptr<UserSummary>>(std::string("/api/agents/") + agentId + std::string("/plugins/gamification/summary"), object{
        object::pair{std::string("params"), object{
            object::pair{std::string("userId"), std::string("userId")}
        }}
    });
}

std::shared_ptr<Promise<std::shared_ptr<ReferralCodeResponse>>> GamificationService::getReferralCode(std::shared_ptr<UUID> agentId, std::shared_ptr<UUID> userId)
{
    return this->get<std::shared_ptr<ReferralCodeResponse>>(std::string("/api/agents/") + agentId + std::string("/plugins/gamification/referral"), object{
        object::pair{std::string("params"), object{
            object::pair{std::string("userId"), std::string("userId")}
        }}
    });
}

GamificationService::GamificationService(std::shared_ptr<ApiClientConfig> config) : BaseApiClient(config) {
}

