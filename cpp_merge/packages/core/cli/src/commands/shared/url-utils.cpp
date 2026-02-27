#include "url-utils.h"
#include <string>

string getAgentRuntimeUrl(std::shared_ptr<OptionValues> opts)
{
    return (OR((OR((opts->remoteUrl->replace((new RegExp(std::string("\/"))), string_empty)), (process->env->AGENT_RUNTIME_URL->replace((new RegExp(std::string("\/"))), string_empty)))), (std::string("http://localhost:") + (OR((OR((opts->port), (process->env->SERVER_PORT))), (std::string("3000")))) + string_empty)));
};


string getAgentsBaseUrl(std::shared_ptr<OptionValues> opts)
{
    return string_empty + getAgentRuntimeUrl(opts) + std::string("/api/agents");
};


