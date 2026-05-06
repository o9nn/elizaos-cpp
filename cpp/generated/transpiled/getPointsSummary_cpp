#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-gamification/src/actions/getPointsSummary.h"

std::shared_ptr<Action> getPointsSummaryAction = object{
    object::pair{std::string("name"), std::string("GET_POINTS_SUMMARY")}, 
    object::pair{std::string("description"), std::string("Get the user's current points, level, streak, and recent awards")}, 
    object::pair{std::string("similes"), array<string>{ std::string("CHECK_POINTS"), std::string("MY_POINTS"), std::string("POINTS_BALANCE"), std::string("SHOW_LEVEL") }}, 
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
            auto summary = std::async([=]() { gamificationService->getUserSummary(message->entityId); });
            auto text = std::string("**Your Points Summary**\
- **Total Points:** ") + summary->allTimePoints->toLocaleString() + std::string("\
- **This Week:** ") + summary->weeklyPoints->toLocaleString() + std::string("\
- **Level:** ") + summary->levelName + std::string(" (") + summary->level + std::string(")\
- **Daily Streak:** ") + summary->streakDays + std::string(" days") + (summary->nextMilestone) ? any(std::string("\
- **Next Milestone:** ") + summary->nextMilestone["pointsNeeded"]->toLocaleString() + std::string(" points to ") + summary->nextMilestone["levelName"] + string_empty) : any(string_empty) + string_empty;
            std::async([=]() { callback(object{
                object::pair{std::string("text"), std::string("text")}, 
                object::pair{std::string("data"), summary}
            }); });
            return object{
                object::pair{std::string("text"), std::string("text")}, 
                object::pair{std::string("success"), true}, 
                object::pair{std::string("data"), summary}
            };
        }
        catch (const any& error)
        {
            auto errorText = std::string("Error fetching points summary");
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
