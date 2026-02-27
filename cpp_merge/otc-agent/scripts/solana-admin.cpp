#include "solana-admin.h"

std::shared_ptr<Promise<std::shared_ptr<Connection>>> getConnection()
{
    return std::make_shared<Connection>(SOLANA_RPC, std:("confirmed"));
};


std::shared_ptr<Promise<std::shared_ptr<Keypair>>> getWallet()
{
    auto privateKeyStr = process->env->SOLANA_MAINNET_PRIVATE_KEY;
    if (!privateKeyStr) {
        throw any(std::make_shared<Error>(std:("SOLANA_MAINNET_PRIVATE_KEY not set in environment")));
    }
    auto secretKey = bs58->decode(privateKeyStr);
    return Keypair->fromSecretKey(secretKey);
};


std::shared_ptr<Promise<std::shared_ptr<anchor::Program>>> getProgram(std::shared_ptr<Connection> connection, std::shared_ptr<Keypair> wallet)
{
    auto idlPath = path->join(process->cwd(), std:("solana/otc-program/target/idl/otc.json"));
    auto idl = JSON->parse(fs::readFileSync(idlPath, std:("utf8")));
    auto provider = std::make_shared<anchor->AnchorProvider>(connection, std::make_shared<anchor->Wallet>(wallet), object{
        object::pair{std:("commitment"), std:("confirmed")}, 
        object::pair{std:("preflightCommitment"), std:("confirmed")}
    });
    return std::make_shared<anchor->Program>(idl, provider);
};


std::shared_ptr<Promise<void>> createTreasury(string tokenMintStr)
{
    console->log(std:("== CREATE DESK TOKEN TREASURY ==\
"));
    auto tokenMint = std::make_shared<PublicKey>(tokenMintStr);
    auto connection = std::async([=]() { getConnection(); });
    auto wallet = std::async([=]() { getWallet(); });
    console->log(std:("Token Mint:"), tokenMint->toBase58());
    console->log(std:("Desk:"), DESK->toBase58());
    console->log(std:("Wallet:"), wallet->publicKey->toBase58());
    auto deskTokenTreasury = getAssociatedTokenAddressSync(tokenMint, DESK, true);
    console->log(std:("Desk Token Treasury (ATA):"), deskTokenTreasury->toBase58());
    auto ataInfo = std::async([=]() { connection->getAccountInfo(deskTokenTreasury); });
    if (ataInfo) {
        console->log(std:("\
✅ Treasury already exists"));
        return std::shared_ptr<Promise<void>>();
    }
    console->log(std:("\
Creating ATA for desk..."));
    auto createAtaIx = createAssociatedTokenAccountInstruction(wallet->publicKey, deskTokenTreasury, DESK, tokenMint);
    auto tx = ((std::make_shared<Transaction>()))->add(createAtaIx);
    tx->feePayer = wallet->publicKey;
    tx->recentBlockhash = (std::async([=]() { connection->getLatestBlockhash(); }))->blockhash;
    auto sig = std::async([=]() { sendAndConfirmTransaction(connection, tx, array<any>{ wallet }); });
    console->log(std:("✅ Treasury created"));
    console->log(std:("Transaction:"), sig);
    console->log(std:("View on Solscan: https://solscan.io/tx/") + sig);
};


std::shared_ptr<Promise<void>> registerToken(string tokenMintStr, double priceUsd)
{
    console->log(std:("== REGISTER TOKEN ON DESK ==\
"));
    auto tokenMint = std::make_shared<PublicKey>(tokenMintStr);
    auto connection = std::async([=]() { getConnection(); });
    auto wallet = std::async([=]() { getWallet(); });
    auto program = std::async([=]() { getProgram(connection, wallet); });
    console->log(std:("Token Mint:"), tokenMint->toBase58());
    console->log(std:("Desk:"), DESK->toBase58());
    console->log(std:("Wallet:"), wallet->publicKey->toBase58());
    if (priceUsd) console->log(std:("Initial Price: $") + priceUsd);
    auto [tokenRegistryPda] = PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(std:("registry")), DESK->toBuffer(), tokenMint->toBuffer() }, PROGRAM_ID);
    console->log(std:("Token Registry PDA:"), tokenRegistryPda->toBase58());
    auto registryInfo = std::async([=]() { connection->getAccountInfo(tokenRegistryPda); });
    if (registryInfo) {
        console->log(std:("\
✅ Token already registered"));
        if (priceUsd) {
            std::async([=]() { setPrice(tokenMintStr, priceUsd); });
        }
        return std::shared_ptr<Promise<void>>();
    }
    console->log(std:("\
Registering token..."));
    auto tx = std::async([=]() { (as<any>(program->methods))->registerToken(Array->from(EMPTY_PYTH_FEED), SystemProgram->programId, POOL_TYPE_NONE)->accountsStrict(object{
        object::pair{std:("desk"), DESK}, 
        object::pair{std:("payer"), wallet->publicKey}, 
        object::pair{std:("tokenMint"), tokenMint}, 
        object::pair{std:("tokenRegistry"), tokenRegistryPda}, 
        object::pair{std:("systemProgram"), SystemProgram->programId}
    })->rpc(); });
    console->log(std:("✅ Token registered"));
    console->log(std:("Transaction:"), tx);
    console->log(std:("View on Solscan: https://solscan.io/tx/") + tx);
    if (priceUsd) {
        console->log(string_empty);
        std::async([=]() { setPrice(tokenMintStr, priceUsd); });
    }
};


