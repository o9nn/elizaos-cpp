#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/server/src/solana.h"

any initSolanaConfig()
{
    auto network = process->env->NETWORK;
    auto rpcUrl = getRpcUrl();
    auto umi = createUmi(rpcUrl);
    auto programId = (network == std::string("devnet")) ? OR((process->env->DEVNET_PROGRAM_ID), (process->env->PROGRAM_ID)) : process->env->PROGRAM_ID;
    if (!programId) {
        throw any(std::make_shared<Error>(std::string("missing program_id env var")));
    }
    any wallet;
    if (!process->env->EXECUTOR_PRIVATE_KEY) {
        throw any(std::make_shared<Error>(std::string("missing EXECUTOR_PRIVATE_KEY env var")));
    }
    try
    {
        wallet = Keypair->fromSecretKey(Uint8Array->from(JSON->parse(process->env->EXECUTOR_PRIVATE_KEY)));
        console->log(std::string("Created wallet from process.env.EXECUTOR_PRIVATE_KEY"));
    }
    catch (const any& error)
    {
        console->error(std::string("Failed to create wallet from env:"), error);
    }
    return object{
        object::pair{std::string("umi"), std::string("umi")}, 
        object::pair{std::string("connection"), std::make_shared<Connection>(rpcUrl)}, 
        object::pair{std::string("program"), nullptr}, 
        object::pair{std::string("programId"), std::make_shared<PublicKey>(programId)}, 
        object::pair{std::string("wallet"), std::string("wallet")}, 
        object::pair{std::string("network"), std::string("network")}
    };
};


std::shared_ptr<Autofun> idl = JSON->parse(JSON->stringify(idlJson));
std::function<any(any, any)> getProgram = [=](auto connection, auto wallet) mutable
{
    auto provider = std::make_shared<AnchorProvider>(connection, wallet, object{
        object::pair{std::string("skipPreflight"), true}, 
        object::pair{std::string("commitment"), std::string("confirmed")}
    });
    return std::make_shared<Program<std::shared_ptr<Autofun>>>(idl, provider);
};

void Main(void)
{
}

MAIN
