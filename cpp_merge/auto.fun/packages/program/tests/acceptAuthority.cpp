#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/program/tests/acceptAuthority.h"

void Main(void)
{
    ([=]() mutable
    {
        console->log(std:("Starting acceptAuthority"));
        auto provider = anchor->AnchorProvider->env();
        anchor->setProvider(provider);
        auto pendingAuthorityWallet = provider->wallet;
        console->log(std:("Attempting to accept authority as: ") + pendingAuthorityWallet->publicKey->toBase58() + string_empty);
        auto program = as<std::shared_ptr<Program<std::shared_ptr<Autofun>>>>(anchor->workspace->Autofun);
        console->log(std:("Using Autofun program ID:"), program->programId->toBase58());
        auto [configPDA] = web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(std:("config")) }, program->programId);
        console->log(std:("Config PDA:"), configPDA->toBase58());
        auto accounts = object{
            object::pair{std:("newAdmin"), pendingAuthorityWallet->publicKey}, 
            object::pair{std:("globalConfig"), configPDA}
        };
        try
        {
            auto configState = std::async([=]() { program->account->config->fetch(configPDA); });
            console->log(std:("Current on-chain authority: ") + configState->authority->toBase58() + string_empty);
            console->log(std:("Current on-chain pending authority: ") + configState->pendingAuthority->toBase58() + string_empty);
            if (configState->pendingAuthority->equals(web3->PublicKey->default)) {
                console->error(std:("Error: No pending authority set in the config account. Run nominate_authority first."));
                return std::shared_ptr<Promise<void>>();
            }
            if (!configState->pendingAuthority->equals(pendingAuthorityWallet->publicKey)) {
                console->error(std:("Error: The configured wallet (") + pendingAuthorityWallet->publicKey->toBase58() + std:(") does not match the pending authority (") + configState->pendingAuthority->toBase58() + std:(") set in the config account."));
                return std::shared_ptr<Promise<void>>();
            }
            console->log(std:("Sending accept_authority transaction..."));
            auto txSignature = std::async([=]() { program->methods->acceptAuthority()->accounts(accounts)->rpc(); });
            console->log(std:("Transaction sent successfully!"));
            console->log(std:("Signature:"), txSignature);
            auto updatedConfigState = std::async([=]() { program->account->config->fetch(configPDA); });
            console->log(std:("New on-chain authority: ") + updatedConfigState->authority->toBase58() + string_empty);
            console->log(std:("New on-chain pending authority: ") + updatedConfigState->pendingAuthority->toBase58() + std:(" (should be default)"));
            if (updatedConfigState->authority->equals(pendingAuthorityWallet->publicKey)) {
                console->log(std:("Authority successfully updated!"));
            } else {
                console->error(std:("Error: Authority was not updated correctly after transaction."));
            }
        }
        catch (const any& err)
        {
            console->error(std:("Accept Authority transaction failed:"), err);
            if (is<anchor->AnchorError>(err)) {
                console->error(std:("AnchorError Details:"), err["error"]);
                console->error(std:("Error Logs:"), err["logs"]);
            }
        }
    }
    )();
}

MAIN
