#include "use-config-account.h"

array<string> configAccountQueryKey = array<string>{ std:("configAccount") };
std::function<std::shared_ptr<Promise<any>>(any)> getConfigAccount = [=](auto program) mutable
{
    return queryClient->ensureQueryData(object{
        object::pair{std:("queryKey"), configAccountQueryKey}, 
        object::pair{std:("queryFn"), [=]() mutable
        {
            if (!program) {
                throw any(std::make_shared<Error>(std:("missing program")));
            }
            auto [configPda, _] = PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(SEED_CONFIG) }, program->programId);
            auto configAccount = std::async([=]() { program->account->config->fetch(configPda); });
            return configAccount;
        }
        }, 
        object::pair{std:("staleTime"), 1000 * 60 * 10}
    });
};

void Main(void)
{
}

MAIN
