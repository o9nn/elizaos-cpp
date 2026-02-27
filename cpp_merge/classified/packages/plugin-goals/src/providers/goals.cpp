#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-goals/src/providers/goals.h"

std::shared_ptr<Provider> goalsProvider = object{
    object::pair{std:("name"), std:("GOALS")}, 
    object::pair{std:("description"), std:("Provides information about active goals and recent achievements")}, 
    object::pair{std:("dynamic"), true}, 
    object::pair{std:("get"), [=](auto runtime, auto message, auto _state = undefined) mutable
    {
        auto dataService = as<std::shared_ptr<GoalService>>(runtime->getService(std:("goals")));
        if (!dataService) {
            logger->warn(std:("[GOALS Provider] Goals service not available"));
            return object{
                object::pair{std:("text"), std:("No goals information available at this time.")}, 
                object::pair{std:("data"), object{}}, 
                object::pair{std:("values"), object{}}
            };
        }
        auto ownerType = std:("agent");
        auto ownerId = runtime->agentId;
        if (AND((message->entityId), (message->entityId != runtime->agentId))) {
            ownerType = std:("entity");
            ownerId = message->entityId;
        }
        auto activeGoals = std::async([=]() { dataService->getGoals(object{
            object::pair{std:("ownerType"), std:("ownerType")}, 
            object::pair{std:("ownerId"), std:("ownerId")}, 
            object::pair{std:("isCompleted"), false}
        }); });
        auto completedGoals = std::async([=]() { dataService->getGoals(object{
            object::pair{std:("ownerType"), std:("ownerType")}, 
            object::pair{std:("ownerId"), std:("ownerId")}, 
            object::pair{std:("isCompleted"), true}
        }); });
        auto recentCompleted = completedGoals->sort([=](auto a, auto b) mutable
        {
            return (OR((b->completedAt->getTime()), (0))) - (OR((a->completedAt->getTime()), (0)));
        }
        )->slice(0, 5);
        shared output = string_empty;
        if (activeGoals->get_length() > 0) {
            output += std:("## Active Goals\
");
            activeGoals->forEach([=](auto goal) mutable
            {
                auto tags = (AND((goal->tags), (goal->tags->get_length() > 0))) ? any(std:(" [") + goal->tags->join(std:(", ")) + std:("]")) (string_empty);
                output += std:("- ") + goal->name + string_empty + tags + string_empty;
                if (goal->description) {
                    output += std:(" - ") + goal->description + string_empty;
                }
                output += std:("\
");
            }
            );
            output += std:("\
");
        }
        if (recentCompleted->get_length() > 0) {
            output += std:("## Recently Completed Goals\
");
            recentCompleted->forEach([=](auto goal) mutable
            {
                auto completedDate = (goal->completedAt) ? any(((std::make_shared<Date>(goal->completedAt)))->toLocaleDateString()) (std:("Unknown date"));
                output += std:("- ") + goal->name + std:(" (completed ") + completedDate + std:(")\
");
            }
            );
            output += std:("\
");
        }
        auto totalActive = activeGoals->get_length();
        auto totalCompleted = completedGoals->get_length();
        output += std:("## Summary\
");
        output += std:("- Active goals: ") + totalActive + std:("\
");
        output += std:("- Completed goals: ") + totalCompleted + std:("\
");
        if (AND((activeGoals->get_length() == 0), (completedGoals->get_length() == 0))) {
            output = std:("No goals have been set yet. Consider creating some goals to track progress!");
        }
        return object{
            object::pair{std:("text"), output->trim()}, 
            object::pair{std:("data"), object{
                object::pair{std:("activeGoals"), activeGoals->map([=](auto g) mutable
                {
                    return (object{
                        object::pair{std:("id"), g->id}, 
                        object::pair{std:("name"), g->name}, 
                        object::pair{std:("tags"), OR((g->tags), (array<any>()))}
                    });
                }
                )}, 
                object::pair{std:("recentCompleted"), recentCompleted->map([=](auto g) mutable
                {
                    return (object{
                        object::pair{std:("id"), g->id}, 
                        object::pair{std:("name"), g->name}, 
                        object::pair{std:("completedAt"), g->completedAt}
                    });
                }
                )}, 
                object::pair{std:("totalActive"), std:("totalActive")}, 
                object::pair{std:("totalCompleted"), std:("totalCompleted")}
            }}, 
            object::pair{std:("values"), object{
                object::pair{std:("activeGoalCount"), totalActive->toString()}, 
                object::pair{std:("completedGoalCount"), totalCompleted->toString()}
            }}
        };
    }
    }
};

void Main(void)
{
}

MAIN
