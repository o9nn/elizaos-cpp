#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/program/tests/updateState.h"

void Main(void)
{
    describe(std:("raydium_vault"), [=]() mutable
    {
        auto provider = anchor->AnchorProvider->env();
        anchor->setProvider(provider);
        auto connection = provider->connection;
        auto nodeWallet = as<std::shared_ptr<NodeWallet>>(provider->wallet);
        shared signerWallet = anchor->web3->Keypair->fromSecretKey(nodeWallet->payer->secretKey);
        shared program = as<std::shared_ptr<Program<std::shared_ptr<RaydiumVault>>>>(anchor->workspace->RaydiumVault);
        it(std:("Get Current authority wallets"), [=]() mutable
        {
            auto [vault_config] = anchor->web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(vaultConfigSeed) }, program->programId);
            auto vault_config_info = std::async([=]() { program->account->vaultConfig->fetch(vault_config); });
            console->log(std:("executor authority: "), vault_config_info->executorAuthority->toString());
            console->log(std:("emergency authority: "), vault_config_info->executorAuthority->toString());
            console->log(std:("manager authority: "), vault_config_info->managerAuthority->toString());
        }
        );
        it(std:("Update executor, emergency and manager authority"), [=]() mutable
        {
            auto [vaultConfig] = anchor->web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(vaultConfigSeed) }, program->programId);
            console->log(std:("Update Executor Wallet..."));
            auto newExecutorAddress = std::make_shared<anchor->web3->PublicKey>(std:("6HHoqvXfNF1aQpwhn4k13CL7iyzFpjghLhG2eBG6xMVV"));
            auto newEmergencyAddress = std::make_shared<anchor->web3->PublicKey>(std:("6HHoqvXfNF1aQpwhn4k13CL7iyzFpjghLhG2eBG6xMVV"));
            auto newManagerAddress = std::make_shared<anchor->web3->PublicKey>(std:("6HHoqvXfNF1aQpwhn4k13CL7iyzFpjghLhG2eBG6xMVV"));
            std::async([=]() { program->rpc->changeExecutorAuthority(newExecutorAddress, object{
                object::pair{std:("accounts"), object{
                    object::pair{std:("authority"), signerWallet->publicKey}, 
                    object::pair{std:("vaultConfig"), vaultConfig}
                }}
            }); });
            console->log(std:("Update Emergency Wallet..."));
            std::async([=]() { program->rpc->changeEmergencyAuthority(newEmergencyAddress, object{
                object::pair{std:("accounts"), object{
                    object::pair{std:("authority"), signerWallet->publicKey}, 
                    object::pair{std:("vaultConfig"), vaultConfig}
                }}
            }); });
            console->log(std:("Update Manager Wallet..."));
            std::async([=]() { program->rpc->changeManagerAuthority(newManagerAddress, object{
                object::pair{std:("accounts"), object{
                    object::pair{std:("authority"), signerWallet->publicKey}, 
                    object::pair{std:("vaultConfig"), vaultConfig}
                }}
            }); });
            auto vaultConfigInfo = std::async([=]() { program->account->vaultConfig->fetch(vaultConfig); });
            console->log(std:("executor authority: "), vaultConfigInfo->executorAuthority->toString());
            console->log(std:("emergency authority: "), vaultConfigInfo->executorAuthority->toString());
            console->log(std:("manager authority: "), vaultConfigInfo->managerAuthority->toString());
        }
        );
    }
    );
}

MAIN
