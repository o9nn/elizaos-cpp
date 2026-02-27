#include "claimFees.hpp"

std::shared_ptr<Promise<string>> claimFees(std::shared_ptr<PublicKey> nftMint, std::shared_ptr<PublicKey> poolId, std::shared_ptr<Connection> connection, std::shared_ptr<PublicKey> claimer, std::shared_ptr<WebSocketClient> websocket, std::shared_ptr<Token> token)
{
    try
    {
        auto wallet = Keypair->fromSecretKey(Uint8Array->from(JSON->parse(process->env->EXECUTOR_PRIVATE_KEY)));
        auto provider = std::make_shared<AnchorProvider>(connection, std::make_shared<Wallet>(wallet), AnchorProvider->defaultOptions());
        auto program = std::make_shared<Program<std::shared_ptr<RaydiumVault>>>(as<any>(raydium_vault_IDL), provider);
        auto txSignature = string_empty;
        try
        {
            txSignature = std::async([=]() { claim(as<any>(program), wallet, nftMint, poolId, connection, claimer, token); });
        }
        catch (const any& error)
        {
            console->error(std:("Error during claim attempt:"), error);
        }
        if (!txSignature) {
            throw any(std::make_shared<Error>(std:("Failed to claim after multiple attempts.")));
        }
        websocket->to(std:("claimer:") + claimer->toBase58() + string_empty)["emit"](std:("claim"), object{
            object::pair{std:("txSignature"), std:("txSignature")}, 
            object::pair{std:("poolId"), poolId->toBase58()}, 
            object::pair{std:("claimer"), claimer->toBase58()}
        });
        console->log(std:("Transaction Signature:"), txSignature);
        return txSignature;
    }
    catch (const any& error)
    {
        console->error(std:("Error during claim:"), error);
        throw any(error);
    }
};


std::shared_ptr<RaydiumVault> raydium_vault_IDL = JSON->parse(JSON->stringify(raydium_vault_IDL_JSON));

void Main(void)
{
}

MAIN
