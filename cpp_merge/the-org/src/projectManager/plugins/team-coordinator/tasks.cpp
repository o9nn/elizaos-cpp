#include "tasks.hpp"

std::function<std::shared_ptr<Promise<void>>(any, any)> registerTasks = [=](auto runtime, auto initialWorldId = undefined) mutable
{
    auto worldId = OR((initialWorldId), (as<std::shared_ptr<UUID>>(runtime->agentId)));
    shared<std::shared_ptr<TeamUpdateTrackerService>> teamUpdateService;
    try
    {
        auto existingService = runtime->getService(TeamUpdateTrackerService::serviceType);
        if (existingService) {
            logger->info(std:("Using existing TeamUpdateTrackerService"));
            teamUpdateService = as<std::shared_ptr<TeamUpdateTrackerService>>(existingService);
        } else {
            logger->info(std:("Creating new TeamUpdateTrackerService instance"));
            teamUpdateService = std::make_shared<TeamUpdateTrackerService>(runtime);
        }
    }
    catch (const any& error)
    {
        logger->warn(std:("Error getting existing service, creating new instance:"), error);
        teamUpdateService = std::make_shared<TeamUpdateTrackerService>(runtime);
    }
    auto tasks = std::async([=]() { runtime->getTasks(object{
        object::pair{std:("tags"), array<string>{ std:("queue"), std:("repeat"), std:("team_coordinator") }}
    }); });
    for (auto& task : tasks)
    {
        if (task->id) {
            std::async([=]() { runtime->deleteTask(task->id); });
        }
    }
    runtime->registerTaskWorker(object{
        object::pair{std:("name"), std:("TEAM_CHECK_IN_SERVICE")}, 
        object::pair{std:("validate"), [=](auto _runtime, auto _message, auto _state) mutable
        {
            return true;
        }
        }, 
        object::pair{std:("execute"), [=](auto runtime, auto _options, auto task) mutable
        {
            try
            {
                logger->info(std:("Running team check-in service job"));
                std::async([=]() { teamUpdateService->checkInServiceJob(); });
            }
            catch (const any& error)
            {
                logger->error(std:("Failed to run check-in service job:"), error);
            }
        }
        }
    });
    runtime->createTask(object{
        object::pair{std:("name"), std:("TEAM_CHECK_IN_SERVICE")}, 
        object::pair{std:("description"), std:("Regular team check-in service job")}, 
        object::pair{std:("worldId"), worldId}, 
        object::pair{std:("metadata"), object{
            object::pair{std:("updatedAt"), Date->now()}, 
            object::pair{std:("updateInterval"), 1000 * 60}
        }}, 
        object::pair{std:("tags"), array<string>{ std:("queue"), std:("repeat"), std:("team_coordinator") }}
    });
};

void Main(void)
{
}

MAIN
