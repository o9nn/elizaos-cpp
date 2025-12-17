#include "solana-admin.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<Connection> getConnection() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return new Connection(SOLANA_RPC, "confirmed");

}

std::future<Keypair> getWallet() {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto privateKeyStr = process.env.SOLANA_MAINNET_PRIVATE_KEY;
        if (!privateKeyStr) {
            throw std::runtime_error("SOLANA_MAINNET_PRIVATE_KEY not set in environment");
        }

        const auto secretKey = bs58.decode(privateKeyStr);
        return Keypair.fromSecretKey(secretKey);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<anchor::Program> getProgram(Connection connection, Keypair wallet) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto idlPath = path.join(process.cwd(), "solana/otc-program/target/idl/otc.json");
    const auto idl = JSON.parse(fs.readFileSync(idlPath, "utf8"));

    const auto provider = new anchor.AnchorProvider(;
    connection,
    new anchor.Wallet(wallet),
    { commitment = "confirmed", preflightCommitment = "confirmed" }
    );

    return new anchor.Program(idl, provider);

}

std::future<void> createTreasury(const std::string& tokenMintStr) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "== CREATE DESK TOKEN TREASURY ==\n" << std::endl;

    const auto tokenMint = new PublicKey(tokenMintStr);
    const auto connection = getConnection();
    const auto wallet = getWallet();

    std::cout << "Token Mint:" << tokenMint.toBase58() << std::endl;
    std::cout << "Desk:" << DESK.toBase58() << std::endl;
    std::cout << "Wallet:" << wallet.publicKey.toBase58() << std::endl;

    // Derive the desk's ATA for the token
    const auto deskTokenTreasury = getAssociatedTokenAddressSync(tokenMint, DESK, true);
    std::cout << "Desk Token Treasury (ATA):" << deskTokenTreasury.toBase58() << std::endl;

    // Check if it exists
    const auto ataInfo = connection.getAccountInfo(deskTokenTreasury);
    if (ataInfo) {
        std::cout << "\n✅ Treasury already exists" << std::endl;
        return;
    }

    std::cout << "\nCreating ATA for desk..." << std::endl;

    const auto createAtaIx = createAssociatedTokenAccountInstruction(;
    wallet.publicKey,
    deskTokenTreasury,
    DESK,
    tokenMint;
    );

    const auto tx = new Transaction().add(createAtaIx);
    tx.feePayer = wallet.publicKey;
    tx.recentBlockhash = (connection.getLatestBlockhash()).blockhash;

    const auto sig = sendAndConfirmTransaction(connection, tx, [wallet]);
    std::cout << "✅ Treasury created" << std::endl;
    std::cout << "Transaction:" << sig << std::endl;
    std::cout << "View on Solscan: https://solscan.io/tx/" + sig << std::endl;

}

std::future<void> registerToken(const std::string& tokenMintStr, std::optional<double> priceUsd) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "== REGISTER TOKEN ON DESK ==\n" << std::endl;

    const auto tokenMint = new PublicKey(tokenMintStr);
    const auto connection = getConnection();
    const auto wallet = getWallet();
    const auto program = getProgram(connection, wallet);

    std::cout << "Token Mint:" << tokenMint.toBase58() << std::endl;
    std::cout << "Desk:" << DESK.toBase58() << std::endl;
    std::cout << "Wallet:" << wallet.publicKey.toBase58() << std::endl;
    if (priceUsd) console.log("Initial Price: $" + priceUsd);

    // Derive token registry PDA
    const auto [tokenRegistryPda] = PublicKey.findProgramAddressSync(;
    [Buffer.from("registry"), DESK.toBuffer(), tokenMint.toBuffer()],
    PROGRAM_ID;
    );
    std::cout << "Token Registry PDA:" << tokenRegistryPda.toBase58() << std::endl;

    // Check if already registered
    const auto registryInfo = connection.getAccountInfo(tokenRegistryPda);
    if (registryInfo) {
        std::cout << "\n✅ Token already registered" << std::endl;

        // Set price if provided
        if (priceUsd) {
            setPrice(tokenMintStr, priceUsd);
        }
        return;
    }

    std::cout << "\nRegistering token..." << std::endl;

    const auto tx = (program.methods.Program["methods"]).registerToken(;
    Array.from(EMPTY_PYTH_FEED),
    SystemProgram.programId,
    POOL_TYPE_NONE;
    );
    .accountsStrict({
        desk: DESK,
        payer: wallet.publicKey,
        tokenMint: tokenMint,
        tokenRegistry: tokenRegistryPda,
        systemProgram: SystemProgram.programId,
        });
        .rpc();

        std::cout << "✅ Token registered" << std::endl;
        std::cout << "Transaction:" << tx << std::endl;
        std::cout << "View on Solscan: https://solscan.io/tx/" + tx << std::endl;

        // Set price if provided
        if (priceUsd) {
            std::cout << "" << std::endl;
            setPrice(tokenMintStr, priceUsd);
        }

}

