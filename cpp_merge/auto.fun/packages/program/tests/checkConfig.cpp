#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/program/tests/checkConfig.h"

void Main(void)
{
    ([=]() mutable
    {
        auto provider = anchor->AnchorProvider->env();
        anchor->setProvider(provider);
        auto autofunProgram = as<std::shared_ptr<Program<std::shared_ptr<Autofun>>>>(anchor->workspace->Autofun);
        auto raydiumVaultProgram = as<std::shared_ptr<Program<std::shared_ptr<RaydiumVault>>>>(anchor->workspace->RaydiumVault);
        auto [configPDA] = web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(std:("config")) }, autofunProgram->programId);
        auto [globalVaultPDA] = web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(std:("global")) }, autofunProgram->programId);
        auto globalWsolSeed = Buffer::from(array<double>{ 6, 221, 246, 225, 215, 101, 161, 147, 217, 203, 225, 70, 206, 235, 121, 172, 28, 180, 133, 237, 95, 91, 55, 145, 58, 140, 245, 133, 126, 255, 0, 169 });
        auto nativeMint = std::make_shared<web3->PublicKey>(std:("So11111111111111111111111111111111111111112"));
        auto [globalWsolAccountPDA] = web3->PublicKey->findProgramAddressSync(array<any>{ globalVaultPDA->toBuffer(), globalWsolSeed, nativeMint->toBuffer() }, autofunProgram->programId);
        auto [vaultConfigPDA] = web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(std:("raydium_vault_config")) }, raydiumVaultProgram->programId);
        auto autofunConfigPDA = std::async([=]() { provider->connection->getAccountInfo(configPDA, std:("confirmed")); });
        auto globalVaultAccountInfo = std::async([=]() { provider->connection->getAccountInfo(globalVaultPDA, std:("confirmed")); });
        auto vaultAccountInfo = std::async([=]() { provider->connection->getAccountInfo(vaultConfigPDA, std:("confirmed")); });
        if (!autofunConfigPDA) {
            console->error(std:("Config account not found for PDA:"), configPDA->toBase58());
        } else {
            try
            {
                auto decodedAutofunConfig = autofunProgram->coder->accounts->decode(std:("config"), autofunConfigPDA->data);
                console->log(std:("-------------------------------- Autofun Config --------------------------------"));
                console->log(std:("Config PDA:"), configPDA->toBase58());
                console->log(std:("Current Admin:"), decodedAutofunConfig->authority->toString());
                console->log(std:("Pending Admin:"), decodedAutofunConfig->pendingAuthority->toString());
                console->log(std:("Team Wallet:"), decodedAutofunConfig->teamWallet->toString());
                console->log(std:("Init Bonding Curve:"), decodedAutofunConfig->initBondingCurve);
                console->log(std:("Platform Buy Fee:"), ((std::make_shared<BN>(decodedAutofunConfig->platformBuyFee)))->toNumber());
                console->log(std:("Platform Sell Fee:"), ((std::make_shared<BN>(decodedAutofunConfig->platformSellFee)))->toNumber());
                console->log(std:("Curve Limit:"), ((std::make_shared<BN>(decodedAutofunConfig->curveLimit)))->toNumber());
                console->log(std:("Lamport Amount Config:"), object{
                    object::pair{std:("min"), ((std::make_shared<BN>(decodedAutofunConfig->lamportAmountConfig->range->min)))->toNumber()}, 
                    object::pair{std:("max"), ((std::make_shared<BN>(decodedAutofunConfig->lamportAmountConfig->range->max)))->toNumber()}
                });
                console->log(std:("Token Supply Config:"), object{
                    object::pair{std:("min"), ((std::make_shared<BN>(decodedAutofunConfig->tokenSupplyConfig->range->min)))->toNumber()}, 
                    object::pair{std:("max"), ((std::make_shared<BN>(decodedAutofunConfig->tokenSupplyConfig->range->max)))->toNumber()}
                });
                console->log(std:("Token Decimals Config:"), object{
                    object::pair{std:("min"), decodedAutofunConfig->tokenDecimalsConfig->range->min}, 
                    object::pair{std:("max"), decodedAutofunConfig->tokenDecimalsConfig->range->max}
                });
            }
            catch (const any& error)
            {
                console->error(std:("Error decoding config account data:"), error);
            }
        }
        if (!globalVaultAccountInfo) {
            console->error(std:("Global vault account not found for PDA:"), globalVaultPDA->toBase58());
        } else {
            console->log(std:("Global Vault PDA:"), globalVaultPDA->toBase58());
        }
        console->log(std:("Global WSOL Account PDA:"), globalWsolAccountPDA->toBase58());
        if (!vaultAccountInfo) {
            console->error(std:("Vault config account not found for PDA:"), vaultConfigPDA->toBase58());
        } else {
            try
            {
                auto decodedVaultConfig = raydiumVaultProgram->coder->accounts->decode(std:("vaultConfig"), vaultAccountInfo->data);
                console->log(std:("-------------------------------- Vault Config --------------------------------"));
                console->log(std:("Vault Config PDA:"), vaultConfigPDA->toBase58());
                console->log(std:("Executor Authority:"), decodedVaultConfig->executorAuthority->toString());
                console->log(std:("Emergency Authority:"), decodedVaultConfig->emergencyAuthority->toString());
                console->log(std:("Manager Authority:"), decodedVaultConfig->managerAuthority->toString());
            }
            catch (const any& error)
            {
                console->error(std:("Error decoding vault config account data:"), error);
            }
        }
    }
    )();
}

MAIN
