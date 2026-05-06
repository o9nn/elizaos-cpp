#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/client/src/hooks/use-config-account.h"

array<string> configAccountQueryKey = array<string>{ std::string("configAccount") };
std::function<std::shared_ptr<Promise<any>>(any)> getConfigAccount = [=](auto program) mutable
{
    return queryClient->ensureQueryData(object{
        object::pair{std::string("queryKey"), configAccountQueryKey}, 
        object::pair{std::string("queryFn"), [=]() mutable
        {
            if (!program) {
                throw any(std::make_shared<Error>(std::string("missing program")));
            }
            auto [configPda, _] = PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(SEED_CONFIG) }, program->programId);
            auto configAccount = std::async([=]() { program->account->config->fetch(configPda); });
            return configAccount;
        }
        }, 
        object::pair{std::string("staleTime"), 1000 * 60 * 10}
    });
};

void Main(void)
{
}

MAIN
