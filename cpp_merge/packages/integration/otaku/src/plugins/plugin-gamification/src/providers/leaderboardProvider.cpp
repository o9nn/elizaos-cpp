#include "leaderboardProvider.hpp"

std::shared_ptr<Provider> leaderboardProvider = object{
    object::pair{std:("name"), std:("LEADERBOARD")}, 
    object::pair{std:("description"), std:("Provides top users for leaderboard awareness")}, 
    object::pair{std:("dynamic"), true}, 
    object::pair{std:("get"), [=](auto runtime, auto message, auto state = undefined) mutable
    {
        try
        {
            auto gamificationService = as<std::shared_ptr<GamificationService>>(runtime->getService(std:("gamification")));
            if (!gamificationService) {
                return object{
                    object::pair{std:("text"), std:("Gamification service not available")}, 
                    object::pair{std:("values"), object{}}
                };
            }
            auto topUsers = std::async([=]() { gamificationService->getLeaderboard(std:("weekly"), 5); });
            auto userRank = std::async([=]() { gamificationService->getUserRank(message->entityId, std:("weekly")); });
            return object{
                object::pair{std:("text"), std:("Current weekly leaderboard top 5. User's rank: #") + userRank + string_empty}, 
                object::pair{std:("values"), object{
                    object::pair{std:("topUsers"), std:("topUsers")}, 
                    object::pair{std:("userRank"), std:("userRank")}
                }}, 
                object::pair{std:("data"), object{
                    object::pair{std:("topUsers"), topUsers->map([=](auto user) mutable
                    {
                        return (object{
                            object::pair{std:("userId"), user->userId}, 
                            object::pair{std:("points"), user->points}, 
                            object::pair{std:("level"), user->level}, 
                            object::pair{std:("levelName"), user->levelName}
                        });
                    }
                    )}, 
                    object::pair{std:("userRank"), std:("userRank")}
                }}
            };
        }
        catch (const any& error)
        {
            return object{
                object::pair{std:("text"), std:("Unable to fetch leaderboard")}, 
                object::pair{std:("values"), object{}}
            };
        }
    }
    }
};

void Main(void)
{
}

MAIN
