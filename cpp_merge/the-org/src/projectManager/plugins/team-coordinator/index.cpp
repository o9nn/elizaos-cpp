#include "index.hpp"
#include <string>

any initialize(std::shared_ptr<IAgentRuntime> runtime)
{
    std::make_shared<CheckInService>(runtime);
    return object{
        object::pair{std::string("actions"), array<any>{ recordCheckInAction, teamMemberUpdatesAction, listCheckInSchedules, generateReport, addTeamMemberAction, listTeamMembersAction, updatesFormatAction }}
    };
};


std::shared_ptr<Plugin> teamCoordinatorPlugin = object{
    object::pair{std::string("name"), std::string("team-coordinator")}, 
    object::pair{std::string("description"), std::string("Team Coordinator plugin for managing team activities")}, 
    object::pair{std::string("providers"), array<any>()}, 
    object::pair{std::string("actions"), array<any>{ teamMemberUpdatesAction, listCheckInSchedules, generateReport, recordCheckInAction, addTeamMemberAction, listTeamMembersAction, updatesFormatAction }}, 
    object::pair{std::string("init"), [=](auto config, auto runtime) mutable
    {
        try
        {
            logger->info(std::string("Initializing Team Coordinator plugin..."));
            logger->info(std::string("Registering TeamUpdateTrackerService..."));
            std::async([=]() { runtime->registerService(TeamUpdateTrackerService); });
            logger->info(std::string("Scheduling team coordinator tasks registration..."));
            auto registerTasksWithRetry = [=](auto retries = 10, auto delay = 1000) mutable
            {
                for (auto i = 0; i < retries; i++)
                {
                    try
                    {
                        if (AND((runtime->getTasks), (type_of(runtime->getTasks) == std::string("function")))) {
                            logger->info(std::string("Runtime is ready, registering team coordinator tasks..."));
                            std::async([=]() { registerTasks(runtime); });
                            logger->info(std::string("Team coordinator tasks registered successfully"));
                            return std::shared_ptr<Promise<void>>();
                        } else {
                            logger->info(std::string("Runtime not ready yet, retrying in ") + delay + std::string("ms... (attempt ") + (i + 1) + std::string("/") + retries + std::string(")"));
                        }
                    }
                    catch (const any& error)
                    {
                        logger->warn(std::string("Failed to register tasks (attempt ") + (i + 1) + std::string("/") + retries + std::string("):"), error);
                    }
                    std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                    {
                        return setTimeout(resolve, delay);
                    }
                    ); });
                }
                logger->error(std::string("Failed to register team coordinator tasks after all retries"));
            };
            registerTasksWithRetry()->_catch([=](auto error) mutable
            {
                logger->error(std::string("Error in registerTasksWithRetry:"), error);
            }
            );
            logger->info(std::string("Team Coordinator plugin initialized successfully"));
        }
        catch (const any& error)
        {
            logger->error(std::string("Failed to initialize Team Coordinator plugin:"), error);
            throw any(error);
        }
    }
    }, 
    object::pair{std::string("services"), array<TeamUpdateTrackerService>{ TeamUpdateTrackerService, CheckInService }}
};

void Main(void)
{
}

MAIN
