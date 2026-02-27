#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/commands/agent/utils/display.h"

std::shared_ptr<Promise<void>> listAgents(std::shared_ptr<OptionValues> opts)
{
    try
    {
        auto agents = std::async([=]() { getAgents(opts); });
        auto agentData = agents->map([=](auto agent) mutable
        {
            return (object{
                object::pair{std:("Name"), agent->name}, 
                object::pair{std:("ID"), agent->id}, 
                object::pair{std:("Status"), OR((agent->status), (std:("unknown")))}
            });
        }
        );
        if (opts->json) {
            console->info(JSON->stringify(agentData, nullptr, 2));
        } else {
            console->info(std:("\
Available agents:"));
            if (agentData->get_length() == 0) {
                console->info(std:("No agents found"));
            } else {
                console->table(agentData);
            }
        }
        return std::shared_ptr<Promise<void>>();
    }
    catch (const any& error)
    {
        std::async([=]() { checkServer(opts); });
        handleError(error);
    }
};


