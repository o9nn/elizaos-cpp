#include "/home/runner/work/elizaos-cpp/elizaos-cpp/otc-agent/scripts/solana-admin.h"

std::shared_ptr<Promise<std::shared_ptr<Connection>>> getConnection()
{
    return std::make_shared<Connection>(SOLANA_RPC, std::string("confirmed"));
};


std::shared_ptr<Promise<std::shared_ptr<Keypair>>> getWallet()
{
    auto privateKeyStr = process->env->SOLANA_MAINNET_PRIVATE_KEY;
    if (!privateKeyStr) {
        throw any(std::make_shared<Error>(std::string("SOLANA_MAINNET_PRIVATE_KEY not set in environment")));
    }
    auto secretKey = bs58->decode(privateKeyStr);
    return Keypair->fromSecretKey(secretKey);
};


std::shared_ptr<Promise<std::shared_ptr<anchor::Program>>> getProgram(std::shared_ptr<Connection> connection, std::shared_ptr<Keypair> wallet)
{
    auto idlPath = path->join(process->cwd(), std::string("solana/otc-program/target/idl/otc.json"));
    auto idl = JSON->parse(fs::readFileSync(idlPath, std::string("utf8")));
    auto provider = std::make_shared<anchor->AnchorProvider>(connection, std::make_shared<anchor->Wallet>(wallet), object{
        object::pair{std::string("commitment"), std::string("confirmed")}, 
        object::pair{std::string("preflightCommitment"), std::string("confirmed")}
    });
    return std::make_shared<anchor->Program>(idl, provider);
};


std::shared_ptr<Promise<void>> createTreasury(string tokenMintStr)
{
    console->log(std::string("=== CREATE DESK TOKEN TREASURY ===\
"));
    auto tokenMint = std::make_shared<PublicKey>(tokenMintStr);
    auto connection = std::async([=]() { getConnection(); });
    auto wallet = std::async([=]() { getWallet(); });
    console->log(std::string("Token Mint:"), tokenMint->toBase58());
    console->log(std::string("Desk:"), DESK->toBase58());
    console->log(std::string("Wallet:"), wallet->publicKey->toBase58());
    auto deskTokenTreasury = getAssociatedTokenAddressSync(tokenMint, DESK, true);
    console->log(std::string("Desk Token Treasury (ATA):"), deskTokenTreasury->toBase58());
    auto ataInfo = std::async([=]() { connection->getAccountInfo(deskTokenTreasury); });
    if (ataInfo) {
        console->log(std::string("\
✅ Treasury already exists"));
        return std::shared_ptr<Promise<void>>();
    }
    console->log(std::string("\
Creating ATA for desk..."));
    auto createAtaIx = createAssociatedTokenAccountInstruction(wallet->publicKey, deskTokenTreasury, DESK, tokenMint);
    auto tx = ((std::make_shared<Transaction>()))->add(createAtaIx);
    tx->feePayer = wallet->publicKey;
    tx->recentBlockhash = (std::async([=]() { connection->getLatestBlockhash(); }))->blockhash;
    auto sig = std::async([=]() { sendAndConfirmTransaction(connection, tx, array<any>{ wallet }); });
    console->log(std::string("✅ Treasury created"));
    console->log(std::string("Transaction:"), sig);
    console->log(std::string("View on Solscan: https://solscan.io/tx/") + sig);
};


