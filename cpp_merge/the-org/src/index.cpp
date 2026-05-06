#include "index.hpp"
#include <string>

array<any> allAgents = array<object>{ devRel, communityManager, liaison, projectManager, socialMediaManager };
array<string> rawArgs = process->argv->slice(2);
array<any> enabledAgents = allAgents;
array<string> potentialAgentFlags = array<string>();
double doubleDashIndex = rawArgs->indexOf(std::string("--"));
array<any> availableAgents = enabledAgents->filter(hasRequiredEnvVars);
object project = object{
    object::pair{std::string("agents"), availableAgents}
};

void Main(void)
{
    dotenv->config(object{
        object::pair{std::string("path"), std::string("../.env")}
    });
    if (doubleDashIndex != -1) {
        potentialAgentFlags = rawArgs->slice(doubleDashIndex + 1)->filter([=](auto arg) mutable
        {
            return arg->startsWith(std::string("--"));
        }
        );
    } else {
        potentialAgentFlags = rawArgs->filter([=](auto arg) mutable
        {
            return AND((arg->startsWith(std::string("--"))), (arg != std::string("--")));
        }
        );
    }
    if (potentialAgentFlags->get_length() > 0) {
        array<string> requestedAgentNames = potentialAgentFlags->map([=](auto arg) mutable
        {
            return arg->replace((new RegExp(std::string("^-"))), string_empty)->toLowerCase();
        }
        );
        object agentsMap = object{
            object::pair{std::string("devRel"), std::string("devRel")}, 
            object::pair{std::string("communityManager"), std::string("communityManager")}, 
            object::pair{std::string("liaison"), std::string("liaison")}, 
            object::pair{std::string("projectManager"), std::string("projectManager")}, 
            object::pair{std::string("socialMediaManager"), std::string("socialMediaManager")}
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
        console->log(std::string("allAgents"), allAgents);
        console->log(std::string("matchedAgents"), matchedAgents);
        if (matchedAgents->get_length() > 0) {
            enabledAgents = matchedAgents;
        } else {
            logger->warn(std::string("No matching agents found for flags: ") + potentialAgentFlags->join(std::string(", ")) + std::string(". Available agent names (use --name):"));
            allAgents->forEach([=](auto agent) mutable
            {
                auto objectKey = Object->keys(agentsMap)->find([=](auto key) mutable
                {
                    return const_(agentsMap)[as<any>(key)] == agent;
                }
                );
                if (objectKey) {
                    logger->warn(std::string("  --") + objectKey + std::string(" (for ") + (OR((agent["character"]["name"]), (std::string("Unknown")))) + std::string(")"));
                }
            }
            );
            enabledAgents = array<any>();
        }
    }
    if (allAgents->get_length() == 0) {
        logger->warn(std::string("No agents are enabled in the configuration"));
    } else if (availableAgents->get_length() == 0) {
        logger->error(std::string("NO AGENTS AVAILABLE - INITIALIZING DEFAULT ELIZA CHARACTER"));
        logger->info(std::string("Configure the required platform integrations in your .env file"));
    } else if (availableAgents->get_length() < enabledAgents->get_length()) {
        logger->warn(string_empty + (enabledAgents->get_length() - availableAgents->get_length()) + std::string(" out of ") + enabledAgents->get_length() + std::string(" enabled agents were filtered out due to missing platform requirements."));
    } else {
        logger->info(string_empty + availableAgents->get_length() + std::string(" agents successfully initialized"));
    }
}

MAIN
