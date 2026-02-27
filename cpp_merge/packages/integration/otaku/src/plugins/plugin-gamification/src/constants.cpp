#include "constants.hpp"
#include <string>

Record<GamificationEventType, double> BASE_POINTS = object{
    object::pair{GamificationEventType::ACCOUNT_CREATION, 100}, 
    object::pair{GamificationEventType::DAILY_LOGIN_STREAK, 25}, 
    object::pair{GamificationEventType::MEANINGFUL_CHAT, 2}, 
    object::pair{GamificationEventType::SWAP_COMPLETED, 80}, 
    object::pair{GamificationEventType::BRIDGE_COMPLETED, 120}, 
    object::pair{GamificationEventType::TRANSFER_COMPLETED, 40}, 
    object::pair{GamificationEventType::DAILY_QUEST, 100}, 
    object::pair{GamificationEventType::REFERRAL_SIGNUP, 200}, 
    object::pair{GamificationEventType::REFERRAL_ACTIVATION, 300}, 
    object::pair{GamificationEventType::REFERRED_WELCOME, 50}, 
    object::pair{GamificationEventType::FIRST_CHAIN_BONUS, 50}, 
    object::pair{GamificationEventType::AGENT_ACTION, 10}
};
Partial<Record<GamificationEventType, double>> DAILY_CAPS = object{
    object::pair{GamificationEventType::MEANINGFUL_CHAT, 6}, 
    object::pair{GamificationEventType::SWAP_COMPLETED, Infinity}, 
    object::pair{GamificationEventType::BRIDGE_COMPLETED, Infinity}, 
    object::pair{GamificationEventType::AGENT_ACTION, Infinity}
};
object VOLUME_MULTIPLIERS = object{
    object::pair{std::string("SWAP"), object{
        object::pair{std::string("base"), 80}, 
        object::pair{std::string("perDollar"), 0.1}, 
        object::pair{std::string("cap"), 420}
    }}, 
    object::pair{std::string("BRIDGE"), object{
        object::pair{std::string("base"), 120}, 
        object::pair{std::string("perDollar"), 0.15}, 
        object::pair{std::string("cap"), 600}
    }}
};
array<object> LEVEL_THRESHOLDS = array<object>{ object{
    object::pair{std::string("level"), 0}, 
    object::pair{std::string("name"), std::string("Explorer")}, 
    object::pair{std::string("minPoints"), 0}, 
    object::pair{std::string("maxPoints"), 999}
}, object{
    object::pair{std::string("level"), 1}, 
    object::pair{std::string("name"), std::string("Strategist")}, 
    object::pair{std::string("minPoints"), 1000}, 
    object::pair{std::string("maxPoints"), 4999}
}, object{
    object::pair{std::string("level"), 2}, 
    object::pair{std::string("name"), std::string("Architect")}, 
    object::pair{std::string("minPoints"), 5000}, 
    object::pair{std::string("maxPoints"), 19999}
}, object{
    object::pair{std::string("level"), 3}, 
    object::pair{std::string("name"), std::string("Overlord")}, 
    object::pair{std::string("minPoints"), 20000}, 
    object::pair{std::string("maxPoints"), Infinity}
} };
double STREAK_BONUS_PER_DAY = 10;
double MAX_STREAK_BONUS = 70;
double MIN_TRANSFER_VALUE_USD = 25;
double MIN_CHAT_LENGTH = 25;
array<object> MESSAGE_LENGTH_TIERS = array<object>{ object{
    object::pair{std::string("minLength"), 25}, 
    object::pair{std::string("maxLength"), 74}, 
    object::pair{std::string("points"), 2}
}, object{
    object::pair{std::string("minLength"), 75}, 
    object::pair{std::string("maxLength"), 99}, 
    object::pair{std::string("points"), 3}
}, object{
    object::pair{std::string("minLength"), 100}, 
    object::pair{std::string("maxLength"), 199}, 
    object::pair{std::string("points"), 4}
}, object{
    object::pair{std::string("minLength"), 200}, 
    object::pair{std::string("maxLength"), Infinity}, 
    object::pair{std::string("points"), 5}
} };
double DAILY_QUEST_UNIQUE_ACTIONS_REQUIRED = 3;
double REFERRAL_ACTIVATION_WINDOW_DAYS = 7;

void Main(void)
{
}

MAIN
