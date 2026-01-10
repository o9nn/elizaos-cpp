#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/program/tests/initAutofun.h"

void Main(void)
{
    config(object{
        object::pair{std::string("path"), std::string("../../.env")}
    });
    console->log(std::string("ENV: "), process->env->ENV);
    ([=]() mutable
    {
        console->log(std::string("Starting initAutofun"));
        auto deployer = anchor->AnchorProvider->env();
        console->log(std::string("deployer: "), deployer);
        anchor->setProvider(deployer);
        console->log(std::string("deployer: "), deployer);
        auto program = as<std::shared_ptr<Program<std::shared_ptr<Autofun>>>>(anchor->workspace->Autofun);
        console->log(std::string("programId: "), program->programId->toBase58());
        auto [configPDA] = web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(std::string("config")) }, program->programId);
        auto [globalVault] = web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(std::string("global")) }, program->programId);
        auto globalWsolSeed = Buffer::from(array<double>{ 6, 221, 246, 225, 215, 101, 161, 147, 217, 203, 225, 70, 206, 235, 121, 172, 28, 180, 133, 237, 95, 91, 55, 145, 58, 140, 245, 133, 126, 255, 0, 169 });
        auto nativeMint = std::make_shared<web3->PublicKey>(std::string("So11111111111111111111111111111111111111112"));
        auto associatedTokenProgramId = std::make_shared<web3->PublicKey>(std::string("ATokenGPvbdGVxr1b2hvZbsiqW5xWH25efTNsLJA8knL"));
        auto tokenProgramId = std::make_shared<web3->PublicKey>(std::string("TokenkegQfeZyiNwAJbNbGKPFXCWuBvf9Ss623VQ5DA"));
        auto [globalWsolAccount] = web3->PublicKey->findProgramAddressSync(array<any>{ globalVault->toBuffer(), tokenProgramId->toBuffer(), nativeMint->toBuffer() }, associatedTokenProgramId);
        console->log(std::string("provider.wallet.publicKey: "), deployer->wallet->publicKey->toBase58());
        auto newConfig = object{
            object::pair{std::string("authority"), std::make_shared<anchor->web3->PublicKey>(std::string("autozgbVb1EvhrTZTkpLekJRN4sN5hhGYpMMiY9kQ5S"))}, 
            object::pair{std::string("pendingAuthority"), std::make_shared<anchor->web3->PublicKey>(std::string("autozgbVb1EvhrTZTkpLekJRN4sN5hhGYpMMiY9kQ5S"))}, 
            object::pair{std::string("teamWallet"), std::make_shared<anchor->web3->PublicKey>(std::string("autovtovm7oqwtbyrWgdSH7i1W4nLPRWjXM2wcdqn1R"))}, 
            object::pair{std::string("initBondingCurve"), 100}, 
            object::pair{std::string("platformBuyFee"), std::make_shared<BN>(100)}, 
            object::pair{std::string("platformSellFee"), std::make_shared<BN>(100)}, 
            object::pair{std::string("curveLimit"), std::make_shared<BN>((process->env->ENV == std::string("dev")) ? 11300000000ll : 113000000000ll)}, 
            object::pair{std::string("lamportAmountConfig"), object{
                object::pair{std::string("range"), object{
                    object::pair{std::string("min"), std::make_shared<BN>(0.01 * anchor->web3->LAMPORTS_PER_SOL)}, 
                    object::pair{std::string("max"), std::make_shared<BN>(100 * anchor->web3->LAMPORTS_PER_SOL)}
                }}
            }}, 
            object::pair{std::string("tokenSupplyConfig"), object{
                object::pair{std::string("range"), object{
                    object::pair{std::string("min"), std::make_shared<BN>(5000)}, 
                    object::pair{std::string("max"), std::make_shared<BN>(1000000000000000ll)}
                }}
            }}, 
            object::pair{std::string("tokenDecimalsConfig"), object{
                object::pair{std::string("range"), object{
                    object::pair{std::string("min"), 6}, 
                    object::pair{std::string("max"), 9}
                }}
            }}
        };
        console->log(std::string("New Config:"), newConfig);
        console->log(std::string("Config PDA:"), configPDA->toBase58());
        console->log(std::string("Global Vault PDA:"), globalVault->toBase58());
        console->log(std::string("Global WSOL Account PDA:"), globalWsolAccount->toBase58());
        auto accounts = object{
            object::pair{std::string("payer"), deployer->wallet->publicKey}, 
            object::pair{std::string("config"), configPDA}, 
            object::pair{std::string("global_vault"), globalVault}, 
            object::pair{std::string("global_wsol_account"), globalWsolAccount}, 
            object::pair{std::string("native_mint"), nativeMint}, 
            object::pair{std::string("system_program"), web3->SystemProgram->programId}, 
            object::pair{std::string("token_program"), tokenProgramId}, 
            object::pair{std::string("associated_token_program"), associatedTokenProgramId}
        };
        try
        {
            auto txSignature = std::async([=]() { program->methods->configure(newConfig)->accounts(accounts)->rpc(); });
            console->log(std::string("Transaction sent successfully!"));
            console->log(std::string("Signature:"), txSignature);
        }
        catch (const any& err)
        {
            console->error(std::string("Transaction failed:"), err);
        }
    }
    )();
}

MAIN
