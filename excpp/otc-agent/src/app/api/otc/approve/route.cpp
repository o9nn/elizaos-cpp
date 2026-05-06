#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> POST(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        // Resolve OTC address (chain-specific first, then devnet file fallback for local development)
        const auto resolveOtcAddress = async (): Promise<Address> => {;
            // Try to get chain-specific address first (production/configured networks)
            try {
                return getContractAddress();
                } catch {
                    // Fallback to devnet file for local development
                    const auto deployed = path.join(;
                    process.cwd(),
                    "contracts/ignition/deployments/chain-31337/deployed_addresses.json",
                    );
                    try {
                        const auto raw = fs.readFile(deployed, "utf8");
                        const auto json = /* JSON.parse */ raw;
                        const auto addr =;
                        (json["OTCModule#OTC"]) ||;
                        (json["OTCDeskModule#OTC"]) ||;
                        (json["ElizaOTCModule#ElizaOTC"]) ||;
                        (json["OTCModule#desk"]);
                        if (addr) {
                            std::cout << "[Approve API] Using devnet address: " + addr << std::endl;
                            return addr;
                        }
                        } catch {
                            // File doesn't exist or can't be read
                        }
                        throw new Error(
                        "No OTC address configured. Set NETWORK and chain-specific NEXT_PUBLIC_*_OTC_ADDRESS env var.",
                        );
                    }
                    };

                    const auto OTC_ADDRESS = resolveOtcAddress();
                    const auto RAW_PK = process.env.EVM_PRIVATE_KEY | std::nullopt;
                    const auto EVM_PRIVATE_KEY =;
                    RAW_PK && /^0x[0-9a-fA-F]{64}$/.test(RAW_PK);
                    "? (RAW_PK as " + "0x" + string;
                    : std::nullopt;
                    if (RAW_PK && !EVM_PRIVATE_KEY) {
                        console.warn(
                        "[Approve API] Ignoring invalid EVM_PRIVATE_KEY format. Falling back to impersonation.",
                        );
                    }

                    // Parse body
                    const auto contentType = request.headers.get("content-type") || "";
                    auto offerId: string | number | bigint;
                    auto chainType: string | std::nullopt;
                    auto offerAddress: string | std::nullopt;

                    if (contentType.includes("application/json")) {
                        const auto body = request.json();
                        offerId = body.offerId;
                        chainType = body.chain;
                        offerAddress = body.offerAddress;
                        } else if ((std::find(contentType.begin(), contentType.end(), "application/x-www-form-urlencoded") != contentType.end())) {
                            // Use type assertion for FormData as Next.js returns a compatible type
                            const auto form = (request.formData()) as {;
                                get: (name: string) => FormDataEntryValue | nullptr;
                                };
                                const auto v = form.get("offerId");
                                if (!v) throw new Error("offerId required in form data");
                                offerId = std::to_string(v);
                                } else {
                                    const auto { searchParams } = new URL(request.url);
                                    const auto v = searchParams.get("offerId");
                                    if (!v) throw new Error("offerId required in query params");
                                    offerId = v;
                                }

                                std::cout << "[Approve API] Approving offer:" << offerId << "chain:" << chainType << std::endl;

                                // Handle Solana approval
                                if (chainType == "solana") {
                                    if (!offerAddress) throw new Error("offerAddress required for Solana");
                                    console.log(
                                    "[Approve API] Processing Solana approval for offer:",
                                    offerAddress,
                                    );

                                    // Import Anchor and Solana libs dynamically
                                    const auto anchor = import("@coral-xyz/anchor");
                                    const auto { Connection, PublicKey, Keypair } = import("@solana/web3.js");

                                    const auto SOLANA_RPC =;
                                    process.env.NEXT_PUBLIC_SOLANA_RPC || "http://127.0.0.1:8899";
                                    const auto SOLANA_DESK = process.env.NEXT_PUBLIC_SOLANA_DESK;

                                    if (!SOLANA_DESK) throw new Error("SOLANA_DESK not configured");

                                    const auto connection = new Connection(SOLANA_RPC, "confirmed");

                                    // Load owner/approver keypair from id.json
                                    const auto idlPath = path.join(;
                                    process.cwd(),
                                    "solana/otc-program/target/idl/otc.json",
                                    );
                                    const auto keypairPath = path.join(process.cwd(), "solana/otc-program/id.json");
                                    const auto idl = /* JSON.parse */ fs.readFile(idlPath, "utf8");
                                    const auto keypairData = /* JSON.parse */ fs.readFile(keypairPath, "utf8");
                                    const auto approverKeypair = Keypair.fromSecretKey(Uint8Array.from(keypairData));

                                    // Create provider with the approver keypair
                                    // Wallet interface matches @coral-xyz/anchor's Wallet type
                                    interface AnchorWallet {
                                        publicKey: SolanaPublicKey;
                                        signTransaction<T extends Transaction | VersionedTransaction>(;
                                        tx: T,
                                        ): Promise<T>;
                                        signAllTransactions<T extends Transaction | VersionedTransaction>(;
                                        txs: T[],
                                        ): Promise<T[]>;
                                    }

                                    const AnchorWallet wallet = {;
                                        publicKey: approverKeypair.publicKey,
                                        signTransaction: async <T extends Transaction | VersionedTransaction>(
                                        tx: T,
                                        ) => {
                                            (tx).partialSign(approverKeypair);
                                            return tx;
                                            },
                                            signAllTransactions: async <T extends Transaction | VersionedTransaction>(
                                            txs: T[],
                                            ) => {
                                                txs.forEach((tx) => (tx).partialSign(approverKeypair));
                                                return txs;
                                                },
                                                };

                                                const auto provider = new anchor.AnchorProvider(connection, wallet, {;
                                                    commitment: "confirmed",
                                                    });
                                                    anchor.setProvider(provider);

                                                    const auto program = new anchor.Program(idl, provider);

                                                    // Approve the offer
                                                    const auto desk = new PublicKey(SOLANA_DESK);
                                                    const auto offer = new PublicKey(offerAddress);

                                                    const auto approveTx = program.methods;
                                                    .approveOffer(new anchor.BN(offerId));
                                                    .accounts({
                                                        desk,
                                                        offer,
                                                        approver: approverKeypair.publicKey,
                                                        });
                                                        .signers([approverKeypair]);
                                                        .rpc();

                                                        std::cout << "[Approve API] ✅ Solana offer approved:" << approveTx << std::endl;

                                                        // Fetch offer to get payment details and token mint
                                                        // In token-agnostic architecture, each offer stores its own token_mint
                                                        type ProgramAccountsFetch = {
                                                            offer: {
                                                                fetch: (address: SolanaPublicKey) => Promise<{
                                                                    currency: number;
                                                                    id: import("@coral-xyz/anchor").BN;
                                                                    tokenMint: SolanaPublicKey;
                                                                    }>;
                                                                    };
                                                                    desk: {
                                                                        fetch: (
                                                                        address: SolanaPublicKey,
                                                                        ) => Promise<{ usdcMint: SolanaPublicKey }>;
                                                                        };
                                                                        };
                                                                        const auto programAccounts = program.account;
                                                                        const auto offerData = programAccounts.offer.fetch(offer);

                                                                        // Auto-fulfill (backend pays)
                                                                        std::cout << "[Approve API] Auto-fulfilling Solana offer..." << std::endl;

                                                                        const auto { getAssociatedTokenAddress } = import("@solana/spl-token");
                                                                        const auto deskData = programAccounts.desk.fetch(desk);
                                                                        // Token mint comes from the offer itself (multi-token support)
                                                                        const auto tokenMint = new PublicKey(offerData.tokenMint);
                                                                        const auto deskTokenTreasury = getAssociatedTokenAddress(;
                                                                        tokenMint,
                                                                        desk,
                                                                        true,
                                                                        );

                                                                        auto fulfillTx: string;

                                                                        if (offerData.currency == 0) {
                                                                            // Pay with SOL
                                                                            fulfillTx = program.methods;
                                                                            .fulfillOfferSol(new anchor.BN(offerId));
                                                                            .accounts({
                                                                                desk,
                                                                                offer,
                                                                                deskTokenTreasury,
                                                                                payer: approverKeypair.publicKey,
                                                                                systemProgram: new PublicKey("11111111111111111111111111111111"),
                                                                                });
                                                                                .signers([approverKeypair]);
                                                                                .rpc();
                                                                                std::cout << "[Approve API] ✅ Paid with SOL:" << fulfillTx << std::endl;
                                                                                } else {
                                                                                    // Pay with USDC
                                                                                    const auto usdcMint = new PublicKey(deskData.usdcMint);
                                                                                    const auto deskUsdcTreasury = getAssociatedTokenAddress(;
                                                                                    usdcMint,
                                                                                    desk,
                                                                                    true,
                                                                                    );
                                                                                    const auto payerUsdcAta = getAssociatedTokenAddress(;
                                                                                    usdcMint,
                                                                                    approverKeypair.publicKey,
                                                                                    false,
                                                                                    );

                                                                                    fulfillTx = program.methods;
                                                                                    .fulfillOfferUsdc(new anchor.BN(offerId));
                                                                                    .accounts({
                                                                                        desk,
                                                                                        offer,
                                                                                        deskTokenTreasury,
                                                                                        deskUsdcTreasury,
                                                                                        payerUsdcAta,
                                                                                        payer: approverKeypair.publicKey,
                                                                                        tokenProgram: new PublicKey(
                                                                                        "TokenkegQfeZyiNwAJbNbGKPFXCWuBvf9Ss623VQ5DA",
                                                                                        ),
                                                                                        systemProgram: new PublicKey("11111111111111111111111111111111"),
                                                                                        });
                                                                                        .signers([approverKeypair]);
                                                                                        .rpc();
                                                                                        std::cout << "[Approve API] ✅ Paid with USDC:" << fulfillTx << std::endl;
                                                                                    }

                                                                                    return NextResponse.json({;
                                                                                        success: true,
                                                                                        approved: true,
                                                                                        autoFulfilled: true,
                                                                                        fulfillTx,
                                                                                        chain: "solana",
                                                                                        offerAddress,
                                                                                        approvalTx: approveTx,
                                                                                        });
                                                                                    }

                                                                                    const auto chain = getChain();
                                                                                    const auto rpcUrl = getRpcUrl();
                                                                                    const auto publicClient = createPublicClient({ chain, transport: http(rpcUrl) });
                                                                                    const auto abi = otcArtifact.abi;

                                                                                    // Resolve approver account: prefer PK; else use testWalletPrivateKey from deployment; else impersonate
                                                                                    auto account: PrivateKeyAccount | Address;
                                                                                    // eslint-disable-next-line @typescript-eslint/no-explicit-any
                                                                                    auto walletClient: any; // Using any to avoid viem deep type instantiation issues;
                                                                                    auto approverAddr: Address;

                                                                                    if (EVM_PRIVATE_KEY) {
                                                                                        account = privateKeyToAccount(EVM_PRIVATE_KEY);
                                                                                        walletClient = createWalletClient({
                                                                                            account,
                                                                                            chain,
                                                                                            transport: http(rpcUrl),
                                                                                            });
                                                                                            approverAddr = account.address;
                                                                                            } else {
                                                                                                const auto deploymentInfoPath = path.join(;
                                                                                                process.cwd(),
                                                                                                "contracts/deployments/eliza-otc-deployment.json",
                                                                                                );
                                                                                                const auto raw = fs.readFile(deploymentInfoPath, "utf8");
                                                                                                const auto json = /* JSON.parse */ raw;
                                                                                                const auto testPk = "json.testWalletPrivateKey as " + "0x" + string;

                                                                                                if (testPk && /^0x[0-9a-fA-F]{64}$/.test(testPk)) {
                                                                                                    account = privateKeyToAccount(testPk);
                                                                                                    walletClient = createWalletClient({
                                                                                                        account,
                                                                                                        chain,
                                                                                                        transport: http(rpcUrl),
                                                                                                        });
                                                                                                        approverAddr = account.address;
                                                                                                        console.log(
                                                                                                        "[Approve API] Using testWalletPrivateKey from deployment for approvals",
                                                                                                        { address: approverAddr },
                                                                                                        );
                                                                                                        } else {
                                                                                                            approverAddr = json.accounts.approver;
                                                                                                            if (!approverAddr) throw new Error("approver address not found");

                                                                                                            // Impersonate approver on Anvil
                                                                                                            fetch("http://127.0.0.1:8545", {
                                                                                                                method: "POST",
                                                                                                                headers: { "Content-Type": "application/json" },
                                                                                                                body: JSON.stringify({
                                                                                                                    jsonrpc: "2.0",
                                                                                                                    method: "anvil_impersonateAccount",
                                                                                                                    params: [approverAddr],
                                                                                                                    id: 1,
                                                                                                                    }),
                                                                                                                    });
                                                                                                                    account = approverAddr;
                                                                                                                    walletClient = createWalletClient({ chain, transport: http(rpcUrl) });
                                                                                                                    console.log("[Approve API] Impersonating approver account on Anvil", {
                                                                                                                        address: approverAddr,
                                                                                                                        });
                                                                                                                    }
                                                                                                                }

                                                                                                                // Ensure single approver mode (dev convenience)
                                                                                                                const auto currentRequired = safeReadContract<bigint>(publicClient, {;
                                                                                                                    address: OTC_ADDRESS,
                                                                                                                    abi,
                                                                                                                    functionName: "requiredApprovals",
                                                                                                                    args: [],
                                                                                                                    });

                                                                                                                    console.log(
                                                                                                                    "[Approve API] Current required approvals:",
                                                                                                                    Number(currentRequired),
                                                                                                                    );

                                                                                                                    if (Number(currentRequired) != 1) {
                                                                                                                        std::cout << "[Approve API] Setting requiredApprovals to 1..." << std::endl;
                                                                                                                        const auto deploymentInfoPath = path.join(;
                                                                                                                        process.cwd(),
                                                                                                                        "contracts/deployments/eliza-otc-deployment.json",
                                                                                                                        );
                                                                                                                        const auto raw = fs.readFile(deploymentInfoPath, "utf8");
                                                                                                                        const auto json = /* JSON.parse */ raw;
                                                                                                                        const auto ownerAddr = json.accounts.owner;

                                                                                                                        fetch("http://127.0.0.1:8545", {
                                                                                                                            method: "POST",
                                                                                                                            headers: { "Content-Type": "application/json" },
                                                                                                                            body: JSON.stringify({
                                                                                                                                jsonrpc: "2.0",
                                                                                                                                method: "anvil_impersonateAccount",
                                                                                                                                params: [ownerAddr],
                                                                                                                                id: 1,
                                                                                                                                }),
                                                                                                                                });

                                                                                                                                const auto { request: setReq } = publicClient.simulateContract({;
                                                                                                                                    address: OTC_ADDRESS,
                                                                                                                                    abi,
                                                                                                                                    functionName: "setRequiredApprovals",
                                                                                                                                    args: [1n],
                                                                                                                                    account: ownerAddr,
                                                                                                                                    });
                                                                                                                                    createWalletClient({ chain, transport: http(rpcUrl) }).writeContract({
                                                                                                                                        ...setReq,
                                                                                                                                        account: ownerAddr,
                                                                                                                                        });
                                                                                                                                        std::cout << "[Approve API] ✅ Set requiredApprovals to 1" << std::endl;
                                                                                                                                        } else {
                                                                                                                                            std::cout << "[Approve API] ✅ Already in single-approver mode" << std::endl;
                                                                                                                                        }

                                                                                                                                        // Poll for offer to exist (tx might still be pending)
                                                                                                                                        // This handles the case where frontend calls us immediately after tx submission
                                                                                                                                        auto offer;
                                                                                                                                        const auto maxPollAttempts = 10; // 10 attempts * 2 seconds = 20 seconds max wait;

                                                                                                                                        for (int attempt = 1; attempt <= maxPollAttempts; attempt++) {
                                                                                                                                            const auto offerRaw = safeReadContract<RawOfferData>(publicClient, {;
                                                                                                                                                address: OTC_ADDRESS,
                                                                                                                                                abi,
                                                                                                                                                functionName: "offers",
                                                                                                                                                args: [BigInt(offerId)],
                                                                                                                                                });

                                                                                                                                                offer = parseOfferStruct(offerRaw);

                                                                                                                                                // Check if offer exists (beneficiary is set when offer is created)
                                                                                                                                                if (
                                                                                                                                                offer.beneficiary &&;
                                                                                                                                                offer.beneficiary != "0x0000000000000000000000000000000000000000";
                                                                                                                                                ) {
                                                                                                                                                    std::cout << "[Approve API] Offer found on attempt " + attempt << std::endl;
                                                                                                                                                    break;
                                                                                                                                                }

                                                                                                                                                if (attempt < maxPollAttempts) {
                                                                                                                                                    console.log(
                                                                                                                                                    "[Approve API] Offer " + offerId + " not found yet, waiting... (" + attempt + "/" + maxPollAttempts + ")"
                                                                                                                                                    );
                                                                                                                                                    new Promise((resolve) => setTimeout(resolve, 2000));
                                                                                                                                                }
                                                                                                                                            }

                                                                                                                                            if (
                                                                                                                                            !offer.beneficiary ||;
                                                                                                                                            offer.beneficiary == "0x0000000000000000000000000000000000000000";
                                                                                                                                            ) {
                                                                                                                                                return NextResponse.json(;
                                                                                                                                                {
                                                                                                                                                    "error: " + "Offer " + offerId + " not found after " + maxPollAttempts + " attempts. Transaction may still be pending."
                                                                                                                                                    },
                                                                                                                                                    { status: 404 },
                                                                                                                                                    );
                                                                                                                                                }

                                                                                                                                                console.log("[Approve API] Offer state:", {
                                                                                                                                                    approved: offer.approved,
                                                                                                                                                    cancelled: offer.cancelled,
                                                                                                                                                    beneficiary: offer.beneficiary,
                                                                                                                                                    });

                                                                                                                                                    if (offer.approved) {
                                                                                                                                                        std::cout << "[Approve API] Offer already approved" << std::endl;
                                                                                                                                                        return NextResponse.json({;
                                                                                                                                                            success: true,
                                                                                                                                                            txHash: "already-approved",
                                                                                                                                                            alreadyApproved: true,
                                                                                                                                                            });
                                                                                                                                                        }

                                                                                                                                                        // ============ PRICE VALIDATION ============
                                                                                                                                                        // Validate that the offer price hasn't diverged too much from market price
                                                                                                                                                        // This prevents abuse from stale quotes or manipulated pool prices
                                                                                                                                                        const auto MAX_PRICE_DIVERGENCE_BPS = 1000; // 10% maximum divergence;

                                                                                                                                                        // Skip price validation for local development (Anvil has mock prices)
                                                                                                                                                        const auto isLocalNetwork =;
                                                                                                                                                        process.env.NEXT_PUBLIC_NETWORK == "localhost" ||;
                                                                                                                                                        process.env.NEXT_PUBLIC_NETWORK == "anvil" ||;
                                                                                                                                                        process.env.NETWORK == "localhost" ||;
                                                                                                                                                        process.env.NETWORK == "anvil";

                                                                                                                                                        if (isLocalNetwork) {
                                                                                                                                                            std::cout << "[Approve API] Skipping price validation on local network" << std::endl;
                                                                                                                                                        }

                                                                                                                                                        try {
                                                                                                                                                            const auto { checkPriceDivergence } = import("@/utils/price-validator");
                                                                                                                                                            const auto { TokenDB, QuoteDB } = import("@/services/database");

                                                                                                                                                            // Get token info from the offer
                                                                                                                                                            // offer.priceUsdPerToken is in 8 decimals (Chainlink format)
                                                                                                                                                            const auto offerPriceUsd = Number(offer.priceUsdPerToken) / 1e8;

                                                                                                                                                            // Find the specific token associated with this offer
                                                                                                                                                            // Primary method: Use the on-chain tokenId (keccak256 hash of symbol) to look up token
                                                                                                                                                            std::string tokenAddress = nullptr;
                                                                                                                                                            std::string tokenChain = "base";

                                                                                                                                                            // The offer.tokenId is a bytes32 (keccak256 of token symbol)
                                                                                                                                                            if (offer.tokenId) {
                                                                                                                                                                const auto token = TokenDB.getTokenByOnChainId(offer.tokenId);
                                                                                                                                                                if (token) {
                                                                                                                                                                    tokenAddress = token.contractAddress;
                                                                                                                                                                    tokenChain = token.chain as "base" | "solana";
                                                                                                                                                                    console.log("[Approve API] Found token via on-chain tokenId:", {
                                                                                                                                                                        symbol: token.symbol,
                                                                                                                                                                        address: tokenAddress,
                                                                                                                                                                        chain: tokenChain,
                                                                                                                                                                        });
                                                                                                                                                                    }
                                                                                                                                                                }

                                                                                                                                                                // Fallback: Try to find via quote (if we have a matching quote by beneficiary)
                                                                                                                                                                if (!tokenAddress) {
                                                                                                                                                                    const auto activeQuotes = QuoteDB.getActiveQuotes();
                                                                                                                                                                    const auto matchingQuote = activeQuotes.find(;
                                                                                                                                                                    (q: { beneficiary: string }) =>
                                                                                                                                                                    q.beneficiary.toLowerCase() == offer.beneficiary.toLowerCase(),
                                                                                                                                                                    );

                                                                                                                                                                    if (matchingQuote && "tokenId" in matchingQuote) {
                                                                                                                                                                        const auto token = TokenDB.getToken(matchingQuote.tokenId);
                                                                                                                                                                        if (token) {
                                                                                                                                                                            tokenAddress = token.contractAddress;
                                                                                                                                                                            tokenChain = token.chain as "base" | "solana";
                                                                                                                                                                            console.log("[Approve API] Found token via quote:", {
                                                                                                                                                                                symbol: token.symbol,
                                                                                                                                                                                address: tokenAddress,
                                                                                                                                                                                chain: tokenChain,
                                                                                                                                                                                });
                                                                                                                                                                            }
                                                                                                                                                                        }
                                                                                                                                                                    }

                                                                                                                                                                    // Last resort fallback: Use first active Base token (for backwards compatibility)
                                                                                                                                                                    if (!tokenAddress) {
                                                                                                                                                                        console.warn(
                                                                                                                                                                        "[Approve API] Could not find token via on-chain tokenId or quote, using fallback",
                                                                                                                                                                        );
                                                                                                                                                                        const auto tokens = TokenDB.getAllTokens({ isActive: true });
                                                                                                                                                                        const auto baseToken = tokens.find((t) => t.chain == "base");
                                                                                                                                                                        if (baseToken) {
                                                                                                                                                                            tokenAddress = baseToken.contractAddress;
                                                                                                                                                                            tokenChain = "base";
                                                                                                                                                                        }
                                                                                                                                                                    }

                                                                                                                                                                    if (tokenAddress && offerPriceUsd > 0) {
                                                                                                                                                                        console.log("[Approve API] Validating price against market...", {
                                                                                                                                                                            offerPriceUsd,
                                                                                                                                                                            tokenAddress,
                                                                                                                                                                            tokenChain,
                                                                                                                                                                            });

                                                                                                                                                                            const auto priceCheck = checkPriceDivergence(;
                                                                                                                                                                            tokenAddress,
                                                                                                                                                                            tokenChain,
                                                                                                                                                                            offerPriceUsd,
                                                                                                                                                                            );

                                                                                                                                                                            if (!priceCheck.valid && priceCheck.divergencePercent != undefined) {
                                                                                                                                                                                console.log("[Approve API] Price divergence detected:", {
                                                                                                                                                                                    offerPrice: offerPriceUsd,
                                                                                                                                                                                    marketPrice: priceCheck.aggregatedPrice,
                                                                                                                                                                                    divergence: priceCheck.divergencePercent,
                                                                                                                                                                                    warning: priceCheck.warning,
                                                                                                                                                                                    });

                                                                                                                                                                                    // Reject if divergence exceeds threshold (skip on local network)
                                                                                                                                                                                    if (
                                                                                                                                                                                    priceCheck.divergencePercent > MAX_PRICE_DIVERGENCE_BPS / 100 &&;
                                                                                                                                                                                    !isLocalNetwork;
                                                                                                                                                                                    ) {
                                                                                                                                                                                        return NextResponse.json(;
                                                                                                                                                                                        {
                                                                                                                                                                                            success: false,
                                                                                                                                                                                            error: "Price divergence too high",
                                                                                                                                                                                            details: {
                                                                                                                                                                                                offerPrice: offerPriceUsd,
                                                                                                                                                                                                marketPrice: priceCheck.aggregatedPrice,
                                                                                                                                                                                                divergencePercent: priceCheck.divergencePercent,
                                                                                                                                                                                                maxAllowedPercent: MAX_PRICE_DIVERGENCE_BPS / 100,
                                                                                                                                                                                                reason: priceCheck.warning,
                                                                                                                                                                                                },
                                                                                                                                                                                                },
                                                                                                                                                                                                { status: 400 },
                                                                                                                                                                                                );
                                                                                                                                                                                                } else if (;
                                                                                                                                                                                                isLocalNetwork &&;
                                                                                                                                                                                                priceCheck.divergencePercent > MAX_PRICE_DIVERGENCE_BPS / 100;
                                                                                                                                                                                                ) {
                                                                                                                                                                                                    console.log(
                                                                                                                                                                                                    "[Approve API] Skipping price rejection on local network (divergence:",
                                                                                                                                                                                                    priceCheck.divergencePercent,
                                                                                                                                                                                                    "%)",
                                                                                                                                                                                                    );
                                                                                                                                                                                                }
                                                                                                                                                                                                } else {
                                                                                                                                                                                                    console.log("[Approve API] Price validation passed:", {
                                                                                                                                                                                                        divergence: priceCheck.divergencePercent,
                                                                                                                                                                                                        valid: priceCheck.valid,
                                                                                                                                                                                                        });
                                                                                                                                                                                                    }
                                                                                                                                                                                                }
                                                                                                                                                                                                } catch (priceError) {
                                                                                                                                                                                                    // Log but don't block - fail open for price validation errors
                                                                                                                                                                                                    console.warn(
                                                                                                                                                                                                    "[Approve API] Price validation failed (continuing):",
                                                                                                                                                                                                    priceError,
                                                                                                                                                                                                    );
                                                                                                                                                                                                }
                                                                                                                                                                                                // ============ END PRICE VALIDATION ============

                                                                                                                                                                                                // Approve immediately
                                                                                                                                                                                                const auto accountAddr = (;
                                                                                                                                                                                                typeof account == "string" ? account : account.address
                                                                                                                                                                                                );

                                                                                                                                                                                                console.log("[Approve API] Simulating approval...", {
                                                                                                                                                                                                    offerId,
                                                                                                                                                                                                    account: accountAddr,
                                                                                                                                                                                                    otcAddress: OTC_ADDRESS,
                                                                                                                                                                                                    });

                                                                                                                                                                                                    const auto { request: approveRequest } = publicClient.simulateContract({;
                                                                                                                                                                                                        address: OTC_ADDRESS,
                                                                                                                                                                                                        abi,
                                                                                                                                                                                                        functionName: "approveOffer",
                                                                                                                                                                                                        args: [BigInt(offerId)],
                                                                                                                                                                                                        account: accountAddr,
                                                                                                                                                                                                        });

                                                                                                                                                                                                        std::cout << "[Approve API] Sending approval tx..." << std::endl;
                                                                                                                                                                                                        "const auto txHash: " + "0x" + string;
                                                                                                                                                                                                        walletClient.writeContract(approveRequest);

                                                                                                                                                                                                        std::cout << "[Approve API] Waiting for confirmation..." << txHash << std::endl;
                                                                                                                                                                                                        const auto approvalReceipt = publicClient.waitForTransactionReceipt({;
                                                                                                                                                                                                            hash: txHash,
                                                                                                                                                                                                            });

                                                                                                                                                                                                            console.log("[Approve API] Approval receipt:", {
                                                                                                                                                                                                                status: approvalReceipt.status,
                                                                                                                                                                                                                blockNumber: approvalReceipt.blockNumber,
                                                                                                                                                                                                                gasUsed: approvalReceipt.std::to_string(gasUsed),
                                                                                                                                                                                                                });

                                                                                                                                                                                                                std::cout << "[Approve API] ✅ Offer approved:" << offerId << "tx:" << txHash << std::endl;

                                                                                                                                                                                                                // Update quote status and financial data if we can find it
                                                                                                                                                                                                                const auto runtime = agentRuntime.getRuntime();
                                                                                                                                                                                                                const auto quoteService = runtime.getService<QuoteService>("QuoteService");

                                                                                                                                                                                                                if (quoteService && offer.beneficiary) {
                                                                                                                                                                                                                    const auto activeQuotes = quoteService.getActiveQuotes();
                                                                                                                                                                                                                    const auto matchingQuote = activeQuotes.find(;
                                                                                                                                                                                                                    (q: QuoteMemory) =>
                                                                                                                                                                                                                    q.beneficiary.toLowerCase() == offer.beneficiary.toLowerCase(),
                                                                                                                                                                                                                    );

                                                                                                                                                                                                                    if (matchingQuote) {
                                                                                                                                                                                                                        // Calculate financial values from on-chain offer data
                                                                                                                                                                                                                        const auto tokenAmountWei = BigInt(offer.tokenAmount);
                                                                                                                                                                                                                        const auto priceUsd8 = BigInt(offer.priceUsdPerToken);
                                                                                                                                                                                                                        const auto discountBpsNum = Number(offer.discountBps);

                                                                                                                                                                                                                        // totalUsd = (tokenAmount * priceUsdPerToken) / 1e18 (result in 8 decimals)
                                                                                                                                                                                                                        const auto totalUsd8 = (tokenAmountWei * priceUsd8) / BigInt(1e18);
                                                                                                                                                                                                                        const auto totalUsd = Number(totalUsd8) / 1e8;

                                                                                                                                                                                                                        // discountUsd = totalUsd * discountBps / 10000
                                                                                                                                                                                                                        const auto discountUsd8 = (totalUsd8 * BigInt(discountBpsNum)) / 10000n;
                                                                                                                                                                                                                        const auto discountUsd = Number(discountUsd8) / 1e8;

                                                                                                                                                                                                                        // discountedUsd = totalUsd - discountUsd
                                                                                                                                                                                                                        const auto discountedUsd8 = totalUsd8 - discountUsd8;
                                                                                                                                                                                                                        const auto discountedUsd = Number(discountedUsd8) / 1e8;

                                                                                                                                                                                                                        // Determine payment currency and amount based on offer currency
                                                                                                                                                                                                                        const auto paymentCurrency: "ETH" | "USDC" =;
                                                                                                                                                                                                                        offer.currency == 0 ? "ETH" : "USDC";
                                                                                                                                                                                                                        auto paymentAmount = "0";

                                                                                                                                                                                                                        if (offer.currency == 0) {
                                                                                                                                                                                                                            // Calculate required ETH
                                                                                                                                                                                                                            const auto ethPrice = Number(offer.ethUsdPrice) / 1e8;
                                                                                                                                                                                                                            paymentAmount = (discountedUsd / ethPrice).toFixed(6);
                                                                                                                                                                                                                            } else {
                                                                                                                                                                                                                                // USDC
                                                                                                                                                                                                                                paymentAmount = discountedUsd.toFixed(2);
                                                                                                                                                                                                                            }

                                                                                                                                                                                                                            console.log("[Approve API] Calculated financial data:", {
                                                                                                                                                                                                                                tokenAmount: offer.std::to_string(tokenAmount),
                                                                                                                                                                                                                                totalUsd,
                                                                                                                                                                                                                                discountUsd,
                                                                                                                                                                                                                                discountedUsd,
                                                                                                                                                                                                                                paymentAmount,
                                                                                                                                                                                                                                paymentCurrency,
                                                                                                                                                                                                                                });

                                                                                                                                                                                                                                // Update quote status
                                                                                                                                                                                                                                quoteService.updateQuoteStatus(matchingQuote.quoteId, "approved", {
                                                                                                                                                                                                                                    offerId: std::to_string(offerId),
                                                                                                                                                                                                                                    transactionHash: txHash,
                                                                                                                                                                                                                                    blockNumber: Number(approvalReceipt.blockNumber),
                                                                                                                                                                                                                                    rejectionReason: "",
                                                                                                                                                                                                                                    approvalNote: "Approved via API",
                                                                                                                                                                                                                                    });

                                                                                                                                                                                                                                    // Update quote with financial data from contract
                                                                                                                                                                                                                                    const auto updatedQuote = quoteService.getQuoteByQuoteId(;
                                                                                                                                                                                                                                    matchingQuote.quoteId,
                                                                                                                                                                                                                                    );
                                                                                                                                                                                                                                    updatedQuote.tokenAmount = offer.std::to_string(tokenAmount);
                                                                                                                                                                                                                                    updatedQuote.totalUsd = totalUsd;
                                                                                                                                                                                                                                    updatedQuote.discountUsd = discountUsd;
                                                                                                                                                                                                                                    updatedQuote.discountedUsd = discountedUsd;
                                                                                                                                                                                                                                    updatedQuote.paymentAmount = paymentAmount;
                                                                                                                                                                                                                                    updatedQuote.paymentCurrency = paymentCurrency;
                                                                                                                                                                                                                                    updatedQuote.discountBps = discountBpsNum;

                                                                                                                                                                                                                                    "runtime.setCache(" + "quote:" + matchingQuote.quoteId

                                                                                                                                                                                                                                    console.log(
                                                                                                                                                                                                                                    "[Approve API] Updated quote with financial data:",
                                                                                                                                                                                                                                    matchingQuote.quoteId,
                                                                                                                                                                                                                                    );
                                                                                                                                                                                                                                }
                                                                                                                                                                                                                            }

                                                                                                                                                                                                                            // If still not approved (multi-approver deployments), escalate approvals
                                                                                                                                                                                                                            auto approvedOfferRaw = safeReadContract<RawOfferData>(publicClient, {;
                                                                                                                                                                                                                                address: OTC_ADDRESS,
                                                                                                                                                                                                                                abi,
                                                                                                                                                                                                                                functionName: "offers",
                                                                                                                                                                                                                                args: [BigInt(offerId)],
                                                                                                                                                                                                                                });

                                                                                                                                                                                                                                auto approvedOffer = parseOfferStruct(approvedOfferRaw);

                                                                                                                                                                                                                                if (!approvedOffer.approved) {
                                                                                                                                                                                                                                    // Load known approver and agent from deployment file
                                                                                                                                                                                                                                    const auto deploymentInfoPath = path.join(;
                                                                                                                                                                                                                                    process.cwd(),
                                                                                                                                                                                                                                    "contracts/deployments/eliza-otc-deployment.json",
                                                                                                                                                                                                                                    );
                                                                                                                                                                                                                                    const auto raw = fs.readFile(deploymentInfoPath, "utf8");
                                                                                                                                                                                                                                    const auto json = /* JSON.parse */ raw;
                                                                                                                                                                                                                                    const auto approver = json.accounts.approver;
                                                                                                                                                                                                                                    const auto agentAddr = json.accounts.agent;
                                                                                                                                                                                                                                    const auto candidates = [approver, agentAddr];

                                                                                                                                                                                                                                    for (const auto& addr : candidates)
                                                                                                                                                                                                                                        std::cout << "[Approve API] Attempting secondary approval by" << addr << std::endl;

                                                                                                                                                                                                                                        fetch("http://127.0.0.1:8545", {
                                                                                                                                                                                                                                            method: "POST",
                                                                                                                                                                                                                                            headers: { "Content-Type": "application/json" },
                                                                                                                                                                                                                                            body: JSON.stringify({
                                                                                                                                                                                                                                                jsonrpc: "2.0",
                                                                                                                                                                                                                                                method: "anvil_impersonateAccount",
                                                                                                                                                                                                                                                params: [addr],
                                                                                                                                                                                                                                                id: 1,
                                                                                                                                                                                                                                                }),
                                                                                                                                                                                                                                                });

                                                                                                                                                                                                                                                const auto { request: req2 } = publicClient.simulateContract({;
                                                                                                                                                                                                                                                    address: OTC_ADDRESS,
                                                                                                                                                                                                                                                    abi,
                                                                                                                                                                                                                                                    functionName: "approveOffer",
                                                                                                                                                                                                                                                    args: [BigInt(offerId)],
                                                                                                                                                                                                                                                    account: addr,
                                                                                                                                                                                                                                                    });
                                                                                                                                                                                                                                                    createWalletClient({
                                                                                                                                                                                                                                                        chain,
                                                                                                                                                                                                                                                        transport: http(rpcUrl),
                                                                                                                                                                                                                                                        }).writeContract({ ...req2, account: addr });

                                                                                                                                                                                                                                                        // Re-read state after each attempt
                                                                                                                                                                                                                                                        approvedOfferRaw = safeReadContract<RawOfferData>(publicClient, {
                                                                                                                                                                                                                                                            address: OTC_ADDRESS,
                                                                                                                                                                                                                                                            abi,
                                                                                                                                                                                                                                                            functionName: "offers",
                                                                                                                                                                                                                                                            args: [BigInt(offerId)],
                                                                                                                                                                                                                                                            });
                                                                                                                                                                                                                                                            approvedOffer = parseOfferStruct(approvedOfferRaw);
                                                                                                                                                                                                                                                            if (approvedOffer.approved) break;
                                                                                                                                                                                                                                                        }
                                                                                                                                                                                                                                                    }

                                                                                                                                                                                                                                                    // Final verification that offer was approved
                                                                                                                                                                                                                                                    std::cout << "[Approve API] Verifying final approval state..." << std::endl;

                                                                                                                                                                                                                                                    approvedOfferRaw = safeReadContract<RawOfferData>(publicClient, {
                                                                                                                                                                                                                                                        address: OTC_ADDRESS,
                                                                                                                                                                                                                                                        abi,
                                                                                                                                                                                                                                                        functionName: "offers",
                                                                                                                                                                                                                                                        args: [BigInt(offerId)],
                                                                                                                                                                                                                                                        });

                                                                                                                                                                                                                                                        approvedOffer = parseOfferStruct(approvedOfferRaw);

                                                                                                                                                                                                                                                        console.log("[Approve API] Final offer state:", {
                                                                                                                                                                                                                                                            offerId,
                                                                                                                                                                                                                                                            approved: approvedOffer.approved,
                                                                                                                                                                                                                                                            cancelled: approvedOffer.cancelled,
                                                                                                                                                                                                                                                            paid: approvedOffer.paid,
                                                                                                                                                                                                                                                            fulfilled: approvedOffer.fulfilled,
                                                                                                                                                                                                                                                            });

                                                                                                                                                                                                                                                            if (approvedOffer.cancelled) {
                                                                                                                                                                                                                                                                return NextResponse.json({ error: "Offer is cancelled" }, { status: 400 });
                                                                                                                                                                                                                                                            }

                                                                                                                                                                                                                                                            if (!approvedOffer.approved) {
                                                                                                                                                                                                                                                                throw std::runtime_error("Offer still not approved after all attempts");
                                                                                                                                                                                                                                                            }

                                                                                                                                                                                                                                                            // Check if approver should also fulfill
                                                                                                                                                                                                                                                            const auto requireApproverToFulfill = safeReadContract<boolean>(;
                                                                                                                                                                                                                                                            publicClient,
                                                                                                                                                                                                                                                            {
                                                                                                                                                                                                                                                                address: OTC_ADDRESS,
                                                                                                                                                                                                                                                                abi,
                                                                                                                                                                                                                                                                functionName: "requireApproverToFulfill",
                                                                                                                                                                                                                                                                args: [],
                                                                                                                                                                                                                                                                },
                                                                                                                                                                                                                                                                );

                                                                                                                                                                                                                                                                console.log(
                                                                                                                                                                                                                                                                "[Approve API] requireApproverToFulfill:",
                                                                                                                                                                                                                                                                requireApproverToFulfill,
                                                                                                                                                                                                                                                                );

                                                                                                                                                                                                                                                                "auto fulfillTxHash: " + "0x" + string;

                                                                                                                                                                                                                                                                // If approver-only fulfill is enabled, backend pays immediately after approval
                                                                                                                                                                                                                                                                if (requireApproverToFulfill && !approvedOffer.paid) {
                                                                                                                                                                                                                                                                    std::cout << "[Approve API] Auto-fulfilling offer (approver-only mode)..." << std::endl;

                                                                                                                                                                                                                                                                    try {
                                                                                                                                                                                                                                                                        const auto accountAddr = (;
                                                                                                                                                                                                                                                                        typeof account == "string" ? account : account.address
                                                                                                                                                                                                                                                                        );

                                                                                                                                                                                                                                                                        // Calculate required payment
                                                                                                                                                                                                                                                                        const auto currency = approvedOffer.currency;
                                                                                                                                                                                                                                                                        auto valueWei: bigint | std::nullopt;

                                                                                                                                                                                                                                                                        if (currency == 0) {
                                                                                                                                                                                                                                                                            // ETH payment required
                                                                                                                                                                                                                                                                            const auto requiredEth = safeReadContract<bigint>(publicClient, {;
                                                                                                                                                                                                                                                                                address: OTC_ADDRESS,
                                                                                                                                                                                                                                                                                abi,
                                                                                                                                                                                                                                                                                functionName: "requiredEthWei",
                                                                                                                                                                                                                                                                                args: [BigInt(offerId)],
                                                                                                                                                                                                                                                                                });

                                                                                                                                                                                                                                                                                valueWei = requiredEth;
                                                                                                                                                                                                                                                                                std::cout << "[Approve API] Required ETH:" << std::to_string(requiredEth) << std::endl;
                                                                                                                                                                                                                                                                                } else {
                                                                                                                                                                                                                                                                                    // USDC payment - need to approve first
                                                                                                                                                                                                                                                                                    const auto usdcAddress = safeReadContract<Address>(publicClient, {;
                                                                                                                                                                                                                                                                                        address: OTC_ADDRESS,
                                                                                                                                                                                                                                                                                        abi,
                                                                                                                                                                                                                                                                                        functionName: "usdc",
                                                                                                                                                                                                                                                                                        args: [],
                                                                                                                                                                                                                                                                                        });

                                                                                                                                                                                                                                                                                        const auto requiredUsdc = safeReadContract<bigint>(publicClient, {;
                                                                                                                                                                                                                                                                                            address: OTC_ADDRESS,
                                                                                                                                                                                                                                                                                            abi,
                                                                                                                                                                                                                                                                                            functionName: "requiredUsdcAmount",
                                                                                                                                                                                                                                                                                            args: [BigInt(offerId)],
                                                                                                                                                                                                                                                                                            });

                                                                                                                                                                                                                                                                                            std::cout << "[Approve API] Required USDC:" << std::to_string(requiredUsdc) << std::endl;

                                                                                                                                                                                                                                                                                            // Approve USDC
                                                                                                                                                                                                                                                                                            const auto erc20Abi = [;
                                                                                                                                                                                                                                                                                            {
                                                                                                                                                                                                                                                                                                type: "function",
                                                                                                                                                                                                                                                                                                name: "approve",
                                                                                                                                                                                                                                                                                                stateMutability: "nonpayable",
                                                                                                                                                                                                                                                                                                inputs: [
                                                                                                                                                                                                                                                                                                { name: "spender", type: "address" },
                                                                                                                                                                                                                                                                                                { name: "amount", type: "uint256" },
                                                                                                                                                                                                                                                                                                ],
                                                                                                                                                                                                                                                                                                outputs: [{ name: "", type: "bool" }],
                                                                                                                                                                                                                                                                                                },
                                                                                                                                                                                                                                                                                                ];

                                                                                                                                                                                                                                                                                                const auto { request: approveUsdcReq } = publicClient.simulateContract(;
                                                                                                                                                                                                                                                                                                {
                                                                                                                                                                                                                                                                                                    address: usdcAddress,
                                                                                                                                                                                                                                                                                                    abi: erc20Abi,
                                                                                                                                                                                                                                                                                                    functionName: "approve",
                                                                                                                                                                                                                                                                                                    args: [OTC_ADDRESS, requiredUsdc],
                                                                                                                                                                                                                                                                                                    account: accountAddr,
                                                                                                                                                                                                                                                                                                    },
                                                                                                                                                                                                                                                                                                    );

                                                                                                                                                                                                                                                                                                    walletClient.writeContract(approveUsdcReq);
                                                                                                                                                                                                                                                                                                    std::cout << "[Approve API] USDC approved" << std::endl;
                                                                                                                                                                                                                                                                                                }

                                                                                                                                                                                                                                                                                                // Fulfill offer
                                                                                                                                                                                                                                                                                                const auto { request: fulfillReq } = publicClient.simulateContract({;
                                                                                                                                                                                                                                                                                                    address: OTC_ADDRESS,
                                                                                                                                                                                                                                                                                                    abi,
                                                                                                                                                                                                                                                                                                    functionName: "fulfillOffer",
                                                                                                                                                                                                                                                                                                    args: [BigInt(offerId)],
                                                                                                                                                                                                                                                                                                    account: accountAddr,
                                                                                                                                                                                                                                                                                                    value: valueWei,
                                                                                                                                                                                                                                                                                                    });

                                                                                                                                                                                                                                                                                                    fulfillTxHash = walletClient.writeContract(fulfillReq);
                                                                                                                                                                                                                                                                                                    std::cout << "[Approve API] Fulfill tx sent:" << fulfillTxHash << std::endl;

                                                                                                                                                                                                                                                                                                    if (fulfillTxHash) {
                                                                                                                                                                                                                                                                                                        publicClient.waitForTransactionReceipt({ hash: fulfillTxHash });
                                                                                                                                                                                                                                                                                                    }
                                                                                                                                                                                                                                                                                                    std::cout << "[Approve API] ✅ Offer fulfilled automatically" << std::endl;
                                                                                                                                                                                                                                                                                                    } catch (fulfillError) {
                                                                                                                                                                                                                                                                                                        std::cerr << "[Approve API] ❌ Auto-fulfill failed:" << fulfillError << std::endl;

                                                                                                                                                                                                                                                                                                        // Check if offer got paid by another transaction during our attempt
                                                                                                                                                                                                                                                                                                        const auto recheckOffer = safeReadContract<RawOfferData>(publicClient, {;
                                                                                                                                                                                                                                                                                                            address: OTC_ADDRESS,
                                                                                                                                                                                                                                                                                                            abi,
                                                                                                                                                                                                                                                                                                            functionName: "offers",
                                                                                                                                                                                                                                                                                                            args: [BigInt(offerId)],
                                                                                                                                                                                                                                                                                                            });

                                                                                                                                                                                                                                                                                                            const auto recheckParsed = parseOfferStruct(recheckOffer);

                                                                                                                                                                                                                                                                                                            if (recheckParsed.paid) {
                                                                                                                                                                                                                                                                                                                console.log(
                                                                                                                                                                                                                                                                                                                "[Approve API] ✅ Offer was paid by another transaction, continuing...",
                                                                                                                                                                                                                                                                                                                );
                                                                                                                                                                                                                                                                                                                fulfillTxHash = std::nullopt; // No fulfillTx from us, but offer is paid;
                                                                                                                                                                                                                                                                                                                } else {
                                                                                                                                                                                                                                                                                                                    // Offer is approved but not paid - this is a real error
                                                                                                                                                                                                                                                                                                                    throw new Error(
                                                                                                                                                                                                                                                                                                                    "Auto-fulfill failed: " + std::to_string(true /* instanceof check */ ? fulfillError.message : std::to_string(fulfillError)) + ". Offer is approved but not paid."
                                                                                                                                                                                                                                                                                                                    );
                                                                                                                                                                                                                                                                                                                }
                                                                                                                                                                                                                                                                                                            }
                                                                                                                                                                                                                                                                                                            } else if (approvedOffer.paid) {
                                                                                                                                                                                                                                                                                                                std::cout << "[Approve API] ✅ Offer already paid << skipping auto-fulfill" << std::endl;
                                                                                                                                                                                                                                                                                                                } else if (!requireApproverToFulfill) {
                                                                                                                                                                                                                                                                                                                    console.log(
                                                                                                                                                                                                                                                                                                                    "[Approve API] ⚠️  requireApproverToFulfill is disabled. User must fulfill (pay) manually.",
                                                                                                                                                                                                                                                                                                                    );
                                                                                                                                                                                                                                                                                                                }

                                                                                                                                                                                                                                                                                                                // Return success
                                                                                                                                                                                                                                                                                                                return NextResponse.json({;
                                                                                                                                                                                                                                                                                                                    success: true,
                                                                                                                                                                                                                                                                                                                    approved: true,
                                                                                                                                                                                                                                                                                                                    approvalTx: txHash,
                                                                                                                                                                                                                                                                                                                    fulfillTx: fulfillTxHash,
                                                                                                                                                                                                                                                                                                                    offerId: std::to_string(offerId),
                                                                                                                                                                                                                                                                                                                    autoFulfilled: Boolean(fulfillTxHash),
                                                                                                                                                                                                                                                                                                                    message: fulfillTxHash
                                                                                                                                                                                                                                                                                                                    ? "Offer approved and fulfilled automatically";
                                                                                                                                                                                                                                                                                                                    : "Offer approved. Please complete payment to fulfill the offer.",
                                                                                                                                                                                                                                                                                                                    });

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
