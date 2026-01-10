#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/program/tests/launchToken.h"

void Main(void)
{
    ([=]() mutable
    {
        auto provider = anchor->AnchorProvider->env();
        anchor->setProvider(provider);
        auto signerWallet = anchor->web3->Keypair->fromSecretKey((as<std::shared_ptr<NodeWallet>>(provider->wallet))->payer->secretKey);
        auto program = as<std::shared_ptr<Program<std::shared_ptr<Autofun>>>>(anchor->workspace->Autofun);
        auto name = std::string("YourTokenName");
        auto symbol = std::string("YTN");
        auto uri = std::string("https://example.com/metadata.json");
        auto decimals = 6;
        auto token_supply = std::make_shared<anchor->BN>(1000000000000000ll);
        auto virtual_lamport_reserves = std::make_shared<anchor->BN>(280000000);
        console->log(std::string("Decimals:"), decimals);
        console->log(std::string("Token Supply:"), token_supply->toNumber());
        console->log(std::string("Virtual Reserves:"), virtual_lamport_reserves->toNumber());
        auto [configPDA] = web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(std::string("config")) }, program->programId);
        auto configAccount = std::async([=]() { program->account->config->fetch(configPDA); });
        auto tokenKp = Keypair->generate();
        try
        {
            auto tx = std::async([=]() { program->methods->launch(decimals, token_supply, virtual_lamport_reserves, name, symbol, uri)->accounts(object{
                object::pair{std::string("creator"), provider->wallet->publicKey}, 
                object::pair{std::string("token"), tokenKp->publicKey}, 
                object::pair{std::string("teamWallet"), configAccount->teamWallet}
            })->signers(array<any>{ signerWallet, tokenKp })->rpc(); });
            console->log(std::string("Transaction successful with signature:"), tx);
        }
        catch (const any& error)
        {
            console->error(std::string("Transaction failed:"), error);
        }
    }
    )();
}

MAIN
