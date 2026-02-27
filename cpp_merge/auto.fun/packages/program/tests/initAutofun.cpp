#include "initAutofun.hpp"

void Main(void)
{
    config(object{
        object::pair{std:("path"), std:("../../.env")}
    });
    console->log(std:("ENV: "), process->env->ENV);
    ([=]() mutable
    {
        console->log(std:("Starting initAutofun"));
        auto deployer = anchor->AnchorProvider->env();
        console->log(std:("deployer: "), deployer);
        anchor->setProvider(deployer);
        console->log(std:("deployer: "), deployer);
        auto program = as<std::shared_ptr<Program<std::shared_ptr<Autofun>>>>(anchor->workspace->Autofun);
        console->log(std:("programId: "), program->programId->toBase58());
        auto [configPDA] = web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(std:("config")) }, program->programId);
        auto [globalVault] = web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(std:("global")) }, program->programId);
        auto globalWsolSeed = Buffer::from(array<double>{ 6, 221, 246, 225, 215, 101, 161, 147, 217, 203, 225, 70, 206, 235, 121, 172, 28, 180, 133, 237, 95, 91, 55, 145, 58, 140, 245, 133, 126, 255, 0, 169 });
        auto nativeMint = std::make_shared<web3->PublicKey>(std:("So11111111111111111111111111111111111111112"));
        auto associatedTokenProgramId = std::make_shared<web3->PublicKey>(std:("ATokenGPvbdGVxr1b2hvZbsiqW5xWH25efTNsLJA8knL"));
        auto tokenProgramId = std::make_shared<web3->PublicKey>(std:("TokenkegQfeZyiNwAJbNbGKPFXCWuBvf9Ss623VQ5DA"));
        auto [globalWsolAccount] = web3->PublicKey->findProgramAddressSync(array<any>{ globalVault->toBuffer(), tokenProgramId->toBuffer(), nativeMint->toBuffer() }, associatedTokenProgramId);
        console->log(std:("provider.wallet.publicKey: "), deployer->wallet->publicKey->toBase58());
        auto newConfig = object{
            object::pair{std:("authority"), std::make_shared<anchor->web3->PublicKey>(std:("autozgbVb1EvhrTZTkpLekJRN4sN5hhGYpMMiY9kQ5S"))}, 
            object::pair{std:("pendingAuthority"), std::make_shared<anchor->web3->PublicKey>(std:("autozgbVb1EvhrTZTkpLekJRN4sN5hhGYpMMiY9kQ5S"))}, 
            object::pair{std:("teamWallet"), std::make_shared<anchor->web3->PublicKey>(std:("autovtovm7oqwtbyrWgdSH7i1W4nLPRWjXM2wcdqn1R"))}, 
            object::pair{std:("initBondingCurve"), 100}, 
            object::pair{std:("platformBuyFee"), std::make_shared<BN>(100)}, 
            object::pair{std:("platformSellFee"), std::make_shared<BN>(100)}, 
            object::pair{std:("curveLimit"), std::make_shared<BN>((process->env->ENV == std:("dev")) ? 11300000000ll : 113000000000ll)}, 
            object::pair{std:("lamportAmountConfig"), object{
                object::pair{std:("range"), object{
                    object::pair{std:("min"), std::make_shared<BN>(0.01 * anchor->web3->LAMPORTS_PER_SOL)}, 
                    object::pair{std:("max"), std::make_shared<BN>(100 * anchor->web3->LAMPORTS_PER_SOL)}
                }}
            }}, 
            object::pair{std:("tokenSupplyConfig"), object{
                object::pair{std:("range"), object{
                    object::pair{std:("min"), std::make_shared<BN>(5000)}, 
                    object::pair{std:("max"), std::make_shared<BN>(1000000000000000ll)}
                }}
            }}, 
            object::pair{std:("tokenDecimalsConfig"), object{
                object::pair{std:("range"), object{
                    object::pair{std:("min"), 6}, 
                    object::pair{std:("max"), 9}
                }}
            }}
        };
        console->log(std:("New Config:"), newConfig);
        console->log(std:("Config PDA:"), configPDA->toBase58());
        console->log(std:("Global Vault PDA:"), globalVault->toBase58());
        console->log(std:("Global WSOL Account PDA:"), globalWsolAccount->toBase58());
        auto accounts = object{
            object::pair{std:("payer"), deployer->wallet->publicKey}, 
            object::pair{std:("config"), configPDA}, 
            object::pair{std:("global_vault"), globalVault}, 
            object::pair{std:("global_wsol_account"), globalWsolAccount}, 
            object::pair{std:("native_mint"), nativeMint}, 
            object::pair{std:("system_program"), web3->SystemProgram->programId}, 
            object::pair{std:("token_program"), tokenProgramId}, 
            object::pair{std:("associated_token_program"), associatedTokenProgramId}
        };
        try
        {
            auto txSignature = std::async([=]() { program->methods->configure(newConfig)->accounts(accounts)->rpc(); });
            console->log(std:("Transaction sent successfully!"));
            console->log(std:("Signature:"), txSignature);
        }
        catch (const any& err)
        {
            console->error(std:("Transaction failed:"), err);
        }
    }
    )();
}

MAIN