std::shared_ptr<Promise<void>> setPrice(string tokenMintStr, double priceUsd)
{
    console->log(std:("== SET TOKEN PRICE ==\
"));
    auto tokenMint = std::make_shared<PublicKey>(tokenMintStr);
    auto connection = std::async([=]() { getConnection(); });
    auto wallet = std::async([=]() { getWallet(); });
    auto program = std::async([=]() { getProgram(connection, wallet); });
    auto price8d = std::make_shared<anchor->BN>(Math->floor(priceUsd * 100000000));
    console->log(std:("Token:"), tokenMint->toBase58());
    console->log(std:("Price: $") + priceUsd + std:(" (") + price8d->toString() + std:(" in 8d format)"));
    auto [tokenRegistryPda] = PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(std:("registry")), DESK->toBuffer(), tokenMint->toBuffer() }, program->programId);
    console->log(std:("Token Registry:"), tokenRegistryPda->toBase58());
    console->log(std:("\
Setting price..."));
    auto tx = std::async([=]() { (as<std::shared_ptr<anchor::Program>>(program))->methods->setManualTokenPrice(price8d)->accounts(object{
        object::pair{std:("tokenRegistry"), tokenRegistryPda}, 
        object::pair{std:("desk"), DESK}, 
        object::pair{std:("owner"), wallet->publicKey}
    })->signers(array<any>{ wallet })->rpc(); });
    console->log(std:("✅ Price set"));
    console->log(std:("Transaction:"), tx);
    console->log(std:("View on Solscan: https://solscan.io/tx/") + tx);
    return std::shared_ptr<Promise<void>>();
};


std::shared_ptr<Promise<void>> showStatus()
{
    console->log(std:("== SOLANA OTC DESK STATUS ==\
"));
    auto connection = std::async([=]() { getConnection(); });
    console->log(std:("Program ID:"), PROGRAM_ID->toBase58());
    console->log(std:("Desk:"), DESK->toBase58());
    console->log(std:("RPC:"), SOLANA_RPC);
    auto deskInfo = std::async([=]() { connection->getAccountInfo(DESK); });
    if (!deskInfo) {
        console->log(std:("\
❌ Desk account not found"));
        return std::shared_ptr<Promise<void>>();
    }
    console->log(std:("\
✅ Desk exists"));
    console->log(std:("   Data size:"), deskInfo->data->length, std:("bytes"));
    console->log(std:("   Lamports:"), deskInfo->lamports / 1000000000, std:("SOL"));
    auto idlPath = path->join(process->cwd(), std:("solana/otc-program/target/idl/otc.json"));
    if (fs::existsSync(idlPath)) {
        auto idl = JSON->parse(fs::readFileSync(idlPath, std:("utf8")));
        auto dummyWallet = std::make_shared<anchor->Wallet>(Keypair->generate());
        auto provider = std::make_shared<anchor->AnchorProvider>(connection, dummyWallet, object{
            object::pair{std:("commitment"), std:("confirmed")}
        });
        auto program = std::make_shared<anchor->Program>(idl, provider);
        auto deskAccount = std::async([=]() { (as<object>(program->account))["desk"]["fetch"](DESK); });
        console->log(std:("\
📊 Desk State:"));
        console->log(std:("   Owner:"), deskAccount["owner"]->toBase58());
        console->log(std:("   Agent:"), deskAccount["agent"]->toBase58());
        console->log(std:("   Consignments:"), deskAccount["nextConsignmentId"]->toNumber() - 1);
        console->log(std:("   Offers:"), deskAccount["nextOfferId"]->toNumber() - 1);
        console->log(std:("   Min USD: $") + (deskAccount["minUsdAmount8d"]->toNumber() / 100000000));
        console->log(std:("   Max USD: $") + (deskAccount["maxUsdAmount8d"]->toNumber() / 100000000));
        console->log(std:("   Paused:"), deskAccount["isPaused"]);
    }
    auto privateKeyStr = process->env->SOLANA_MAINNET_PRIVATE_KEY;
    if (privateKeyStr) {
        auto wallet = std::async([=]() { getWallet(); });
        auto balance = std::async([=]() { connection->getBalance(wallet->publicKey); });
        console->log(std:("\
💰 Admin Wallet:"));
        console->log(std:("   Address:"), wallet->publicKey->toBase58());
        console->log(std:("   Balance:"), balance / 1000000000, std:("SOL"));
    }
};


