#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/program/tests/deposit.h"

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
        it(std::string("Deposit Position"), [=]() mutable
        {
            auto isDev = isDevnet(connection);
            auto position_nft = getNftAddress(isDev);
            auto [vault_config] = anchor->web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(vaultConfigSeed) }, program->programId);
            auto [user_position] = anchor->web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(positionSeed), position_nft["toBuffer"]() }, program->programId);
            auto from_account = spl->getAssociatedTokenAddressSync(position_nft, signerWallet->publicKey);
            auto [nft_token_faucet] = anchor->web3->PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(nftFaucetSeed), position_nft["toBuffer"]() }, program->programId);
            auto claimer_address = signerWallet->publicKey;
            std::async([=]() { program->rpc->deposit(claimer_address, object{
                object::pair{std::string("accounts"), object{
                    object::pair{std::string("authority"), signerWallet->publicKey}, 
                    object::pair{std::string("vaultConfig"), vault_config}, 
                    object::pair{std::string("userPosition"), user_position}, 
                    object::pair{std::string("positionNft"), position_nft}, 
                    object::pair{std::string("fromAccount"), from_account}, 
                    object::pair{std::string("nftTokenFaucet"), nft_token_faucet}, 
                    object::pair{std::string("tokenProgram"), spl->TOKEN_PROGRAM_ID}, 
                    object::pair{std::string("systemProgram"), anchor->web3->SystemProgram->programId}
                }}
            }); });
        }
        );
    }
    );
}

MAIN
