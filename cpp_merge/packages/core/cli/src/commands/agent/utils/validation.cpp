#include "/home/runner/work/elizaos-cpp/elizaos-cpp/eliza/packages/cli/src/commands/agent/utils/validation.h"

std::shared_ptr<Promise<array<std::shared_ptr<AgentBasic>>>> getAgents(std::shared_ptr<OptionValues> opts)
{
    auto baseUrl = getAgentsBaseUrl(opts);
    auto response = std::async([=]() { fetch(baseUrl); });
    if (!response->ok) {
        throw any(std::make_shared<Error>(std:("Failed to fetch agents list: ") + response->statusText + string_empty));
    }
    auto rawData = std::async([=]() { response->json(); });
    auto validatedData = AgentsListResponseSchema->parse(rawData);
    return as<array<std::shared_ptr<AgentBasic>>>((OR((validatedData->data->agents), (array<any>()))));
};


std::shared_ptr<Promise<string>> resolveAgentId(string idOrNameOrIndex, std::shared_ptr<OptionValues> opts)
{
    auto agents = std::async([=]() { getAgents(opts); });
    auto agentByName = agents->find([=](auto agent) mutable
    {
        return agent->name->toLowerCase() == idOrNameOrIndex->toLowerCase();
    }
    );
    if (agentByName) {
        return agentByName->id;
    }
    auto agentById = agents->find([=](auto agent) mutable
    {
        return agent->id == idOrNameOrIndex;
    }
    );
    if (agentById) {
        return agentById->id;
    }
    if (!Number->isNaN(Number(idOrNameOrIndex))) {
        auto indexAgent = const_(agents)[Number(idOrNameOrIndex)];
        if (indexAgent) {
            return indexAgent->id;
        }
    }
    throw any(std::make_shared<Error>(std:("AGENT_NOT_FOUND:") + idOrNameOrIndex + string_empty));
};


any AgentBasicSchema = z->object(object{
    object::pair{std:("id"), z->string()}, 
    object::pair{std:("name"), z->string()}, 
    object::pair{std:("status"), z->string()->optional()}
})->passthrough();
any AgentsListResponseSchema = z->object(object{
    object::pair{std:("success"), z->boolean()}, 
    object::pair{std:("data"), z->object(object{
        object::pair{std:("agents"), z->array(AgentBasicSchema)}
    })->optional()}
});

void Main(void)
{
}

MAIN
