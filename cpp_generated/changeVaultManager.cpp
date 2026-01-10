#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/program/tests/changeVaultManager.h"

void Main(void)
{
    ([=]() mutable
    {
        auto provider = anchor->AnchorProvider->env();
        anchor->setProvider(provider);
        auto raydiumProgram = as<std::shared_ptr<Program<std::shared_ptr<RaydiumVault>>>>(anchor->workspace->RaydiumVault);
        auto [vaultConfigPDA] = anchor->web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(vaultConfigSeed) }, raydiumProgram->programId);
        auto newManagerAuthority = std::make_shared<anchor->web3->PublicKey>(std::string("autozgbVb1EvhrTZTkpLekJRN4sN5hhGYpMMiY9kQ5S"));
        console->log(std::string("Attempting to change manager for vault: ") + vaultConfigPDA->toString() + string_empty);
        console->log(std::string("Current authority (wallet): ") + provider->wallet->publicKey->toString() + string_empty);
        console->log(std::string("New manager authority: ") + newManagerAuthority->toString() + string_empty);
        try
        {
            auto txSignature = std::async([=]() { raydiumProgram->methods->changeManagerAuthority(newManagerAuthority)->accounts(object{
                object::pair{std::string("authority"), provider->wallet->publicKey}, 
                object::pair{std::string("vaultConfig"), vaultConfigPDA}
            })->rpc(); });
            console->log(std::string("Transaction sent successfully!"));
            console->log(std::string("Signature:"), txSignature);
            auto updatedVaultConfig = std::async([=]() { raydiumProgram->account->vaultConfig->fetch(vaultConfigPDA); });
            console->log(std::string("Updated manager authority:"), updatedVaultConfig->managerAuthority->toString());
            if (updatedVaultConfig->managerAuthority->equals(newManagerAuthority)) {
                console->log(std::string("Manager authority updated successfully!"));
            } else {
                console->error(std::string("Verification failed: Manager authority did not update."));
            }
        }
        catch (const any& error)
        {
            console->error(std::string("Error changing manager authority:"), error);
            if (is<anchor->AnchorError>(error)) {
                console->error(std::string("AnchorError Code:"), error["error"]["errorCode"]["code"]);
                console->error(std::string("AnchorError Number:"), error["error"]["errorCode"]["number"]);
                console->error(std::string("AnchorError Message:"), error["error"]["errorMessage"]);
                console->error(std::string("Error Logs:"), error["logs"]);
            }
        }
    }
    )();
}

MAIN
