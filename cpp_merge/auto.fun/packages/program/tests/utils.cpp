#include "utils.hpp"
#include <string>

any provider = anchor->AnchorProvider->env();
any connection = provider->connection;
double fixedPoint = parseFloat(std::string("1000000000"));
string vaultConfigSeed = std::string("raydium_vault_config");
string positionSeed = std::string("raydium_position");
string claimerInfoSeed = std::string("raydium_claimer_info");
string nftFaucetSeed = std::string("raydium_vault_nft_seed");
any token0 = std::make_shared<anchor->web3->PublicKey>(std::string("EPjFWdd5AufqSSqeM2qN1xzybapC8G4wEGGkZwyTDt1v"));
any token1 = std::make_shared<anchor->web3->PublicKey>(std::string("HAs8hvTB8ZH6dBG26KQGik4fxitNYi41jnYd49bvtime"));
any dev_nft_address = std::make_shared<anchor->web3->PublicKey>(std::string("BmdxcFyjLtoXykcszqqEvxZApwTnD9ya3BeHUCdJ7ZP5"));
any clmm_nft_address = std::make_shared<anchor->web3->PublicKey>(std::string("B1iyEdGyP11DBasG4FMZv5KisFqGkVzhbtqwMvm1gnQ4"));
any cpmm_nft_address_0 = std::make_shared<anchor->web3->PublicKey>(std::string("AsWThwVGcWtPmCAPHkikLvuqQzhNxQ8eRcYk7GzDSoDo"));
any cpmm_nft_address_1 = std::make_shared<anchor->web3->PublicKey>(std::string("EekJPuXSAP5hfcBbUQZrYfPtQGWK2FHQtYsDJAsjiVz6"));
any claimer_address_0 = std::make_shared<anchor->web3->PublicKey>(std::string("6HHoqvXfNF1aQpwhn4k13CL7iyzFpjghLhG2eBG6xMVV"));
std::function<any(boolean)> getNftAddress = [=](auto isDev) mutable
{
    return (equals(isDev, false)) ? cpmm_nft_address_0 : dev_nft_address;
};
string devnetEndpoint = std::string("https://api.devnet.solana.com");
std::function<std::shared_ptr<Promise<void>>(any, any, any)> sendSolTo = [=](auto amount, auto signerWallet, auto recvWallet) mutable
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
    }
    catch (const any& error)
    {
        console->log(std::string("transaction failed: "), error);
    }
    auto afterBal = std::async([=]() { connection->getBalance(recvWallet); });
    console->log(std::string("afterBal: "), parseFloat(afterBal->toString()) / fixedPoint);
};
std::function<std::shared_ptr<Promise<void>>(any, any, any, any)> sendTokenTo = [=](auto amount, auto signerWallet, auto recvWallet, auto tokenAddress) mutable
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
std::function<boolean(any)> isDevnet = [=](auto connection) mutable
{
    return equals(connection->rpcEndpoint, devnetEndpoint);
};
object utils = object{
    object::pair{std::string("sendSolTo"), std::string("sendSolTo")}, 
    object::pair{std::string("sendTokenTo"), std::string("sendTokenTo")}, 
    object::pair{std::string("isDevnet"), std::string("isDevnet")}, 
    object::pair{std::string("getNftAddress"), std::string("getNftAddress")}, 
    object::pair{std::string("vaultConfigSeed"), std::string("vaultConfigSeed")}, 
    object::pair{std::string("positionSeed"), std::string("positionSeed")}, 
    object::pair{std::string("claimerInfoSeed"), std::string("claimerInfoSeed")}, 
    object::pair{std::string("nftFaucetSeed"), std::string("nftFaucetSeed")}, 
    object::pair{std::string("claimer_address_0"), std::string("claimer_address_0")}
};

void Main(void)
{
    anchor->setProvider(provider);
}

MAIN
