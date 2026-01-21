#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/commands/agent/utils/display.h"

std::shared_ptr<Promise<void>> listAgents(std::shared_ptr<OptionValues> opts)
{
    try
    {
        auto agents = std::async([=]() { getAgents(opts); });
        auto agentData = agents->map([=](auto agent) mutable
        {
            return (object{
                object::pair{std::string("Name"), agent->name}, 
                object::pair{std::string("ID"), agent->id}, 
                object::pair{std::string("Status"), OR((agent->status), (std::string("unknown")))}
            });
        }
        );
        if (opts->json) {
            console->info(JSON->stringify(agentData, nullptr, 2));
        } else {
            console->info(std::string("\
Available agents:"));
            if (agentData->get_length() == 0) {
                console->info(std::string("No agents found"));
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


