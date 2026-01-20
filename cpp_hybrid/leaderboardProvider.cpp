#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-gamification/src/providers/leaderboardProvider.h"

std::shared_ptr<Provider> leaderboardProvider = object{
    object::pair{std::string("name"), std::string("LEADERBOARD")}, 
    object::pair{std::string("description"), std::string("Provides top users for leaderboard awareness")}, 
    object::pair{std::string("dynamic"), true}, 
    object::pair{std::string("get"), [=](auto runtime, auto message, auto state = undefined) mutable
    {
        try
        {
            auto gamificationService = as<std::shared_ptr<GamificationService>>(runtime->getService(std::string("gamification")));
            if (!gamificationService) {
                return object{
                    object::pair{std::string("text"), std::string("Gamification service not available")}, 
                    object::pair{std::string("values"), object{}}
                };
            }
            auto topUsers = std::async([=]() { gamificationService->getLeaderboard(std::string("weekly"), 5); });
            auto userRank = std::async([=]() { gamificationService->getUserRank(message->entityId, std::string("weekly")); });
            return object{
                object::pair{std::string("text"), std::string("Current weekly leaderboard top 5. User's rank: #") + userRank + string_empty}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("topUsers"), std::string("topUsers")}, 
                    object::pair{std::string("userRank"), std::string("userRank")}
                }}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("topUsers"), topUsers->map([=](auto user) mutable
                    {
                        return (object{
                            object::pair{std::string("userId"), user->userId}, 
                            object::pair{std::string("points"), user->points}, 
                            object::pair{std::string("level"), user->level}, 
                            object::pair{std::string("levelName"), user->levelName}
                        });
                    }
                    )}, 
                    object::pair{std::string("userRank"), std::string("userRank")}
                }}
            };
        }
        catch (const any& error)
        {
            return object{
                object::pair{std::string("text"), std::string("Unable to fetch leaderboard")}, 
                object::pair{std::string("values"), object{}}
            };
        }
    }
    }
};

void Main(void)
{
}

MAIN
