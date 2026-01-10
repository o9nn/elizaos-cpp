#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/program/tests/changeVaultExec.h"

void Main(void)
{
    ([=]() mutable
    {
        auto provider = anchor->AnchorProvider->env();
        anchor->setProvider(provider);
        auto raydiumProgram = as<std::shared_ptr<Program<std::shared_ptr<RaydiumVault>>>>(anchor->workspace->RaydiumVault);
        auto [vaultConfigPDA] = anchor->web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(std::string("raydium_vault_config")) }, raydiumProgram->programId);
        auto newExecutorAuthority = std::make_shared<anchor->web3->PublicKey>(std::string("autozgbVb1EvhrTZTkpLekJRN4sN5hhGYpMMiY9kQ5S"));
        try
        {
            auto txSignature = std::async([=]() { raydiumProgram->methods->changeExecutorAuthority(newExecutorAuthority)->accounts(object{
                object::pair{std::string("authority"), provider->wallet->publicKey}, 
                object::pair{std::string("vaultConfig"), vaultConfigPDA}
            })->rpc(); });
            console->log(std::string("Transaction sent successfully!"));
            console->log(std::string("Signature:"), txSignature);
        }
        catch (const any& error)
        {
            console->error(std::string("Error changing executor authority:"), error);
        }
    }
    )();
}

MAIN
