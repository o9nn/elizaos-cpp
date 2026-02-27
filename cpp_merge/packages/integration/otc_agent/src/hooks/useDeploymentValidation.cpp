#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otc-agent/src/hooks/useDeploymentValidation.h"

void useDeploymentValidation()
{
    useEffect([=]() mutable
    {
        if (process->env->NODE_ENV == std:("development")) return;
        auto chains = array<std::shared_ptr<Chain>>{ std:("base"), std:("solana") };
        auto missing = array<string>();
        for (auto& chain : chains)
        {
            auto config = getChainConfig(chain);
            if (isEVMChain(chain)) {
                if (!config->contracts->otc) missing->push(string_empty + chain + std:(" (OTC Contract)"));
            }
            if (isSolanaChain(chain)) {
                if (!config->contracts->otc) missing->push(string_empty + chain + std:(" (Desk Address)"));
            }
        }
        if (missing->get_length() > 0) {
            console->error(std:("❌ CRITICAL: Missing deployment configuration for: ") + missing->join(std:(", ")) + std:(". ") + std:("Please ensure deployment JSONs are present in src/config/deployments/ or env vars are set."));
        }
    }
    , array<any>());
};



void Main(void)
{
    std:("use client");
}

MAIN
