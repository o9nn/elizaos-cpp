#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/program/tests/updateState.h"

void Main(void)
{
    describe(std::string("raydium_vault"), [=]() mutable
    {
        auto provider = anchor->AnchorProvider->env();
        anchor->setProvider(provider);
        auto connection = provider->connection;
        auto nodeWallet = as<std::shared_ptr<NodeWallet>>(provider->wallet);
        shared signerWallet = anchor->web3->Keypair->fromSecretKey(nodeWallet->payer->secretKey);
        shared program = as<std::shared_ptr<Program<std::shared_ptr<RaydiumVault>>>>(anchor->workspace->RaydiumVault);
        it(std::string("Get Current authority wallets"), [=]() mutable
        {
            auto [vault_config] = anchor->web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(vaultConfigSeed) }, program->programId);
            auto vault_config_info = std::async([=]() { program->account->vaultConfig->fetch(vault_config); });
            console->log(std::string("executor authority: "), vault_config_info->executorAuthority->toString());
            console->log(std::string("emergency authority: "), vault_config_info->executorAuthority->toString());
            console->log(std::string("manager authority: "), vault_config_info->managerAuthority->toString());
        }
        );
        it(std::string("Update executor, emergency and manager authority"), [=]() mutable
        {
            auto [vaultConfig] = anchor->web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(vaultConfigSeed) }, program->programId);
            console->log(std::string("Update Executor Wallet..."));
            auto newExecutorAddress = std::make_shared<anchor->web3->PublicKey>(std::string("6HHoqvXfNF1aQpwhn4k13CL7iyzFpjghLhG2eBG6xMVV"));
            auto newEmergencyAddress = std::make_shared<anchor->web3->PublicKey>(std::string("6HHoqvXfNF1aQpwhn4k13CL7iyzFpjghLhG2eBG6xMVV"));
            auto newManagerAddress = std::make_shared<anchor->web3->PublicKey>(std::string("6HHoqvXfNF1aQpwhn4k13CL7iyzFpjghLhG2eBG6xMVV"));
            std::async([=]() { program->rpc->changeExecutorAuthority(newExecutorAddress, object{
                object::pair{std::string("accounts"), object{
                    object::pair{std::string("authority"), signerWallet->publicKey}, 
                    object::pair{std::string("vaultConfig"), vaultConfig}
                }}
            }); });
            console->log(std::string("Update Emergency Wallet..."));
            std::async([=]() { program->rpc->changeEmergencyAuthority(newEmergencyAddress, object{
                object::pair{std::string("accounts"), object{
                    object::pair{std::string("authority"), signerWallet->publicKey}, 
                    object::pair{std::string("vaultConfig"), vaultConfig}
                }}
            }); });
            console->log(std::string("Update Manager Wallet..."));
            std::async([=]() { program->rpc->changeManagerAuthority(newManagerAddress, object{
                object::pair{std::string("accounts"), object{
                    object::pair{std::string("authority"), signerWallet->publicKey}, 
                    object::pair{std::string("vaultConfig"), vaultConfig}
                }}
            }); });
            auto vaultConfigInfo = std::async([=]() { program->account->vaultConfig->fetch(vaultConfig); });
            console->log(std::string("executor authority: "), vaultConfigInfo->executorAuthority->toString());
            console->log(std::string("emergency authority: "), vaultConfigInfo->executorAuthority->toString());
            console->log(std::string("manager authority: "), vaultConfigInfo->managerAuthority->toString());
        }
        );
    }
    );
}

MAIN
