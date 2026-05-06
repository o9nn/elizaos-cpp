#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-gamification/src/actions/getLeaderboard.h"

std::shared_ptr<Action> getLeaderboardAction = object{
    object::pair{std::string("name"), std::string("GET_LEADERBOARD")}, 
    object::pair{std::string("description"), std::string("Get the current leaderboard rankings")}, 
    object::pair{std::string("similes"), array<string>{ std::string("LEADERBOARD"), std::string("RANKINGS"), std::string("TOP_USERS"), std::string("LEADERBOARD_RANKINGS") }}, 
    object::pair{std::string("validate"), [=](auto runtime, auto message) mutable
    {
        return true;
    }
    }, 
    object::pair{std::string("handler"), [=](auto runtime, auto message, auto state = undefined, auto options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto gamificationService = as<std::shared_ptr<GamificationService>>(runtime->getService(std::string("gamification")));
            if (!gamificationService) {
                auto errorText = std::string("Gamification service not available");
                std::async([=]() { callback(object{
                    object::pair{std::string("text"), errorText}
                }); });
                return object{
                    object::pair{std::string("text"), errorText}, 
                    object::pair{std::string("success"), false}
                };
            }
            auto scope = OR(((as<any>(options->scope))), (std::string("weekly")));
            auto limit = OR((options->limit), (10));
            auto entries = std::async([=]() { gamificationService->getLeaderboard(scope, limit); });
            auto userRank = std::async([=]() { gamificationService->getUserRank(message->entityId, scope); });
            shared text = std::string("**") + (scope == std::string("weekly")) ? std::string("Weekly") : std::string("All-Time") + std::string(" Leaderboard (Top ") + limit + std::string("):**\
\
");
            entries->forEach([=](auto entry) mutable
            {
                auto displayName = OR((OR((entry->username), (entry->levelName))), (std::string("User ") + entry->userId->substring(0, 8) + string_empty));
                text += string_empty + entry->rank + std::string(". ") + displayName + std::string(" - ") + entry->points->toLocaleString() + std::string(" pts\
");
            }
            );
            if (userRank > 0) {
                text += std::string("\
**Your Rank:** #") + userRank + string_empty;
            }
            auto data = object{
                object::pair{std::string("entries"), std::string("entries")}, 
                object::pair{std::string("userRank"), std::string("userRank")}, 
                object::pair{std::string("scope"), std::string("scope")}
            };
            std::async([=]() { callback(object{
                object::pair{std::string("text"), std::string("text")}, 
                object::pair{std::string("data"), std::string("data")}
            }); });
            return object{
                object::pair{std::string("text"), std::string("text")}, 
                object::pair{std::string("success"), true}, 
                object::pair{std::string("data"), std::string("data")}
            };
        }
        catch (const any& error)
        {
            auto errorText = std::string("Error fetching leaderboard");
            std::async([=]() { callback(object{
                object::pair{std::string("text"), errorText}
            }); });
            return object{
                object::pair{std::string("text"), errorText}, 
                object::pair{std::string("success"), false}
            };
        }
    }
    }
};

void Main(void)
{
}

MAIN
