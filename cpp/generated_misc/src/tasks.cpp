#include "/home/runner/work/elizaos-cpp/elizaos-cpp/the-org/src/projectManager/plugins/team-coordinator/tasks.h"

std::function<std::shared_ptr<Promise<void>>(any, any)> registerTasks = [=](auto runtime, auto initialWorldId = undefined) mutable
{
    auto worldId = OR((initialWorldId), (as<std::shared_ptr<UUID>>(runtime->agentId)));
    shared<std::shared_ptr<TeamUpdateTrackerService>> teamUpdateService;
    try
    {
        auto existingService = runtime->getService(TeamUpdateTrackerService::serviceType);
        if (existingService) {
            logger->info(std::string("Using existing TeamUpdateTrackerService"));
            teamUpdateService = as<std::shared_ptr<TeamUpdateTrackerService>>(existingService);
        } else {
            logger->info(std::string("Creating new TeamUpdateTrackerService instance"));
            teamUpdateService = std::make_shared<TeamUpdateTrackerService>(runtime);
        }
    }
    catch (const any& error)
    {
        logger->warn(std::string("Error getting existing service, creating new instance:"), error);
        teamUpdateService = std::make_shared<TeamUpdateTrackerService>(runtime);
    }
    auto tasks = std::async([=]() { runtime->getTasks(object{
        object::pair{std::string("tags"), array<string>{ std::string("queue"), std::string("repeat"), std::string("team_coordinator") }}
    }); });
    for (auto& task : tasks)
    {
        if (task->id) {
            std::async([=]() { runtime->deleteTask(task->id); });
        }
    }
    runtime->registerTaskWorker(object{
        object::pair{std::string("name"), std::string("TEAM_CHECK_IN_SERVICE")}, 
        object::pair{std::string("validate"), [=](auto _runtime, auto _message, auto _state) mutable
        {
            return true;
        }
        }, 
        object::pair{std::string("execute"), [=](auto runtime, auto _options, auto task) mutable
        {
            try
            {
                logger->info(std::string("Running team check-in service job"));
                std::async([=]() { teamUpdateService->checkInServiceJob(); });
            }
            catch (const any& error)
            {
                logger->error(std::string("Failed to run check-in service job:"), error);
            }
        }
        }
    });
    runtime->createTask(object{
        object::pair{std::string("name"), std::string("TEAM_CHECK_IN_SERVICE")}, 
        object::pair{std::string("description"), std::string("Regular team check-in service job")}, 
        object::pair{std::string("worldId"), worldId}, 
        object::pair{std::string("metadata"), object{
            object::pair{std::string("updatedAt"), Date->now()}, 
            object::pair{std::string("updateInterval"), 1000 * 60}
        }}, 
        object::pair{std::string("tags"), array<string>{ std::string("queue"), std::string("repeat"), std::string("team_coordinator") }}
    });
};

void Main(void)
{
}

MAIN
