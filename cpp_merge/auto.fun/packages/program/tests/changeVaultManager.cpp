#include "changeVaultManager.hpp"

void Main(void)
{
    ([=]() mutable
    {
        auto provider = anchor->AnchorProvider->env();
        anchor->setProvider(provider);
        auto raydiumProgram = as<std::shared_ptr<Program<std::shared_ptr<RaydiumVault>>>>(anchor->workspace->RaydiumVault);
        auto [vaultConfigPDA] = anchor->web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(vaultConfigSeed) }, raydiumProgram->programId);
        auto newManagerAuthority = std::make_shared<anchor->web3->PublicKey>(std:("autozgbVb1EvhrTZTkpLekJRN4sN5hhGYpMMiY9kQ5S"));
        console->log(std:("Attempting to change manager for vault: ") + vaultConfigPDA->toString() + string_empty);
        console->log(std:("Current authority (wallet): ") + provider->wallet->publicKey->toString() + string_empty);
        console->log(std:("New manager authority: ") + newManagerAuthority->toString() + string_empty);
        try
        {
            auto txSignature = std::async([=]() { raydiumProgram->methods->changeManagerAuthority(newManagerAuthority)->accounts(object{
                object::pair{std:("authority"), provider->wallet->publicKey}, 
                object::pair{std:("vaultConfig"), vaultConfigPDA}
            })->rpc(); });
            console->log(std:("Transaction sent successfully!"));
            console->log(std:("Signature:"), txSignature);
            auto updatedVaultConfig = std::async([=]() { raydiumProgram->account->vaultConfig->fetch(vaultConfigPDA); });
            console->log(std:("Updated manager authority:"), updatedVaultConfig->managerAuthority->toString());
            if (updatedVaultConfig->managerAuthority->equals(newManagerAuthority)) {
                console->log(std:("Manager authority updated successfully!"));
            } else {
                console->error(std:("Verification failed: Manager authority did not update."));
            }
        }
        catch (const any& error)
        {
            console->error(std:("Error changing manager authority:"), error);
            if (is<anchor->AnchorError>(error)) {
                console->error(std:("AnchorError Code:"), error["error"]["errorCode"]["code"]);
                console->error(std:("AnchorError Number:"), error["error"]["errorCode"]["number"]);
                console->error(std:("AnchorError Message:"), error["error"]["errorMessage"]);
                console->error(std:("Error Logs:"), error["logs"]);
            }
        }
    }
    )();
}

MAIN
