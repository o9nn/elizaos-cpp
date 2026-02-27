#include "index.hpp"

any initialize(std::shared_ptr<IAgentRuntime> runtime)
{
    std::make_shared<CheckInService>(runtime);
    return object{
        object::pair{std:("actions"), array<any>{ recordCheckInAction, teamMemberUpdatesAction, listCheckInSchedules, generateReport, addTeamMemberAction, listTeamMembersAction, updatesFormatAction }}
    };
};


std::shared_ptr<Plugin> teamCoordinatorPlugin = object{
    object::pair{std:("name"), std:("team-coordinator")}, 
    object::pair{std:("description"), std:("Team Coordinator plugin for managing team activities")}, 
    object::pair{std:("providers"), array<any>()}, 
    object::pair{std:("actions"), array<any>{ teamMemberUpdatesAction, listCheckInSchedules, generateReport, recordCheckInAction, addTeamMemberAction, listTeamMembersAction, updatesFormatAction }}, 
    object::pair{std:("init"), [=](auto config, auto runtime) mutable
    {
        try
        {
            logger->info(std:("Initializing Team Coordinator plugin..."));
            logger->info(std:("Registering TeamUpdateTrackerService..."));
            std::async([=]() { runtime->registerService(TeamUpdateTrackerService); });
            logger->info(std:("Scheduling team coordinator tasks registration..."));
            auto registerTasksWithRetry = [=](auto retries = 10, auto delay = 1000) mutable
            {
                for (auto i = 0; i < retries; i++)
                {
                    try
                    {
                        if (AND((runtime->getTasks), (type_of(runtime->getTasks) == std:("function")))) {
                            logger->info(std:("Runtime is ready, registering team coordinator tasks..."));
                            std::async([=]() { registerTasks(runtime); });
                            logger->info(std:("Team coordinator tasks registered successfully"));
                            return std::shared_ptr<Promise<void>>();
                        } else {
                            logger->info(std:("Runtime not ready yet, retrying in ") + delay + std:("ms... (attempt ") + (i + 1) + std:("/") + retries + std:(")"));
                        }
                    }
                    catch (const any& error)
                    {
                        logger->warn(std:("Failed to register tasks (attempt ") + (i + 1) + std:("/") + retries + std:("):"), error);
                    }
                    std::async([=]() { std::make_shared<Promise>([=](auto resolve) mutable
                    {
                        return setTimeout(resolve, delay);
                    }
                    ); });
                }
                logger->error(std:("Failed to register team coordinator tasks after all retries"));
            };
            registerTasksWithRetry()->_catch([=](auto error) mutable
            {
                logger->error(std:("Error in registerTasksWithRetry:"), error);
            }
            );
            logger->info(std:("Team Coordinator plugin initialized successfully"));
        }
        catch (const any& error)
        {
            logger->error(std:("Failed to initialize Team Coordinator plugin:"), error);
            throw any(error);
        }
    }
    }, 
    object::pair{std:("services"), array<TeamUpdateTrackerService>{ TeamUpdateTrackerService, CheckInService }}
};

void Main(void)
{
}

MAIN
