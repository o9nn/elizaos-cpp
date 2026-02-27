#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otaku/src/plugins/plugin-gamification/src/actions/getPointsSummary.h"

std::shared_ptr<Action> getPointsSummaryAction = object{
    object::pair{std:("name"), std:("GET_POINTS_SUMMARY")}, 
    object::pair{std:("description"), std:("Get the user's current points, level, streak, and recent awards")}, 
    object::pair{std:("similes"), array<string>{ std:("CHECK_POINTS"), std:("MY_POINTS"), std:("POINTS_BALANCE"), std:("SHOW_LEVEL") }}, 
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
            auto summary = std::async([=]() { gamificationService->getUserSummary(message->entityId); });
            auto text = std:("**Your Points Summary**\
- **Total Points:** ") + summary->allTimePoints->toLocaleString() + std:("\
- **This Week:** ") + summary->weeklyPoints->toLocaleString() + std:("\
- **Level:** ") + summary->levelName + std:(" (") + summary->level + std:(")\
- **Daily Streak:** ") + summary->streakDays + std:(" days") + (summary->nextMilestone) ? any(std:("\
- **Next Milestone:** ") + summary->nextMilestone["pointsNeeded"]->toLocaleString() + std:(" points to ") + summary->nextMilestone["levelName"] + string_empty) (string_empty) + string_empty;
            std::async([=]() { callback(object{
                object::pair{std:("text"), std:("text")}, 
                object::pair{std:("data"), summary}
            }); });
            return object{
                object::pair{std:("text"), std:("text")}, 
                object::pair{std:("success"), true}, 
                object::pair{std:("data"), summary}
            };
        }
        catch (const any& error)
        {
            auto errorText = std:("Error fetching points summary");
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
