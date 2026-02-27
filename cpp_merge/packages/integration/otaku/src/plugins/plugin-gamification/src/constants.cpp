#include "constants.hpp"

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
    object::pair{std:("SWAP"), object{
        object::pair{std:("base"), 80}, 
        object::pair{std:("perDollar"), 0.1}, 
        object::pair{std:("cap"), 420}
    }}, 
    object::pair{std:("BRIDGE"), object{
        object::pair{std:("base"), 120}, 
        object::pair{std:("perDollar"), 0.15}, 
        object::pair{std:("cap"), 600}
    }}
};
array<object> LEVEL_THRESHOLDS = array<object>{ object{
    object::pair{std:("level"), 0}, 
    object::pair{std:("name"), std:("Explorer")}, 
    object::pair{std:("minPoints"), 0}, 
    object::pair{std:("maxPoints"), 999}
}, object{
    object::pair{std:("level"), 1}, 
    object::pair{std:("name"), std:("Strategist")}, 
    object::pair{std:("minPoints"), 1000}, 
    object::pair{std:("maxPoints"), 4999}
}, object{
    object::pair{std:("level"), 2}, 
    object::pair{std:("name"), std:("Architect")}, 
    object::pair{std:("minPoints"), 5000}, 
    object::pair{std:("maxPoints"), 19999}
}, object{
    object::pair{std:("level"), 3}, 
    object::pair{std:("name"), std:("Overlord")}, 
    object::pair{std:("minPoints"), 20000}, 
    object::pair{std:("maxPoints"), Infinity}
} };
double STREAK_BONUS_PER_DAY = 10;
double MAX_STREAK_BONUS = 70;
double MIN_TRANSFER_VALUE_USD = 25;
double MIN_CHAT_LENGTH = 25;
array<object> MESSAGE_LENGTH_TIERS = array<object>{ object{
    object::pair{std:("minLength"), 25}, 
    object::pair{std:("maxLength"), 74}, 
    object::pair{std:("points"), 2}
}, object{
    object::pair{std:("minLength"), 75}, 
    object::pair{std:("maxLength"), 99}, 
    object::pair{std:("points"), 3}
}, object{
    object::pair{std:("minLength"), 100}, 
    object::pair{std:("maxLength"), 199}, 
    object::pair{std:("points"), 4}
}, object{
    object::pair{std:("minLength"), 200}, 
    object::pair{std:("maxLength"), Infinity}, 
    object::pair{std:("points"), 5}
} };
double DAILY_QUEST_UNIQUE_ACTIONS_REQUIRED = 3;
double REFERRAL_ACTIVATION_WINDOW_DAYS = 7;

void Main(void)
{
}

MAIN