std::future<void> setPrice(const std::string& tokenMintStr, double priceUsd) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "== SET TOKEN PRICE ==\n" << std::endl;

    const auto tokenMint = new PublicKey(tokenMintStr);
    const auto connection = getConnection();
    const auto wallet = getWallet();
    const auto program = getProgram(connection, wallet);

    // Convert to 8 decimal fixed point
    const auto price8d = new anchor.BN(Math.floor(priceUsd * 1e8));

    std::cout << "Token:" << tokenMint.toBase58() << std::endl;
    std::cout << "Price: $" + priceUsd + " (" + price8d.toString() + " in 8d format)" << std::endl;

    // Derive token registry PDA
    const auto [tokenRegistryPda] = PublicKey.findProgramAddressSync(;
    [Buffer.from("registry"), DESK.toBuffer(), tokenMint.toBuffer()],
    program.programId;
    );

    std::cout << "Token Registry:" << tokenRegistryPda.toBase58() << std::endl;
    std::cout << "\nSetting price..." << std::endl;

    const auto tx = (program.Program).methods;
    .setManualTokenPrice(price8d);
    .accounts({
        tokenRegistry: tokenRegistryPda,
        desk: DESK,
        owner: wallet.publicKey,
        });
        .signers([wallet]);
        .rpc();

        std::cout << "✅ Price set" << std::endl;
        std::cout << "Transaction:" << tx << std::endl;
        std::cout << "View on Solscan: https://solscan.io/tx/" + tx << std::endl;

}

std::future<void> showStatus() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "== SOLANA OTC DESK STATUS ==\n" << std::endl;

    const auto connection = getConnection();

    std::cout << "Program ID:" << PROGRAM_ID.toBase58() << std::endl;
    std::cout << "Desk:" << DESK.toBase58() << std::endl;
    std::cout << "RPC:" << SOLANA_RPC << std::endl;

    // Check desk account
    const auto deskInfo = connection.getAccountInfo(DESK);
    if (!deskInfo) {
        std::cout << "\n❌ Desk account not found" << std::endl;
        return;
    }

    std::cout << "\n✅ Desk exists" << std::endl;
    std::cout << "   Data size:" << deskInfo.data.length << "bytes" << std::endl;
    std::cout << "   Lamports:" << deskInfo.lamports / 1e9 << "SOL" << std::endl;

    // Try to decode desk state
    const auto idlPath = path.join(process.cwd(), "solana/otc-program/target/idl/otc.json");
    if (fs.existsSync(idlPath)) {
        const auto idl = JSON.parse(fs.readFileSync(idlPath, "utf8"));
        const auto dummyWallet = new anchor.Wallet(Keypair.generate());
        const auto provider = new anchor.AnchorProvider(connection, dummyWallet, { commitment: "confirmed" });
        const auto program = new anchor.Program(idl, provider);

        type DeskAccount = {
            owner: PublicKey;
            agent: PublicKey;
            nextConsignmentId: anchor.BN;
            nextOfferId: anchor.BN;
            minUsdAmount8d: anchor.BN;
            maxUsdAmount8d: anchor.BN;
            isPaused: boolean;
            };

            const auto deskAccount = (;
        program.account as { desk: { fetch: (addr: PublicKey) => Promise<DeskAccount> } }
        ).desk.fetch(DESK);

        std::cout << "\n📊 Desk State:" << std::endl;
        std::cout << "   Owner:" << deskAccount.owner.toBase58() << std::endl;
        std::cout << "   Agent:" << deskAccount.agent.toBase58() << std::endl;
        std::cout << "   Consignments:" << deskAccount.nextConsignmentId.toNumber() - 1 << std::endl;
        std::cout << "   Offers:" << deskAccount.nextOfferId.toNumber() - 1 << std::endl;
        std::cout << "   Min USD: $" + (deskAccount.minUsdAmount8d.toNumber() / 1e8) << std::endl;
        std::cout << "   Max USD: $" + (deskAccount.maxUsdAmount8d.toNumber() / 1e8) << std::endl;
        std::cout << "   Paused:" << deskAccount.isPaused << std::endl;
    }

    // Check wallet balance
    const auto privateKeyStr = process.env.SOLANA_MAINNET_PRIVATE_KEY;
    if (privateKeyStr) {
        const auto wallet = getWallet();
        const auto balance = connection.getBalance(wallet.publicKey);
        std::cout << "\n💰 Admin Wallet:" << std::endl;
        std::cout << "   Address:" << wallet.publicKey.toBase58() << std::endl;
        std::cout << "   Balance:" << balance / 1e9 << "SOL" << std::endl;
    }

}

