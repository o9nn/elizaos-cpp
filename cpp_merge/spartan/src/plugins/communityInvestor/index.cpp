#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/communityInvestor/index.h"

std::shared_ptr<Plugin> communityInvestorPlugin = object{
    object::pair{std:("name"), std:("community-investor")}, 
    object::pair{std:("description"), std:("A plugin for community-driven investment alpha and trust scoring.")}, 
    object::pair{std:("config"), object{
        object::pair{std:("BIRDEYE_API_KEY"), string_empty}, 
        object::pair{std:("DEXSCREENER_API_KEY"), string_empty}, 
        object::pair{std:("HELIUS_API_KEY"), string_empty}, 
        object::pair{std:("PROCESS_TRADE_DECISION_INTERVAL_HOURS"), std:("1")}, 
        object::pair{std:("METRIC_REFRESH_INTERVAL_HOURS"), std:("24")}, 
        object::pair{std:("USER_TRADE_COOLDOWN_HOURS"), std:("12")}, 
        object::pair{std:("SCAM_PENALTY"), std:("-100")}, 
        object::pair{std:("SCAM_CORRECT_CALL_BONUS"), std:("100")}, 
        object::pair{std:("MAX_RECOMMENDATIONS_IN_PROFILE"), std:("50")}
    }}, 
    , 
    object::pair{std:("services"), array<CommunityInvestorService>{ CommunityInvestorService }}, 
    object::pair{std:("routes"), communityInvestorRoutes}, 
    object::pair{std:("events"), std:("events")}, 
    object::pair{std:("tests"), array<any>{ allCommunityInvestorPluginTests }}
};
array<std::shared_ptr<AgentPanel>> panels = array<std::shared_ptr<AgentPanel>>{ object{
    object::pair{std:("name"), std:("Trust Marketplace")}, 
    object::pair{std:("path"), std:("display")}, 
    object::pair{std:("component"), std:("LeaderboardPanelPage")}, 
    object::pair{std:("icon"), std:("UsersRound")}, 
    object::pair{std:("public"), true}
} };

void Main(void)
{
}

MAIN
