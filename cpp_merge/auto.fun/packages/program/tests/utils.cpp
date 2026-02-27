#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/program/tests/utils.h"

any provider = anchor->AnchorProvider->env();
any connection = provider->connection;
double fixedPoint = parseFloat(std:("1000000000"));
string vaultConfigSeed = std:("raydium_vault_config");
string positionSeed = std:("raydium_position");
string claimerInfoSeed = std:("raydium_claimer_info");
string nftFaucetSeed = std:("raydium_vault_nft_seed");
any token0 = std::make_shared<anchor->web3->PublicKey>(std:("EPjFWdd5AufqSSqeM2qN1xzybapC8G4wEGGkZwyTDt1v"));
any token1 = std::make_shared<anchor->web3->PublicKey>(std:("HAs8hvTB8ZH6dBG26KQGik4fxitNYi41jnYd49bvtime"));
any dev_nft_address = std::make_shared<anchor->web3->PublicKey>(std:("BmdxcFyjLtoXykcszqqEvxZApwTnD9ya3BeHUCdJ7ZP5"));
any clmm_nft_address = std::make_shared<anchor->web3->PublicKey>(std:("B1iyEdGyP11DBasG4FMZv5KisFqGkVzhbtqwMvm1gnQ4"));
any cpmm_nft_address_0 = std::make_shared<anchor->web3->PublicKey>(std:("AsWThwVGcWtPmCAPHkikLvuqQzhNxQ8eRcYk7GzDSoDo"));
any cpmm_nft_address_1 = std::make_shared<anchor->web3->PublicKey>(std:("EekJPuXSAP5hfcBbUQZrYfPtQGWK2FHQtYsDJAsjiVz6"));
any claimer_address_0 = std::make_shared<anchor->web3->PublicKey>(std:("6HHoqvXfNF1aQpwhn4k13CL7iyzFpjghLhG2eBG6xMVV"));
std::function<any(boolean)> getNftAddress = [=](auto isDev) mutable
{
    return (equals(isDev, false)) ? cpmm_nft_address_0 : dev_nft_address;
};
string devnetEndpoint = std:("https://api.devnet.solana.com");
std::function<std::shared_ptr<Promise<void>>(any, any, any)> sendSolTo = [=](auto amount, auto signerWallet, auto recvWallet) mutable
{
    auto beforeBal = std::async([=]() { connection->getBalance(recvWallet); });
    console->log(std:("beforeBal: "), parseFloat(beforeBal->toString()) / fixedPoint);
    auto transaction = ((std::make_shared<Transaction>()))->add(SystemProgram->transfer(object{
        object::pair{std:("fromPubkey"), signerWallet["publicKey"]}, 
        object::pair{std:("toPubkey"), recvWallet}, 
        object::pair{std:("lamports"), amount}
    }));
    try
    {
        auto signature = std::async([=]() { sendAndConfirmTransaction(connection, transaction, array<any>{ signerWallet }); });
        console->log(std:("confirmed transaction with signature "), signature);
    }
    catch (const any& error)
    {
        console->log(std:("transaction failed: "), error);
    }
    auto afterBal = std::async([=]() { connection->getBalance(recvWallet); });
    console->log(std:("afterBal: "), parseFloat(afterBal->toString()) / fixedPoint);
};
std::function<std::shared_ptr<Promise<void>>(any, any, any, any)> sendTokenTo = [=](auto amount, auto signerWallet, auto recvWallet, auto tokenAddress) mutable
{
    auto signerTokenAccount = spl->getAssociatedTokenAddressSync(tokenAddress, signerWallet["publicKey"]);
    auto bobTokenAccount = spl->getAssociatedTokenAddressSync(tokenAddress, recvWallet);
    auto beforeSignerBal = std::async([=]() { connection->getTokenAccountBalance(signerTokenAccount); });
    auto beforeBobBal = std::async([=]() { connection->getTokenAccountBalance(bobTokenAccount); });
    auto transaction = ((std::make_shared<Transaction>()))->add(spl->createTransferInstruction(signerTokenAccount, bobTokenAccount, signerWallet["publicKey"], amount, array<any>(), spl->TOKEN_PROGRAM_ID));
    auto signature = std::async([=]() { connection->sendTransaction(transaction, array<any>{ signerWallet }); });
    std::async([=]() { connection->confirmTransaction(signature, std:("confirmed")); });
    auto afterSignerBal = std::async([=]() { connection->getTokenAccountBalance(signerTokenAccount); });
    auto afterBobBal = std::async([=]() { connection->getTokenAccountBalance(bobTokenAccount); });
};
std::function<boolean(any)> isDevnet = [=](auto connection) mutable
{
    return equals(connection->rpcEndpoint, devnetEndpoint);
};
object utils = object{
    object::pair{std:("sendSolTo"), std:("sendSolTo")}, 
    object::pair{std:("sendTokenTo"), std:("sendTokenTo")}, 
    object::pair{std:("isDevnet"), std:("isDevnet")}, 
    object::pair{std:("getNftAddress"), std:("getNftAddress")}, 
    object::pair{std:("vaultConfigSeed"), std:("vaultConfigSeed")}, 
    object::pair{std:("positionSeed"), std:("positionSeed")}, 
    object::pair{std:("claimerInfoSeed"), std:("claimerInfoSeed")}, 
    object::pair{std:("nftFaucetSeed"), std:("nftFaucetSeed")}, 
    object::pair{std:("claimer_address_0"), std:("claimer_address_0")}
};

void Main(void)
{
    anchor->setProvider(provider);
}

MAIN