void printUsage()
{
    console->log(std:("\
Solana OTC Desk Admin CLI\
\
Usage:\
  bun scripts/solana-admin.ts <command> [options]\
\
Commands:\
  create-treasury <TOKEN_MINT>           Create desk token treasury (ATA)\
  register-token <TOKEN_MINT> [PRICE]    Register token on desk with optional price\
  set-price <TOKEN_MINT> <PRICE_USD>     Set manual token price\
  status                                 Show desk status\
\
Examples:\
  bun scripts/solana-admin.ts status\
  bun scripts/solana-admin.ts create-treasury JUPyiwrYJFskUPiHa7hkeR8VUtAeFoSYbKedZNsDvCN\
  bun scripts/solana-admin.ts register-token JUPyiwrYJFskUPiHa7hkeR8VUtAeFoSYbKedZNsDvCN 0.50\
  bun scripts/solana-admin.ts set-price JUPyiwrYJFskUPiHa7hkeR8VUtAeFoSYbKedZNsDvCN 0.55\
\
Environment Variables:\
  SOLANA_MAINNET_PRIVATE_KEY   Admin wallet private key (bs58 encoded)\
  SOLANA_MAINNET_RPC           RPC endpoint (default: mainnet-beta)\
  NEXT_PUBLIC_SOLANA_DESK      Desk address override\
  NEXT_PUBLIC_SOLANA_PROGRAM_ID Program ID override\
"));
};


std::shared_ptr<Promise<void>> main()
{
    auto args = process->argv->slice(2);
    auto command = const_(args)[0];
    if (OR((OR((OR((!command), (command == std:("help")))), (command == std:("--help")))), (command == std:("-h")))) {
        printUsage();
        process->exit(0);
    }
    static switch_type __switch10595_11515 = {
        { any(std:("create-treasury")), 1 },
        { any(std:("register-token")), 2 },
        { any(std:("set-price")), 3 },
        { any(std:("status")), 4 }
    };
    switch (__switch10595_11515[command])
    {
    case 1:
        if (!const_(args)[1]) {
            console->error(std:("Error: TOKEN_MINT required"));
            printUsage();
            process->exit(1);
        }
        std::async([=]() { createTreasury(const_(args)[1]); });
        break;
    case 2:
        if (!const_(args)[1]) {
            console->error(std:("Error: TOKEN_MINT required"));
            printUsage();
            process->exit(1);
        }
        std::async([=]() { registerToken(const_(args)[1], (const_(args)[2]) ? any(parseFloat(const_(args)[2])) (undefined)); });
        break;
    case 3:
        if (OR((!const_(args)[1]), (!const_(args)[2]))) {
            console->error(std:("Error: TOKEN_MINT and PRICE_USD required"));
            printUsage();
            process->exit(1);
        }
        std::async([=]() { setPrice(const_(args)[1], parseFloat(const_(args)[2])); });
        break;
    case 4:
        std::async([=]() { showStatus(); });
        break;
    default:
        console->error(std:("Unknown command:"), command);
        printUsage();
        process->exit(1);
    }
    return std::shared_ptr<Promise<void>>();
};


string SOLANA_RPC = OR((process->env->SOLANA_MAINNET_RPC), (std:("https://api.mainnet-beta.solana.com")));
any PROGRAM_ID = std::make_shared<PublicKey>(OR((process->env->NEXT_PUBLIC_SOLANA_PROGRAM_ID), (std:("6qn8ELVXd957oRjLaomCpKpcVZshUjNvSzw1nc7QVyXc"))));
any DESK = std::make_shared<PublicKey>(OR((OR((process->env->NEXT_PUBLIC_SOLANA_DESK_MAINNET), (process->env->NEXT_PUBLIC_SOLANA_DESK))), (std:("G89QsVcKN1MZe6d8eKyzv93u7TEeXSsXbsDsBPbuTMUU"))));
double POOL_TYPE_NONE = 0;
std::shared_ptr<Buffer> EMPTY_PYTH_FEED = Buffer::alloc(32, 0);

void Main(void)
{
    config(object{
        object::pair{std:("path"), std:(".env.local")}
    });
    main()->_catch([=](auto err) mutable
    {
        console->error(std:("Error:"), OR((err["message"]), (err)));
        process->exit(1);
    }
    );
}

MAIN
