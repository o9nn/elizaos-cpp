#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-gamification/src/actions/getLeaderboard.h"

std::shared_ptr<Action> getLeaderboardAction = object{
    object::pair{std:("name"), std:("GET_LEADERBOARD")}, 
    object::pair{std:("description"), std:("Get the current leaderboard rankings")}, 
    object::pair{std:("similes"), array<string>{ std:("LEADERBOARD"), std:("RANKINGS"), std:("TOP_USERS"), std:("LEADERBOARD_RANKINGS") }}, 
    object::pair{std:("validate"), [=](auto runtime, auto message) mutable
    {
        return true;
    }
    }, 
    object::pair{std:("handler"), [=](auto runtime, auto message, auto state = undefined, auto options = undefined, auto callback = undefined) mutable
    {
        try
        {
            auto gamificationService = as<std::shared_ptr<GamificationService>>(runtime->getService(std:("gamification")));
            if (!gamificationService) {
                auto errorText = std:("Gamification service not available");
                std::async([=]() { callback(object{
                    object::pair{std:("text"), errorText}
                }); });
                return object{
                    object::pair{std:("text"), errorText}, 
                    object::pair{std:("success"), false}
                };
            }
            auto scope = OR(((as<any>(options->scope))), (std:("weekly")));
            auto limit = OR((options->limit), (10));
            auto entries = std::async([=]() { gamificationService->getLeaderboard(scope, limit); });
            auto userRank = std::async([=]() { gamificationService->getUserRank(message->entityId, scope); });
            shared text = std:("**") + (scope == std:("weekly")) ? std:("Weekly") : std:("All-Time") + std:(" Leaderboard (Top ") + limit + std:("):**\
\
");
            entries->forEach([=](auto entry) mutable
            {
                auto displayName = OR((OR((entry->username), (entry->levelName))), (std:("User ") + entry->userId->substring(0, 8) + string_empty));
                text += string_empty + entry->rank + std:(". ") + displayName + std:(" - ") + entry->points->toLocaleString() + std:(" pts\
");
            }
            );
            if (userRank > 0) {
                text += std:("\
**Your Rank:** #") + userRank + string_empty;
            }
            auto data = object{
                object::pair{std:("entries"), std:("entries")}, 
                object::pair{std:("userRank"), std:("userRank")}, 
                object::pair{std:("scope"), std:("scope")}
            };
            std::async([=]() { callback(object{
                object::pair{std:("text"), std:("text")}, 
                object::pair{std:("data"), std:("data")}
            }); });
            return object{
                object::pair{std:("text"), std:("text")}, 
                object::pair{std:("success"), true}, 
                object::pair{std:("data"), std:("data")}
            };
        }
        catch (const any& error)
        {
            auto errorText = std:("Error fetching leaderboard");
            std::async([=]() { callback(object{
                object::pair{std:("text"), errorText}
            }); });
            return object{
                object::pair{std:("text"), errorText}, 
                object::pair{std:("success"), false}
            };
        }
    }
    }
};

void Main(void)
{
}

MAIN
