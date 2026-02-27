#include "initRayVault.hpp"

void Main(void)
{
    describe(std:("raydium_vault"), [=]() mutable
    {
        auto provider = anchor->AnchorProvider->env();
        anchor->setProvider(provider);
        auto nodeWallet = as<std::shared_ptr<NodeWallet>>(provider->wallet);
        shared signerWallet = anchor->web3->Keypair->fromSecretKey(nodeWallet->payer->secretKey);
        shared program = as<std::shared_ptr<Program<std::shared_ptr<RaydiumVault>>>>(anchor->workspace->RaydiumVault);
        it(std:("Initialize Vault Config"), [=]() mutable
        {
            auto vaultConfig = object{
                object::pair{std:("executorAuthority"), std::make_shared<anchor->web3->PublicKey>(std:("autozgbVb1EvhrTZTkpLekJRN4sN5hhGYpMMiY9kQ5S"))}, 
                object::pair{std:("emergencyAuthority"), std::make_shared<anchor->web3->PublicKey>(std:("iGkGbxitDUdFhWSewd9gX2QzwTqJQSoFP5pkwvj25YP"))}, 
                object::pair{std:("managerAuthority"), std::make_shared<anchor->web3->PublicKey>(std:("autozgbVb1EvhrTZTkpLekJRN4sN5hhGYpMMiY9kQ5S"))}
            };
            auto [vault] = anchor->web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(vaultConfigSeed) }, program->programId);
            any vaultConfigInfo;
            try
            {
                vaultConfigInfo = std::async([=]() { program->account->vaultConfig->fetch(vault); });
            }
            catch (const any& error)
            {
                std::async([=]() { program->rpc->initialize(vaultConfig, object{
                    object::pair{std:("accounts"), object{
                        object::pair{std:("payer"), signerWallet->publicKey}, 
                        object::pair{std:("vaultConfig"), vault}, 
                        object::pair{std:("systemProgram"), anchor->web3->SystemProgram->programId}
                    }}
                }); });
                vaultConfigInfo = std::async([=]() { program->account->vaultConfig->fetch(vault); });
            }
            console->log(std:("executor: "), vaultConfigInfo["executorAuthority"]["toString"]());
            console->log(std:("emergency: "), vaultConfigInfo["emergencyAuthority"]["toString"]());
            console->log(std:("manager: "), vaultConfigInfo["managerAuthority"]["toString"]());
        }
        );
    }
    );
}

MAIN
