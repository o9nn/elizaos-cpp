#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/program/tests/checkBalance.h"

void Main(void)
{
    describe(std::string("raydium_vault"), [=]() mutable
    {
        auto provider = anchor->AnchorProvider->env();
        anchor->setProvider(provider);
        shared connection = provider->connection;
        auto nodeWallet = as<std::shared_ptr<NodeWallet>>(provider->wallet);
        shared signerWallet = anchor->web3->Keypair->fromSecretKey(nodeWallet->payer->secretKey);
        it(std::string("Check Balance"), [=]() mutable
        {
            auto isDev = isDevnet(connection);
            auto position_nft = getNftAddress(isDev);
            auto claimer = claimer_address_0;
            std::async([=]() { spl->getOrCreateAssociatedTokenAccount(connection, signerWallet, position_nft, signerWallet->publicKey); });
            std::async([=]() { spl->getOrCreateAssociatedTokenAccount(connection, signerWallet, position_nft, claimer); });
            auto position_nft_account_signer = spl->getAssociatedTokenAddressSync(position_nft, signerWallet->publicKey);
            auto position_nft_account_claimer = spl->getAssociatedTokenAddressSync(position_nft, claimer);
            console->log(std::string("signer balance: "), (std::async([=]() { connection->getTokenAccountBalance(position_nft_account_signer); }))->value->amount);
            console->log(std::string("claimer balance: "), (std::async([=]() { connection->getTokenAccountBalance(position_nft_account_claimer); }))->value->amount);
        }
        );
    }
    );
}

MAIN
