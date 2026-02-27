#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otc-agent/src/hooks/useDeploymentValidation.h"

void useDeploymentValidation()
{
    useEffect([=]() mutable
    {
        if (process->env->NODE_ENV == std::string("development")) return;
        auto chains = array<std::shared_ptr<Chain>>{ std::string("base"), std::string("solana") };
        auto missing = array<string>();
        for (auto& chain : chains)
        {
            auto config = getChainConfig(chain);
            if (isEVMChain(chain)) {
                if (!config->contracts->otc) missing->push(string_empty + chain + std::string(" (OTC Contract)"));
            }
            if (isSolanaChain(chain)) {
                if (!config->contracts->otc) missing->push(string_empty + chain + std::string(" (Desk Address)"));
            }
        }
        if (missing->get_length() > 0) {
            console->error(std::string("❌ CRITICAL: Missing deployment configuration for: ") + missing->join(std::string(", ")) + std::string(". ") + std::string("Please ensure deployment JSONs are present in src/config/deployments/ or env vars are set."));
        }
    }
    , array<any>());
};



void Main(void)
{
    std::string("use client");
}

MAIN
