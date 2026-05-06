#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-gamification/src/providers/pointsProvider.h"

std::shared_ptr<Provider> pointsProvider = object{
    object::pair{std::string("name"), std::string("USER_POINTS")}, 
    object::pair{std::string("description"), std::string("Provides current user points, level, and streak for agent awareness")}, 
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
            auto summary = std::async([=]() { gamificationService->getUserSummary(message->entityId); });
            return object{
                object::pair{std::string("text"), std::string("User has ") + summary->allTimePoints->toLocaleString() + std::string(" total points (Level: ") + summary->levelName + std::string(", Streak: ") + summary->streakDays + std::string(" days)")}, 
                object::pair{std::string("values"), object{
                    object::pair{std::string("points"), summary->allTimePoints}, 
                    object::pair{std::string("weeklyPoints"), summary->weeklyPoints}, 
                    object::pair{std::string("level"), summary->level}, 
                    object::pair{std::string("levelName"), summary->levelName}, 
                    object::pair{std::string("streak"), summary->streakDays}
                }}, 
                object::pair{std::string("data"), object{
                    object::pair{std::string("points"), summary->allTimePoints}, 
                    object::pair{std::string("weeklyPoints"), summary->weeklyPoints}, 
                    object::pair{std::string("level"), summary->level}, 
                    object::pair{std::string("levelName"), summary->levelName}, 
                    object::pair{std::string("streak"), summary->streakDays}
                }}
            };
        }
        catch (const any& error)
        {
            return object{
                object::pair{std::string("text"), std::string("Unable to fetch user points")}, 
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
