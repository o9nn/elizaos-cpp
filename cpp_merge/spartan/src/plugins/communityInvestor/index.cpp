#include "index.hpp"
#include <string>

std::shared_ptr<Plugin> communityInvestorPlugin = object{
    object::pair{std::string("name"), std::string("community-investor")}, 
    object::pair{std::string("description"), std::string("A plugin for community-driven investment alpha and trust scoring.")}, 
    object::pair{std::string("config"), object{
        object::pair{std::string("BIRDEYE_API_KEY"), string_empty}, 
        object::pair{std::string("DEXSCREENER_API_KEY"), string_empty}, 
        object::pair{std::string("HELIUS_API_KEY"), string_empty}, 
        object::pair{std::string("PROCESS_TRADE_DECISION_INTERVAL_HOURS"), std::string("1")}, 
        object::pair{std::string("METRIC_REFRESH_INTERVAL_HOURS"), std::string("24")}, 
        object::pair{std::string("USER_TRADE_COOLDOWN_HOURS"), std::string("12")}, 
        object::pair{std::string("SCAM_PENALTY"), std::string("-100")}, 
        object::pair{std::string("SCAM_CORRECT_CALL_BONUS"), std::string("100")}, 
        object::pair{std::string("MAX_RECOMMENDATIONS_IN_PROFILE"), std::string("50")}
    }}, 
    , 
    object::pair{std::string("services"), array<CommunityInvestorService>{ CommunityInvestorService }}, 
    object::pair{std::string("routes"), communityInvestorRoutes}, 
    object::pair{std::string("events"), std::string("events")}, 
    object::pair{std::string("tests"), array<any>{ allCommunityInvestorPluginTests }}
};
array<std::shared_ptr<AgentPanel>> panels = array<std::shared_ptr<AgentPanel>>{ object{
    object::pair{std::string("name"), std::string("Trust Marketplace")}, 
    object::pair{std::string("path"), std::string("display")}, 
    object::pair{std::string("component"), std::string("LeaderboardPanelPage")}, 
    object::pair{std::string("icon"), std::string("UsersRound")}, 
    object::pair{std::string("public"), true}
} };

void Main(void)
{
}

MAIN
