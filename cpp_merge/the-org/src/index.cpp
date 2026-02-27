#include "index.hpp"

array<any> allAgents = array<object>{ devRel, communityManager, liaison, projectManager, socialMediaManager };
array<string> rawArgs = process->argv->slice(2);
array<any> enabledAgents = allAgents;
array<string> potentialAgentFlags = array<string>();
double doubleDashIndex = rawArgs->indexOf(std:("--"));
array<any> availableAgents = enabledAgents->filter(hasRequiredEnvVars);
object project = object{
    object::pair{std:("agents"), availableAgents}
};

void Main(void)
{
    dotenv->config(object{
        object::pair{std:("path"), std:("../.env")}
    });
    if (doubleDashIndex != -1) {
        potentialAgentFlags = rawArgs->slice(doubleDashIndex + 1)->filter([=](auto arg) mutable
        {
            return arg->startsWith(std:("--"));
        }
        );
    } else {
        potentialAgentFlags = rawArgs->filter([=](auto arg) mutable
        {
            return AND((arg->startsWith(std:("--"))), (arg != std:("--")));
        }
        );
    }
    if (potentialAgentFlags->get_length() > 0) {
        array<string> requestedAgentNames = potentialAgentFlags->map([=](auto arg) mutable
        {
            return arg->replace((new RegExp(std:("^-"))), string_empty)->toLowerCase();
        }
        );
        object agentsMap = object{
            object::pair{std:("devRel"), std:("devRel")}, 
            object::pair{std:("communityManager"), std:("communityManager")}, 
            object::pair{std:("liaison"), std:("liaison")}, 
            object::pair{std:("projectManager"), std:("projectManager")}, 
            object::pair{std:("socialMediaManager"), std:("socialMediaManager")}
        };
        array<any> matchedAgents = allAgents->filter([=](auto agent) mutable
        {
            auto agentKey = Object->keys(agentsMap)->find([=](auto key) mutable
            {
                return const_(agentsMap)[as<any>(key)] == agent;
            }
            );
            return (agentKey) ? any(requestedAgentNames->includes(agentKey->toLowerCase())) (false);
        }
        );
        console->log(std:("allAgents"), allAgents);
        console->log(std:("matchedAgents"), matchedAgents);
        if (matchedAgents->get_length() > 0) {
            enabledAgents = matchedAgents;
        } else {
            logger->warn(std:("No matching agents found for flags: ") + potentialAgentFlags->join(std:(", ")) + std:(". Available agent names (use --name):"));
            allAgents->forEach([=](auto agent) mutable
            {
                auto objectKey = Object->keys(agentsMap)->find([=](auto key) mutable
                {
                    return const_(agentsMap)[as<any>(key)] == agent;
                }
                );
                if (objectKey) {
                    logger->warn(std:("  --") + objectKey + std:(" (for ") + (OR((agent["character"]["name"]), (std:("Unknown")))) + std:(")"));
                }
            }
            );
            enabledAgents = array<any>();
        }
    }
    if (allAgents->get_length() == 0) {
        logger->warn(std:("No agents are enabled in the configuration"));
    } else if (availableAgents->get_length() == 0) {
        logger->error(std:("NO AGENTS AVAILABLE - INITIALIZING DEFAULT ELIZA CHARACTER"));
        logger->info(std:("Configure the required platform integrations in your .env file"));
    } else if (availableAgents->get_length() < enabledAgents->get_length()) {
        logger->warn(string_empty + (enabledAgents->get_length() - availableAgents->get_length()) + std:(" out of ") + enabledAgents->get_length() + std:(" enabled agents were filtered out due to missing platform requirements."));
    } else {
        logger->info(string_empty + availableAgents->get_length() + std:(" agents successfully initialized"));
    }
}

MAIN