std::shared_ptr<Promise<void>> registerToken(string tokenMintStr, double priceUsd)
{
    console->log(std::string("=== REGISTER TOKEN ON DESK ===\
"));
    auto tokenMint = std::make_shared<PublicKey>(tokenMintStr);
    auto connection = std::async([=]() { getConnection(); });
    auto wallet = std::async([=]() { getWallet(); });
    auto program = std::async([=]() { getProgram(connection, wallet); });
    console->log(std::string("Token Mint:"), tokenMint->toBase58());
    console->log(std::string("Desk:"), DESK->toBase58());
    console->log(std::string("Wallet:"), wallet->publicKey->toBase58());
    if (priceUsd) console->log(std::string("Initial Price: $") + priceUsd);
    auto [tokenRegistryPda] = PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(std::string("registry")), DESK->toBuffer(), tokenMint->toBuffer() }, PROGRAM_ID);
    console->log(std::string("Token Registry PDA:"), tokenRegistryPda->toBase58());
    auto registryInfo = std::async([=]() { connection->getAccountInfo(tokenRegistryPda); });
    if (registryInfo) {
        console->log(std::string("\
✅ Token already registered"));
        if (priceUsd) {
            std::async([=]() { setPrice(tokenMintStr, priceUsd); });
        }
        return std::shared_ptr<Promise<void>>();
    }
    console->log(std::string("\
Registering token..."));
    auto tx = std::async([=]() { (as<any>(program->methods))->registerToken(Array->from(EMPTY_PYTH_FEED), SystemProgram->programId, POOL_TYPE_NONE)->accountsStrict(object{
        object::pair{std::string("desk"), DESK}, 
        object::pair{std::string("payer"), wallet->publicKey}, 
        object::pair{std::string("tokenMint"), tokenMint}, 
        object::pair{std::string("tokenRegistry"), tokenRegistryPda}, 
        object::pair{std::string("systemProgram"), SystemProgram->programId}
    })->rpc(); });
    console->log(std::string("✅ Token registered"));
    console->log(std::string("Transaction:"), tx);
    console->log(std::string("View on Solscan: https://solscan.io/tx/") + tx);
    if (priceUsd) {
        console->log(string_empty);
        std::async([=]() { setPrice(tokenMintStr, priceUsd); });
    }
};


std::shared_ptr<Promise<void>> setPrice(string tokenMintStr, double priceUsd)
{
    console->log(std::string("=== SET TOKEN PRICE ===\
"));
    auto tokenMint = std::make_shared<PublicKey>(tokenMintStr);
    auto connection = std::async([=]() { getConnection(); });
    auto wallet = std::async([=]() { getWallet(); });
    auto program = std::async([=]() { getProgram(connection, wallet); });
    auto price8d = std::make_shared<anchor->BN>(Math->floor(priceUsd * 100000000));
    console->log(std::string("Token:"), tokenMint->toBase58());
    console->log(std::string("Price: $") + priceUsd + std::string(" (") + price8d->toString() + std::string(" in 8d format)"));
    auto [tokenRegistryPda] = PublicKey->findProgramAddressSync(array<std::shared_ptr<Buffer>>{ Buffer::from(std::string("registry")), DESK->toBuffer(), tokenMint->toBuffer() }, program->programId);
    console->log(std::string("Token Registry:"), tokenRegistryPda->toBase58());
    console->log(std::string("\
Setting price..."));
    auto tx = std::async([=]() { (as<std::shared_ptr<anchor::Program>>(program))->methods->setManualTokenPrice(price8d)->accounts(object{
        object::pair{std::string("tokenRegistry"), tokenRegistryPda}, 
        object::pair{std::string("desk"), DESK}, 
        object::pair{std::string("owner"), wallet->publicKey}
    })->signers(array<any>{ wallet })->rpc(); });
    console->log(std::string("✅ Price set"));
    console->log(std::string("Transaction:"), tx);
    console->log(std::string("View on Solscan: https://solscan.io/tx/") + tx);
    return std::shared_ptr<Promise<void>>();
};


