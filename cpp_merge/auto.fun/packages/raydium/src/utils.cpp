#include "utils.hpp"
#include <string>

double fixedPoint = parseFloat(std::string("1000000000"));
string vaultConfigSeed = std::string("raydium_vault_config");
string positionSeed = std::string("raydium_position");
string claimerInfoSeed = std::string("raydium_claimer_info");
string nftFaucetSeed = std::string("raydium_vault_nft_seed");
any token0 = std::make_shared<anchor->web3->PublicKey>(std::string("EPjFWdd5AufqSSqeM2qN1xzybapC8G4wEGGkZwyTDt1v"));
any claimer_address_0 = std::make_shared<anchor->web3->PublicKey>(std::string("6HHoqvXfNF1aQpwhn4k13CL7iyzFpjghLhG2eBG6xMVV"));
string devnetEndpoint = std::string("https://api.devnet.solana.com");
std::function<std::shared_ptr<Promise<any>>(any, any, any, any)> sendSolTo = [=](auto amount, auto signerWallet, auto recvWallet, auto connection) mutable
{
    auto beforeBal = std::async([=]() { connection->getBalance(recvWallet); });
    console->log(std::string("beforeBal: "), parseFloat(beforeBal->toString()) / fixedPoint);
    auto transaction = ((std::make_shared<Transaction>()))->add(SystemProgram->transfer(object{
        object::pair{std::string("fromPubkey"), signerWallet["publicKey"]}, 
        object::pair{std::string("toPubkey"), recvWallet}, 
        object::pair{std::string("lamports"), amount}
    }));
    try
    {
        auto signature = std::async([=]() { sendAndConfirmTransaction(connection, transaction, array<any>{ signerWallet }); });
        console->log(std::string("confirmed transaction with signature "), signature);
        auto afterBal = std::async([=]() { connection->getBalance(recvWallet); });
        console->log(std::string("afterBal: "), parseFloat(afterBal->toString()) / fixedPoint);
        return signature;
    }
    catch (const any& error)
    {
        console->log(std::string("transaction failed: "), error);
    }
};
std::function<std::shared_ptr<Promise<void>>(any, any, any, any, any)> sendTokenTo = [=](auto amount, auto signerWallet, auto recvWallet, auto tokenAddress, auto connection) mutable
{
    auto signerTokenAccount = spl->getAssociatedTokenAddressSync(tokenAddress, signerWallet["publicKey"]);
    auto bobTokenAccount = spl->getAssociatedTokenAddressSync(tokenAddress, recvWallet);
    auto beforeSignerBal = std::async([=]() { connection->getTokenAccountBalance(signerTokenAccount); });
    auto beforeBobBal = std::async([=]() { connection->getTokenAccountBalance(bobTokenAccount); });
    auto transaction = ((std::make_shared<Transaction>()))->add(spl->createTransferInstruction(signerTokenAccount, bobTokenAccount, signerWallet["publicKey"], amount, array<any>(), spl->TOKEN_PROGRAM_ID));
    auto signature = std::async([=]() { connection->sendTransaction(transaction, array<any>{ signerWallet }); });
    std::async([=]() { connection->confirmTransaction(signature, std::string("confirmed")); });
    auto afterSignerBal = std::async([=]() { connection->getTokenAccountBalance(signerTokenAccount); });
    auto afterBobBal = std::async([=]() { connection->getTokenAccountBalance(bobTokenAccount); });
};
std::function<std::shared_ptr<Promise<any>>(any, any, any, any)> sendNftTo = [=](auto signerWallet, auto recvWallet, auto nftMinted, auto connection) mutable
{
    try
    {
        auto signerTokenAccount = spl->getAssociatedTokenAddressSync(nftMinted, signerWallet->publicKey);
        auto bobTokenAccount = spl->getAssociatedTokenAddressSync(nftMinted, recvWallet);
        auto toAtaInfo = std::async([=]() { connection->getAccountInfo(bobTokenAccount); });
        auto instructions = as<array<std::shared_ptr<TransactionInstruction>>>(array<std::shared_ptr<TransactionInstruction>>());
        if (!toAtaInfo) {
            instructions->push(as<any>(spl->createAssociatedTokenAccountInstruction(signerWallet->publicKey, bobTokenAccount, recvWallet, nftMinted)));
        }
        auto transferIx = spl->createTransferInstruction(signerTokenAccount, bobTokenAccount, signerWallet->publicKey, 1, array<any>(), spl->TOKEN_PROGRAM_ID);
        instructions->push(transferIx);
        shared latestBlockhash = std::async([=]() { connection->getLatestBlockhash(); });
        auto messageV0 = ((std::make_shared<TransactionMessage>(object{
            object::pair{std::string("payerKey"), signerWallet->publicKey}, 
            object::pair{std::string("recentBlockhash"), latestBlockhash->blockhash}, 
            object::pair{std::string("instructions"), instructions}
        })))->compileToV0Message();
        auto transaction = std::make_shared<VersionedTransaction>(messageV0);
        transaction->sign(array<any>{ signerWallet });
        shared signature = std::async([=]() { connection->sendTransaction(transaction); });
        std::async([=]() { retryOperation([=]() mutable
        {
            std::async([=]() { connection->confirmTransaction(object{
                object::pair{std::string("signature"), std::string("signature")}, 
                object::pair{std::string("blockhash"), latestBlockhash->blockhash}, 
                object::pair{std::string("lastValidBlockHeight"), latestBlockhash->lastValidBlockHeight}
            }, std::string("finalized")); });
        }
        , 3, 2000); });
        return signature;
    }
    catch (const any& error)
    {
        console->error(std::string("Error in sendNftTo:"), error);
        throw any(error);
    }
};
std::function<boolean(any)> isDevnet = [=](auto connection) mutable
{
    return equals(connection->rpcEndpoint, devnetEndpoint);
};

void Main(void)
{
}

MAIN
