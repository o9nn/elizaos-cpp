#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/program/tests/changeClaimer.h"

void Main(void)
{
    describe(std::string("raydium_vault"), [=]() mutable
    {
        auto provider = anchor->AnchorProvider->env();
        anchor->setProvider(provider);
        shared connection = provider->connection;
        auto nodeWallet = as<std::shared_ptr<NodeWallet>>(provider->wallet);
        shared signerWallet = anchor->web3->Keypair->fromSecretKey(nodeWallet->payer->secretKey);
        shared program = as<std::shared_ptr<Program<std::shared_ptr<RaydiumVault>>>>(anchor->workspace->RaydiumVault);
        it(std::string("Change Claimer"), [=]() mutable
        {
            auto isDev = isDevnet(connection);
            auto position_nft = getNftAddress(isDev);
            auto [vault_config] = anchor->web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(vaultConfigSeed) }, program->programId);
            auto [user_position] = anchor->web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(positionSeed), position_nft["toBuffer"]() }, program->programId);
            auto claimer_address = claimer_address_0;
            std::async([=]() { program->rpc->changeClaimer(claimer_address, object{
                object::pair{std::string("accounts"), object{
                    object::pair{std::string("authority"), signerWallet->publicKey}, 
                    object::pair{std::string("vaultConfig"), vault_config}, 
                    object::pair{std::string("userPosition"), user_position}, 
                    object::pair{std::string("positionNft"), position_nft}
                }}
            }); });
        }
        );
    }
    );
}

MAIN
