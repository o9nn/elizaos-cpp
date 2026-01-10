#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/program/tests/checkConfig.h"

void Main(void)
{
    ([=]() mutable
    {
        auto provider = anchor->AnchorProvider->env();
        anchor->setProvider(provider);
        auto autofunProgram = as<std::shared_ptr<Program<std::shared_ptr<Autofun>>>>(anchor->workspace->Autofun);
        auto raydiumVaultProgram = as<std::shared_ptr<Program<std::shared_ptr<RaydiumVault>>>>(anchor->workspace->RaydiumVault);
        auto [configPDA] = web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(std::string("config")) }, autofunProgram->programId);
        auto [globalVaultPDA] = web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(std::string("global")) }, autofunProgram->programId);
        auto globalWsolSeed = Buffer::from(array<double>{ 6, 221, 246, 225, 215, 101, 161, 147, 217, 203, 225, 70, 206, 235, 121, 172, 28, 180, 133, 237, 95, 91, 55, 145, 58, 140, 245, 133, 126, 255, 0, 169 });
        auto nativeMint = std::make_shared<web3->PublicKey>(std::string("So11111111111111111111111111111111111111112"));
        auto [globalWsolAccountPDA] = web3->PublicKey->findProgramAddressSync(array<any>{ globalVaultPDA->toBuffer(), globalWsolSeed, nativeMint->toBuffer() }, autofunProgram->programId);
        auto [vaultConfigPDA] = web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(std::string("raydium_vault_config")) }, raydiumVaultProgram->programId);
        auto autofunConfigPDA = std::async([=]() { provider->connection->getAccountInfo(configPDA, std::string("confirmed")); });
        auto globalVaultAccountInfo = std::async([=]() { provider->connection->getAccountInfo(globalVaultPDA, std::string("confirmed")); });
        auto vaultAccountInfo = std::async([=]() { provider->connection->getAccountInfo(vaultConfigPDA, std::string("confirmed")); });
        if (!autofunConfigPDA) {
            console->error(std::string("Config account not found for PDA:"), configPDA->toBase58());
        } else {
            try
            {
                auto decodedAutofunConfig = autofunProgram->coder->accounts->decode(std::string("config"), autofunConfigPDA->data);
                console->log(std::string("-------------------------------- Autofun Config --------------------------------"));
                console->log(std::string("Config PDA:"), configPDA->toBase58());
                console->log(std::string("Current Admin:"), decodedAutofunConfig->authority->toString());
                console->log(std::string("Pending Admin:"), decodedAutofunConfig->pendingAuthority->toString());
                console->log(std::string("Team Wallet:"), decodedAutofunConfig->teamWallet->toString());
                console->log(std::string("Init Bonding Curve:"), decodedAutofunConfig->initBondingCurve);
                console->log(std::string("Platform Buy Fee:"), ((std::make_shared<BN>(decodedAutofunConfig->platformBuyFee)))->toNumber());
                console->log(std::string("Platform Sell Fee:"), ((std::make_shared<BN>(decodedAutofunConfig->platformSellFee)))->toNumber());
                console->log(std::string("Curve Limit:"), ((std::make_shared<BN>(decodedAutofunConfig->curveLimit)))->toNumber());
                console->log(std::string("Lamport Amount Config:"), object{
                    object::pair{std::string("min"), ((std::make_shared<BN>(decodedAutofunConfig->lamportAmountConfig->range->min)))->toNumber()}, 
                    object::pair{std::string("max"), ((std::make_shared<BN>(decodedAutofunConfig->lamportAmountConfig->range->max)))->toNumber()}
                });
                console->log(std::string("Token Supply Config:"), object{
                    object::pair{std::string("min"), ((std::make_shared<BN>(decodedAutofunConfig->tokenSupplyConfig->range->min)))->toNumber()}, 
                    object::pair{std::string("max"), ((std::make_shared<BN>(decodedAutofunConfig->tokenSupplyConfig->range->max)))->toNumber()}
                });
                console->log(std::string("Token Decimals Config:"), object{
                    object::pair{std::string("min"), decodedAutofunConfig->tokenDecimalsConfig->range->min}, 
                    object::pair{std::string("max"), decodedAutofunConfig->tokenDecimalsConfig->range->max}
                });
            }
            catch (const any& error)
            {
                console->error(std::string("Error decoding config account data:"), error);
            }
        }
        if (!globalVaultAccountInfo) {
            console->error(std::string("Global vault account not found for PDA:"), globalVaultPDA->toBase58());
        } else {
            console->log(std::string("Global Vault PDA:"), globalVaultPDA->toBase58());
        }
        console->log(std::string("Global WSOL Account PDA:"), globalWsolAccountPDA->toBase58());
        if (!vaultAccountInfo) {
            console->error(std::string("Vault config account not found for PDA:"), vaultConfigPDA->toBase58());
        } else {
            try
            {
                auto decodedVaultConfig = raydiumVaultProgram->coder->accounts->decode(std::string("vaultConfig"), vaultAccountInfo->data);
                console->log(std::string("-------------------------------- Vault Config --------------------------------"));
                console->log(std::string("Vault Config PDA:"), vaultConfigPDA->toBase58());
                console->log(std::string("Executor Authority:"), decodedVaultConfig->executorAuthority->toString());
                console->log(std::string("Emergency Authority:"), decodedVaultConfig->emergencyAuthority->toString());
                console->log(std::string("Manager Authority:"), decodedVaultConfig->managerAuthority->toString());
            }
            catch (const any& error)
            {
                console->error(std::string("Error decoding vault config account data:"), error);
            }
        }
    }
    )();
}

MAIN