void printUsage() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    console.log(`
    Solana OTC Desk Admin CLI;

    Usage:
    bun scripts/solana-admin.ts <command> [options];

    Commands:
    create-treasury <TOKEN_MINT>           Create desk token treasury (ATA);
    register-token <TOKEN_MINT> [PRICE]    Register token on desk with optional price;
    set-price <TOKEN_MINT> <PRICE_USD>     Set manual token price;
    status                                 Show desk status;

    Examples:
    bun scripts/solana-admin.ts status;
    bun scripts/solana-admin.ts create-treasury JUPyiwrYJFskUPiHa7hkeR8VUtAeFoSYbKedZNsDvCN;
    bun scripts/solana-admin.ts register-token JUPyiwrYJFskUPiHa7hkeR8VUtAeFoSYbKedZNsDvCN 0.50;
    bun scripts/solana-admin.ts set-price JUPyiwrYJFskUPiHa7hkeR8VUtAeFoSYbKedZNsDvCN 0.55;

    Environment Variables:
    SOLANA_MAINNET_PRIVATE_KEY   Admin wallet private key (bs58 encoded);
    SOLANA_MAINNET_RPC           RPC endpoint (default: mainnet-beta)
    NEXT_PUBLIC_SOLANA_DESK      Desk address override;
    NEXT_PUBLIC_SOLANA_PROGRAM_ID Program ID override;
    `);

}

std::future<void> main() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto args = process.argv.slice(2);
    const auto command = args[0];

    if (!command || command == "help" || command == "--help" || command == "-h") {
        printUsage();
        process.exit(0);
    }

    switch (command) {
        case "create-treasury":
        if (!args[1]) {
            std::cerr << "Error: TOKEN_MINT required" << std::endl;
            printUsage();
            process.exit(1);
        }
        createTreasury(args[1]);
        break;

        case "register-token":
        if (!args[1]) {
            std::cerr << "Error: TOKEN_MINT required" << std::endl;
            printUsage();
            process.exit(1);
        }
        registerToken(args[1], args[2] ? parseFloat(args[2]) : std::nullopt);
        break;

        case "set-price":
        if (!args[1] || !args[2]) {
            std::cerr << "Error: TOKEN_MINT and PRICE_USD required" << std::endl;
            printUsage();
            process.exit(1);
        }
        setPrice(args[1], parseFloat(args[2]));
        break;

        case "status":
        showStatus();
        break;

        default:
        std::cerr << "Unknown command:" << command << std::endl;
        printUsage();
        process.exit(1);
    }

}

} // namespace elizaos
