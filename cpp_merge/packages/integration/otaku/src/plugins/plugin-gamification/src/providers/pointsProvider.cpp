#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-gamification/src/providers/pointsProvider.h"

std::shared_ptr<Provider> pointsProvider = object{
    object::pair{std:("name"), std:("USER_POINTS")}, 
    object::pair{std:("description"), std:("Provides current user points, level, and streak for agent awareness")}, 
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
            auto summary = std::async([=]() { gamificationService->getUserSummary(message->entityId); });
            return object{
                object::pair{std:("text"), std:("User has ") + summary->allTimePoints->toLocaleString() + std:(" total points (Level: ") + summary->levelName + std:(", Streak: ") + summary->streakDays + std:(" days)")}, 
                object::pair{std:("values"), object{
                    object::pair{std:("points"), summary->allTimePoints}, 
                    object::pair{std:("weeklyPoints"), summary->weeklyPoints}, 
                    object::pair{std:("level"), summary->level}, 
                    object::pair{std:("levelName"), summary->levelName}, 
                    object::pair{std:("streak"), summary->streakDays}
                }}, 
                object::pair{std:("data"), object{
                    object::pair{std:("points"), summary->allTimePoints}, 
                    object::pair{std:("weeklyPoints"), summary->weeklyPoints}, 
                    object::pair{std:("level"), summary->level}, 
                    object::pair{std:("levelName"), summary->levelName}, 
                    object::pair{std:("streak"), summary->streakDays}
                }}
            };
        }
        catch (const any& error)
        {
            return object{
                object::pair{std:("text"), std:("Unable to fetch user points")}, 
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
