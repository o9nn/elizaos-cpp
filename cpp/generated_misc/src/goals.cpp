#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-goals/src/providers/goals.h"

std::shared_ptr<Provider> goalsProvider = object{
    object::pair{std::string("name"), std::string("GOALS")}, 
    object::pair{std::string("description"), std::string("Provides information about active goals and recent achievements")}, 
    object::pair{std::string("dynamic"), true}, 
    object::pair{std::string("get"), [=](auto runtime, auto message, auto _state = undefined) mutable
    {
        auto dataService = as<std::shared_ptr<GoalService>>(runtime->getService(std::string("goals")));
        if (!dataService) {
            logger->warn(std::string("[GOALS Provider] Goals service not available"));
            return object{
                object::pair{std::string("text"), std::string("No goals information available at this time.")}, 
                object::pair{std::string("data"), object{}}, 
                object::pair{std::string("values"), object{}}
            };
        }
        auto ownerType = std::string("agent");
        auto ownerId = runtime->agentId;
        if (AND((message->entityId), (message->entityId != runtime->agentId))) {
            ownerType = std::string("entity");
            ownerId = message->entityId;
        }
        auto activeGoals = std::async([=]() { dataService->getGoals(object{
            object::pair{std::string("ownerType"), std::string("ownerType")}, 
            object::pair{std::string("ownerId"), std::string("ownerId")}, 
            object::pair{std::string("isCompleted"), false}
        }); });
        auto completedGoals = std::async([=]() { dataService->getGoals(object{
            object::pair{std::string("ownerType"), std::string("ownerType")}, 
            object::pair{std::string("ownerId"), std::string("ownerId")}, 
            object::pair{std::string("isCompleted"), true}
        }); });
        auto recentCompleted = completedGoals->sort([=](auto a, auto b) mutable
        {
            return (OR((b->completedAt->getTime()), (0))) - (OR((a->completedAt->getTime()), (0)));
        }
        )->slice(0, 5);
        shared output = string_empty;
        if (activeGoals->get_length() > 0) {
            output += std::string("## Active Goals\
");
            activeGoals->forEach([=](auto goal) mutable
            {
                auto tags = (AND((goal->tags), (goal->tags->get_length() > 0))) ? any(std::string(" [") + goal->tags->join(std::string(", ")) + std::string("]")) : any(string_empty);
                output += std::string("- ") + goal->name + string_empty + tags + string_empty;
                if (goal->description) {
                    output += std::string(" - ") + goal->description + string_empty;
                }
                output += std::string("\
");
            }
            );
            output += std::string("\
");
        }
        if (recentCompleted->get_length() > 0) {
            output += std::string("## Recently Completed Goals\
");
            recentCompleted->forEach([=](auto goal) mutable
            {
                auto completedDate = (goal->completedAt) ? any(((std::make_shared<Date>(goal->completedAt)))->toLocaleDateString()) : any(std::string("Unknown date"));
                output += std::string("- ") + goal->name + std::string(" (completed ") + completedDate + std::string(")\
");
            }
            );
            output += std::string("\
");
        }
        auto totalActive = activeGoals->get_length();
        auto totalCompleted = completedGoals->get_length();
        output += std::string("## Summary\
");
        output += std::string("- Active goals: ") + totalActive + std::string("\
");
        output += std::string("- Completed goals: ") + totalCompleted + std::string("\
");
        if (AND((activeGoals->get_length() == 0), (completedGoals->get_length() == 0))) {
            output = std::string("No goals have been set yet. Consider creating some goals to track progress!");
        }
        return object{
            object::pair{std::string("text"), output->trim()}, 
            object::pair{std::string("data"), object{
                object::pair{std::string("activeGoals"), activeGoals->map([=](auto g) mutable
                {
                    return (object{
                        object::pair{std::string("id"), g->id}, 
                        object::pair{std::string("name"), g->name}, 
                        object::pair{std::string("tags"), OR((g->tags), (array<any>()))}
                    });
                }
                )}, 
                object::pair{std::string("recentCompleted"), recentCompleted->map([=](auto g) mutable
                {
                    return (object{
                        object::pair{std::string("id"), g->id}, 
                        object::pair{std::string("name"), g->name}, 
                        object::pair{std::string("completedAt"), g->completedAt}
                    });
                }
                )}, 
                object::pair{std::string("totalActive"), std::string("totalActive")}, 
                object::pair{std::string("totalCompleted"), std::string("totalCompleted")}
            }}, 
            object::pair{std::string("values"), object{
                object::pair{std::string("activeGoalCount"), totalActive->toString()}, 
                object::pair{std::string("completedGoalCount"), totalCompleted->toString()}
            }}
        };
    }
    }
};

void Main(void)
{
}

MAIN