std::shared_ptr<Promise<void>> showStatus()
{
    console->log(std::string("=== SOLANA OTC DESK STATUS ===\
"));
    auto connection = std::async([=]() { getConnection(); });
    console->log(std::string("Program ID:"), PROGRAM_ID->toBase58());
    console->log(std::string("Desk:"), DESK->toBase58());
    console->log(std::string("RPC:"), SOLANA_RPC);
    auto deskInfo = std::async([=]() { connection->getAccountInfo(DESK); });
    if (!deskInfo) {
        console->log(std::string("\
❌ Desk account not found"));
        return std::shared_ptr<Promise<void>>();
    }
    console->log(std::string("\
✅ Desk exists"));
    console->log(std::string("   Data size:"), deskInfo->data->length, std::string("bytes"));
    console->log(std::string("   Lamports:"), deskInfo->lamports / 1000000000, std::string("SOL"));
    auto idlPath = path->join(process->cwd(), std::string("solana/otc-program/target/idl/otc.json"));
    if (fs::existsSync(idlPath)) {
        auto idl = JSON->parse(fs::readFileSync(idlPath, std::string("utf8")));
        auto dummyWallet = std::make_shared<anchor->Wallet>(Keypair->generate());
        auto provider = std::make_shared<anchor->AnchorProvider>(connection, dummyWallet, object{
            object::pair{std::string("commitment"), std::string("confirmed")}
        });
        auto program = std::make_shared<anchor->Program>(idl, provider);
        auto deskAccount = std::async([=]() { (as<object>(program->account))["desk"]["fetch"](DESK); });
        console->log(std::string("\
📊 Desk State:"));
        console->log(std::string("   Owner:"), deskAccount["owner"]->toBase58());
        console->log(std::string("   Agent:"), deskAccount["agent"]->toBase58());
        console->log(std::string("   Consignments:"), deskAccount["nextConsignmentId"]->toNumber() - 1);
        console->log(std::string("   Offers:"), deskAccount["nextOfferId"]->toNumber() - 1);
        console->log(std::string("   Min USD: $") + (deskAccount["minUsdAmount8d"]->toNumber() / 100000000));
        console->log(std::string("   Max USD: $") + (deskAccount["maxUsdAmount8d"]->toNumber() / 100000000));
        console->log(std::string("   Paused:"), deskAccount["isPaused"]);
    }
    auto privateKeyStr = process->env->SOLANA_MAINNET_PRIVATE_KEY;
    if (privateKeyStr) {
        auto wallet = std::async([=]() { getWallet(); });
        auto balance = std::async([=]() { connection->getBalance(wallet->publicKey); });
        console->log(std::string("\
💰 Admin Wallet:"));
        console->log(std::string("   Address:"), wallet->publicKey->toBase58());
        console->log(std::string("   Balance:"), balance / 1000000000, std::string("SOL"));
    }
};


void printUsage()
{
    console->log(std::string("\
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
    if (OR((OR((OR((!command), (command == std::string("help")))), (command == std::string("--help")))), (command == std::string("-h")))) {
        printUsage();
        process->exit(0);
    }
    static switch_type __switch10595_11515 = {
        { any(std::string("create-treasury")), 1 },
        { any(std::string("register-token")), 2 },
        { any(std::string("set-price")), 3 },
        { any(std::string("status")), 4 }
    };
    switch (__switch10595_11515[command])
    {
    case 1:
        if (!const_(args)[1]) {
            console->error(std::string("Error: TOKEN_MINT required"));
            printUsage();
            process->exit(1);
        }
        std::async([=]() { createTreasury(const_(args)[1]); });
        break;
    case 2:
        if (!const_(args)[1]) {
            console->error(std::string("Error: TOKEN_MINT required"));
            printUsage();
            process->exit(1);
        }
        std::async([=]() { registerToken(const_(args)[1], (const_(args)[2]) ? any(parseFloat(const_(args)[2])) : any(undefined)); });
        break;
    case 3:
        if (OR((!const_(args)[1]), (!const_(args)[2]))) {
            console->error(std::string("Error: TOKEN_MINT and PRICE_USD required"));
            printUsage();
            process->exit(1);
        }
        std::async([=]() { setPrice(const_(args)[1], parseFloat(const_(args)[2])); });
        break;
    case 4:
        std::async([=]() { showStatus(); });
        break;
    default:
        console->error(std::string("Unknown command:"), command);
        printUsage();
        process->exit(1);
    }
    return std::shared_ptr<Promise<void>>();
};


string SOLANA_RPC = OR((process->env->SOLANA_MAINNET_RPC), (std::string("https://api.mainnet-beta.solana.com")));
any PROGRAM_ID = std::make_shared<PublicKey>(OR((process->env->NEXT_PUBLIC_SOLANA_PROGRAM_ID), (std::string("6qn8ELVXd957oRjLaomCpKpcVZshUjNvSzw1nc7QVyXc"))));
any DESK = std::make_shared<PublicKey>(OR((OR((process->env->NEXT_PUBLIC_SOLANA_DESK_MAINNET), (process->env->NEXT_PUBLIC_SOLANA_DESK))), (std::string("G89QsVcKN1MZe6d8eKyzv93u7TEeXSsXbsDsBPbuTMUU"))));
double POOL_TYPE_NONE = 0;
std::shared_ptr<Buffer> EMPTY_PYTH_FEED = Buffer::alloc(32, 0);

void Main(void)
{
    config(object{
        object::pair{std::string("path"), std::string(".env.local")}
    });
    main()->_catch([=](auto err) mutable
    {
        console->error(std::string("Error:"), OR((err["message"]), (err)));
        process->exit(1);
    }
    );
}

MAIN
