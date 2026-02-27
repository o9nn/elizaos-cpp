#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/program/tests/launchToken.h"

void Main(void)
{
    ([=]() mutable
    {
        auto provider = anchor->AnchorProvider->env();
        anchor->setProvider(provider);
        auto signerWallet = anchor->web3->Keypair->fromSecretKey((as<std::shared_ptr<NodeWallet>>(provider->wallet))->payer->secretKey);
        auto program = as<std::shared_ptr<Program<std::shared_ptr<Autofun>>>>(anchor->workspace->Autofun);
        auto name = std:("YourTokenName");
        auto symbol = std:("YTN");
        auto uri = std:("https://example.com/metadata.json");
        auto decimals = 6;
        auto token_supply = std::make_shared<anchor->BN>(1000000000000000ll);
        auto virtual_lamport_reserves = std::make_shared<anchor->BN>(280000000);
        console->log(std:("Decimals:"), decimals);
        console->log(std:("Token Supply:"), token_supply->toNumber());
        console->log(std:("Virtual Reserves:"), virtual_lamport_reserves->toNumber());
        auto [configPDA] = web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(std:("config")) }, program->programId);
        auto configAccount = std::async([=]() { program->account->config->fetch(configPDA); });
        auto tokenKp = Keypair->generate();
        try
        {
            auto tx = std::async([=]() { program->methods->launch(decimals, token_supply, virtual_lamport_reserves, name, symbol, uri)->accounts(object{
                object::pair{std:("creator"), provider->wallet->publicKey}, 
                object::pair{std:("token"), tokenKp->publicKey}, 
                object::pair{std:("teamWallet"), configAccount->teamWallet}
            })->signers(array<any>{ signerWallet, tokenKp })->rpc(); });
            console->log(std:("Transaction successful with signature:"), tx);
        }
        catch (const any& error)
        {
            console->error(std:("Transaction failed:"), error);
        }
    }
    )();
}

MAIN
