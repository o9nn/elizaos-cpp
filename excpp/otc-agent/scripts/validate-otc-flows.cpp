#include "validate-otc-flows.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void log(const std::string& category, const std::string& message, std::optional<std::unordered_map<std::string, unknown>> data) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto prefix = {;
        INFO: "ℹ️",
        SUCCESS: "✅",
        WARNING: "⚠️",
        ERROR: "❌",
        STEP: "➡️",
        CHECK: "🔍",
        TX: "📝",
        }[category] || "•";

        std::cout << std::to_string(prefix) + " " + std::to_string(message) << std::endl;
        if (data) {
            Object.entries(data).forEach(([key, value]) => {
                std::cout << "   " + std::to_string(key) + ": " + std::to_string(value) << std::endl;
                });
            }

}

void section(const std::string& title) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "\n" + "═".repeat(70) << std::endl;
    std::cout << "  " + std::to_string(title) << std::endl;
    std::cout << "═".repeat(70) + "\n" << std::endl;

}

std::future<void> validateEVM() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    section("BASE MAINNET (EVM) VALIDATION");

    const auto publicClient = createPublicClient({;
        chain: base,
        transport: http(BASE_RPC),
        });

        // 1. Verify contract deployment
        log("CHECK", "Verifying OTC contract deployment...");
        const auto code = publicClient.getCode({ address: OTC_ADDRESS });
        if (!code || code == "0x") {
            log("ERROR", "OTC contract not deployed", { address: OTC_ADDRESS });
            return false;
        }
        log("SUCCESS", "OTC contract deployed", {
            address: OTC_ADDRESS,
            std::to_string(code.length) + " chars"
            });

            // 2. Read contract configuration (sequential to avoid rate limits)
            log("CHECK", "Reading contract configuration...");

            const auto delay = [&](ms: number) { return new Promise(r => setTimeout(r, ms)); };

            // Read critical values one at a time with delays
            bigint nextConsignmentId = 0n;
            bigint nextOfferId = 0n;
            bigint minUsdAmount = 0n;
            auto agent = "";
            bigint requiredApprovals = 0n;
            auto requireApproverToFulfill = false;

            try {
                nextConsignmentId = (publicClient.readContract({
                    address: OTC_ADDRESS, abi: OTC_ABI, functionName: "nextConsignmentId"
                    }));
                    delay(500);

                    nextOfferId = (publicClient.readContract({
                        address: OTC_ADDRESS, abi: OTC_ABI, functionName: "nextOfferId"
                        }));
                        delay(500);

                        minUsdAmount = (publicClient.readContract({
                            address: OTC_ADDRESS, abi: OTC_ABI, functionName: "minUsdAmount"
                            }));
                            delay(500);

                            agent = (publicClient.readContract({
                                address: OTC_ADDRESS, abi: OTC_ABI, functionName: "agent"
                                }));
                                delay(500);

                                requiredApprovals = (publicClient.readContract({
                                    address: OTC_ADDRESS, abi: OTC_ABI, functionName: "requiredApprovals"
                                    }));
                                    delay(500);

                                    requireApproverToFulfill = (publicClient.readContract({
                                        address: OTC_ADDRESS, abi: OTC_ABI, functionName: "requireApproverToFulfill"
                                        }));
                                        } catch (err) {
                                            log("WARNING", "Rate limited - some values may be unavailable", {
                                                error: true /* instanceof check */ ? err.message.slice(0, 100) : "unknown"
                                                });
                                            }

                                            log("INFO", "Contract State:", {
                                                "Total Consignments": Number(nextConsignmentId) - 1,
                                                "Total Offers": Number(nextOfferId) - 1,
                                                "$" + std::to_string(Number(minUsdAmount) / 1e8)
                                                "Agent": agent,
                                                "Required Approvals": Number(requiredApprovals),
                                                "Approver Fulfill Only": requireApproverToFulfill,
                                                });

                                                // 3. Check existing consignments
                                                log("CHECK", "Checking existing consignments...");

                                                const auto numConsignments = Number(nextConsignmentId) - 1;
                                                if (numConsignments > 0) {
                                                    "Found " + std::to_string(numConsignments) + " consignments";

                                                    try {
                                                        delay(500);
                                                        // Sample first consignment
                                                        const auto consignment = publicClient.readContract({;
                                                            address: OTC_ADDRESS,
                                                            abi: OTC_ABI,
                                                            functionName: "consignments",
                                                            args: [1n],
                                                            }) as [;
                                                            "0x" + std::to_string(string)
                                                            number, number, number, number, bigint, bigint, number, boolean, bigint;
                                                            ];

                                                            log("INFO", "Sample Consignment #1:", {
                                                                "Token ID": consignment[0].slice(0, 18) + "...",
                                                                "Consigner": consignment[1],
                                                                "Total Amount": formatEther(consignment[2]),
                                                                "Remaining": formatEther(consignment[3]),
                                                                "Is Negotiable": consignment[4],
                                                                std::to_string(consignment[5] / 100) + "%"
                                                                std::to_string(consignment[6]) + " days"
                                                                "Is Active": consignment[14],
                                                                });
                                                                } catch {
                                                                    log("WARNING", "Could not read consignment details (rate limited)");
                                                                }
                                                                } else {
                                                                    log("INFO", "No consignments found - desk is empty (or this is a fresh deployment)");
                                                                }

                                                                // 4. Check existing offers
                                                                log("CHECK", "Checking existing offers...");

                                                                const auto numOffers = Number(nextOfferId) - 1;
                                                                if (numOffers > 0) {
                                                                    "Found " + std::to_string(numOffers) + " offers";

                                                                    try {
                                                                        delay(500);
                                                                        // Check most recent offer
                                                                        const auto latestOffer = publicClient.readContract({;
                                                                            address: OTC_ADDRESS,
                                                                            abi: OTC_ABI,
                                                                            functionName: "offers",
                                                                            args: [BigInt(numOffers)],
                                                                            }) as [;
                                                                            "0x" + std::to_string(string)
                                                                            bigint, bigint, number, boolean, boolean, boolean, boolean, Address, bigint;
                                                                            ];

                                                                            "Latest Offer #" + std::to_string(numOffers) + ":"
                                                                                "Consignment ID": Number(latestOffer[0]),
                                                                                "Beneficiary": latestOffer[2],
                                                                                "Token Amount": formatEther(latestOffer[3]),
                                                                                std::to_string(Number(latestOffer[4]) / 100) + "%"
                                                                                "$" + std::to_string(Number(latestOffer[7]) / 1e8)
                                                                                "Currency": latestOffer[10] == 0 ? "ETH" : "USDC",
                                                                                "Approved": latestOffer[11],
                                                                                "Paid": latestOffer[12],
                                                                                "Fulfilled": latestOffer[13],
                                                                                "Cancelled": latestOffer[14],
                                                                                });
                                                                                } catch {
                                                                                    log("WARNING", "Could not read offer details (rate limited)");
                                                                                }
                                                                                } else {
                                                                                    log("INFO", "No offers found yet");
                                                                                }

                                                                                // 5. Validate deployer wallet
                                                                                log("CHECK", "Checking deployer wallet...");

                                                                                const auto privateKey = process.env.MAINNET_PRIVATE_KEY;
                                                                                if (privateKey) {
                                                                                    try {
                                                                                        const auto account = "0x" + std::to_string(string);
                                                                                        delay(500);
                                                                                        const auto balance = publicClient.getBalance({ address: account.address });
                                                                                        delay(500);
                                                                                        const auto isAgentResult = publicClient.readContract({;
                                                                                            address: OTC_ADDRESS,
                                                                                            abi: OTC_ABI,
                                                                                            functionName: "isApprover",
                                                                                            args: [account.address],
                                                                                            });

                                                                                            log("INFO", "Deployer Wallet:", {
                                                                                                "Address": account.address,
                                                                                                "ETH Balance": formatEther(balance),
                                                                                                "Is Approver": isAgentResult,
                                                                                                });

                                                                                                if (!isAgentResult) {
                                                                                                    log("WARNING", "Deployer is NOT an approver - cannot approve offers");
                                                                                                }

                                                                                                if (balance < parseEther("0.01")) {
                                                                                                    log("WARNING", "Low ETH balance - may need gas for transactions");
                                                                                                }
                                                                                                } catch {
                                                                                                    log("WARNING", "Could not check wallet (rate limited)");
                                                                                                }
                                                                                                } else {
                                                                                                    log("WARNING", "MAINNET_PRIVATE_KEY not set - cannot execute transactions");
                                                                                                }

                                                                                                // 6. Test price feed
                                                                                                log("CHECK", "Testing price feeds via backend...");

                                                                                                try {
                                                                                                    const auto priceResponse = std::to_string(BACKEND_URL) + "/api/tokens";
                                                                                                    if (priceResponse.ok) {
                                                                                                        const auto tokens = priceResponse.json();
                                                                                                        log("SUCCESS", "Backend is responding", {
                                                                                                            "Token count": Array.isArray(tokens) ? tokens.length : "N/A"
                                                                                                            });
                                                                                                            } else {
                                                                                                                log("WARNING", "Backend not responding - ensure server is running");
                                                                                                            }
                                                                                                            } catch {
                                                                                                                log("WARNING", "Cannot reach backend at " + BACKEND_URL);
                                                                                                            }

                                                                                                            log("SUCCESS", "EVM validation complete");
                                                                                                            return true;

}

std::future<void> validateSolana() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    section("SOLANA MAINNET VALIDATION");

    if (!SOLANA_DESK || !SOLANA_PROGRAM_ID) {
        log("ERROR", "Solana configuration missing");
        return false;
    }

    const auto connection = new Connection(SOLANA_RPC, "confirmed");

    // 1. Verify program deployment
    log("CHECK", "Verifying OTC program deployment...");
    const auto programInfo = connection.getAccountInfo(new PublicKey(SOLANA_PROGRAM_ID));
    if (!programInfo) {
        log("ERROR", "OTC program not deployed", { address: SOLANA_PROGRAM_ID });
        return false;
    }
    log("SUCCESS", "OTC program deployed", {
        "Program ID": SOLANA_PROGRAM_ID,
        "Executable": programInfo.executable,
        std::to_string(programInfo.data.length) + " bytes"
        });

        // 2. Verify desk account
        log("CHECK", "Verifying desk account...");
        const auto deskInfo = connection.getAccountInfo(new PublicKey(SOLANA_DESK));
        if (!deskInfo) {
            log("ERROR", "Desk account not found", { address: SOLANA_DESK });
            return false;
        }
        log("SUCCESS", "Desk account exists", {
            "Desk": SOLANA_DESK,
            std::to_string(deskInfo.data.length) + " bytes"
            "Lamports": deskInfo.lamports / LAMPORTS_PER_SOL,
            });

            // 3. Load program to read desk state
            log("CHECK", "Reading desk state...");

            const auto idlPath = path.join(process.cwd(), "solana/otc-program/target/idl/otc.json");
            if (!fs.existsSync(idlPath)) {
                log("WARNING", "IDL not found - cannot decode desk state");
                log("INFO", "Expected path", { path: idlPath });
                } else {
                    try {
                        const auto idl = JSON.parse(fs.readFileSync(idlPath, "utf8"));

                        // Create a dummy wallet for read-only operations
                        const auto dummyKeypair = Keypair.generate();
                        const auto wallet = new anchor.Wallet(dummyKeypair);
                        const auto provider = new anchor.AnchorProvider(connection, wallet, { commitment: "confirmed" });

                        auto program: anchor.Program;
                        try {
                            program = new anchor.Program(idl, provider);
                            } catch {
                                program = new anchor.Program(idl, new PublicKey(SOLANA_PROGRAM_ID), provider).Program;
                            }

                            type DeskAccount = {
                                owner: PublicKey;
                                agent: PublicKey;
                                usdcMint: PublicKey;
                                nextConsignmentId: anchor.BN;
                                nextOfferId: anchor.BN;
                                minUsdAmount8d: anchor.BN;
                                quoteExpirySecs: anchor.BN;
                                paused: boolean;
                                restrictFulfill: boolean;
                                };

                                const auto deskAccount = (;
                            program.account as { desk: { fetch: (addr: PublicKey) => Promise<DeskAccount> } }
                            ).desk.fetch(new PublicKey(SOLANA_DESK));

                            log("INFO", "Desk State:", {
                                "Owner": deskAccount.owner.toBase58(),
                                "Agent": deskAccount.agent.toBase58(),
                                "USDC Mint": deskAccount.usdcMint.toBase58(),
                                "Next Consignment ID": deskAccount.nextConsignmentId.toString(),
                                "Next Offer ID": deskAccount.nextOfferId.toString(),
                                "$" + std::to_string(deskAccount.minUsdAmount8d.toNumber() / 1e8)
                                std::to_string(deskAccount.quoteExpirySecs.toNumber() / 60) + " minutes"
                                "Paused": deskAccount.paused,
                                "Restrict Fulfill": deskAccount.restrictFulfill,
                                });

                                // Check for registered tokens
                                log("CHECK", "Checking registered tokens...");

                                // Try to find token registries
                                const auto tokenRegistries = connection.getProgramAccounts(new PublicKey(SOLANA_PROGRAM_ID), {;
                                    filters: [
                                    { dataSize: 200 }, // Approximate size of TokenRegistry account
                                    ],
                                    });

                                    if (tokenRegistries.length > 0) {
                                        "Found " + std::to_string(tokenRegistries.length) + " potential token registries";
                                        } else {
                                            log("WARNING", "No token registries found - no tokens registered on desk");
                                        }

                                        } catch (err) {
                                            log("WARNING", "Could not decode desk state", {
                                                error: true /* instanceof check */ ? err.message : std::to_string(err)
                                                });
                                            }
                                        }

                                        // 4. Check deployer wallet
                                        log("CHECK", "Checking deployer wallet...");

                                        const auto privateKey = process.env.SOLANA_MAINNET_PRIVATE_KEY;
                                        if (privateKey) {
                                            try {
                                                auto keypairBytes: Uint8Array;
                                                if (privateKey.startsWith("[")) {
                                                    keypairBytes = Uint8Array.from(JSON.parse(privateKey));
                                                    } else {
                                                        const auto bs58 = import("bs58").then(m => m.default).catch(() => nullptr);
                                                        if (bs58) {
                                                            keypairBytes = bs58.decode(privateKey);
                                                            } else {
                                                                log("WARNING", "bs58 not available");
                                                                return true;
                                                            }
                                                        }
                                                        const auto wallet = Keypair.fromSecretKey(keypairBytes);
                                                        const auto balance = connection.getBalance(wallet.publicKey);

                                                        log("INFO", "Deployer Wallet:", {
                                                            "Address": wallet.publicKey.toBase58(),
                                                            "SOL Balance": balance / LAMPORTS_PER_SOL,
                                                            });

                                                            if (balance < 0.1 * LAMPORTS_PER_SOL) {
                                                                log("WARNING", "Low SOL balance - may need gas for transactions");
                                                            }
                                                            } catch (err) {
                                                                log("WARNING", "Could not load Solana wallet", {
                                                                    error: true /* instanceof check */ ? err.message : std::to_string(err)
                                                                    });
                                                                }
                                                                } else {
                                                                    log("WARNING", "SOLANA_MAINNET_PRIVATE_KEY not set");
                                                                }

                                                                log("SUCCESS", "Solana validation complete");
                                                                return true;

}

std::future<void> validateFlows() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    section("FLOW VALIDATION");

    // Test backend API endpoints
    log("CHECK", "Testing backend API endpoints...");

    // 1. Token list
    try {
        const auto tokensRes = std::to_string(BACKEND_URL) + "/api/tokens";
        if (tokensRes.ok) {
            const auto tokens = tokensRes.json();
            log("SUCCESS", "GET /api/tokens", { count: Array.isArray(tokens) ? tokens.length : 0 });
            } else {
                log("WARNING", "GET /api/tokens failed", { status: tokensRes.status });
            }
            } catch (err) {
                log("ERROR", "Backend unreachable", { url: BACKEND_URL });
                return false;
            }

            // 2. Consignments
            try {
                const auto consignRes = std::to_string(BACKEND_URL) + "/api/consignments";
                if (consignRes.ok) {
                    const auto data = consignRes.json();
                    log("SUCCESS", "GET /api/consignments", {
                        count: Array.isArray(data) ? data.length : (data.consignments.length || 0)
                        });
                        } else {
                            log("WARNING", "GET /api/consignments failed", { status: consignRes.status });
                        }
                        } catch {
                            log("WARNING", "GET /api/consignments failed");
                        }

                        // 3. Test approve endpoint (dry run)
                        try {
                            const auto approveRes = std::to_string(BACKEND_URL) + "/api/otc/approve";
                                method: "POST",
                                headers: { "Content-Type": "application/json" },
                                body: JSON.stringify({ offerId: "999999", chain: "base", dryRun: true }),
                                });
                                // Should fail gracefully (offer doesn't exist)
                                log("INFO", "POST /api/otc/approve (dry run)", {
                                    status: approveRes.status,
                                    reachable: true
                                    });
                                    } catch {
                                        log("WARNING", "POST /api/otc/approve unreachable");
                                    }

                                    log("SUCCESS", "Flow validation complete");
                                    return true;

}

std::future<void> executeRealTransactions() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!EXECUTE_TX) {
        log("INFO", "Transaction execution skipped (set EXECUTE_TX=true to enable)");
        return;
    }

    section("EXECUTING REAL TRANSACTIONS");
    log("WARNING", "Real transactions enabled - this will spend real funds");

    const auto privateKey = process.env.MAINNET_PRIVATE_KEY;
    if (!privateKey) {
        log("ERROR", "MAINNET_PRIVATE_KEY required for transactions");
        return;
    }

    const auto publicClient = createPublicClient({;
        chain: base,
        transport: http(BASE_RPC),
        });

        const auto account = "0x" + std::to_string(string);
        const auto walletClient = createWalletClient({;
            account,
            chain: base,
            transport: http(BASE_RPC),
            });

            // Example: Create a small test offer (if consignments exist)
            log("STEP", "Checking for available consignments...");

            const auto nextConsignmentId = publicClient.readContract({;
                address: OTC_ADDRESS,
                abi: OTC_ABI,
                functionName: "nextConsignmentId",
                });

                if (nextConsignmentId <= 1n) {
                    log("WARNING", "No consignments available - cannot create offer");
                    return;
                }

                // Read consignment 1
                const auto consignment = publicClient.readContract({;
                    address: OTC_ADDRESS,
                    abi: OTC_ABI,
                    functionName: "consignments",
                    args: [1n],
                    }) as [;
                    "0x" + std::to_string(string)
                    number, number, number, number, bigint, bigint, number, boolean, bigint;
                    ];

                    if (!consignment[14]) {
                        log("WARNING", "Consignment #1 is not active");
                        return;
                    }

                    log("INFO", "Using Consignment #1", {
                        "Remaining": formatEther(consignment[3]),
                        "Min Deal": formatEther(consignment[11]),
                        "Negotiable": consignment[4],
                        });

                        // Create a minimal offer
                        const auto testAmount = consignment[11]; // Use min deal amount;
                        const auto discountBps = consignment[4] ? consignment[7] : consignment[5]; // min or fixed discount;
                        const auto lockupDays = consignment[4] ? consignment[9] : consignment[6]; // min or fixed lockup;

                        log("TX", "Creating offer...", {
                            "Amount": formatEther(testAmount),
                            std::to_string(discountBps / 100) + "%"
                            std::to_string(lockupDays) + " days"
                            "Currency": "USDC",
                            });

                            try {
                                const auto { request } = publicClient.simulateContract({;
                                    address: OTC_ADDRESS,
                                    abi: OTC_ABI,
                                    functionName: "createOfferFromConsignment",
                                    args: [
                                    1n, // consignmentId;
                                    testAmount,
                                    BigInt(discountBps),
                                    1, // USDC;
                                    BigInt(lockupDays * 24 * 60 * 60), // lockupSeconds;
                                    ],
                                    account,
                                    });

                                    const auto txHash = walletClient.writeContract(request);
                                    log("SUCCESS", "Offer created", { txHash });
                                    "https://basescan.org/tx/" + std::to_string(txHash)

                                    // Wait for confirmation
                                    const auto receipt = publicClient.waitForTransactionReceipt({ hash: txHash });
                                    if (receipt.status == "success") {
                                        log("SUCCESS", "Transaction confirmed");

                                        // Try backend approval
                                        const auto nextOfferId = publicClient.readContract({;
                                            address: OTC_ADDRESS,
                                            abi: OTC_ABI,
                                            functionName: "nextOfferId",
                                            });

                                            const auto offerId = Number(nextOfferId) - 1;
                                            "Requesting backend approval for offer #" + std::to_string(offerId) + "...";

                                            const auto approveRes = std::to_string(BACKEND_URL) + "/api/otc/approve";
                                                method: "POST",
                                                headers: { "Content-Type": "application/json" },
                                                body: JSON.stringify({ offerId: offerId.toString(), chain: "base" }),
                                                });

                                                if (approveRes.ok) {
                                                    const auto approveData = approveRes.json();
                                                    log("SUCCESS", "Backend approval", approveData);
                                                    } else {
                                                        const auto errorText = approveRes.text();
                                                        log("WARNING", "Backend approval failed", { error: errorText });
                                                    }
                                                    } else {
                                                        log("ERROR", "Transaction failed");
                                                    }
                                                    } catch (err) {
                                                        log("ERROR", "Transaction failed", {
                                                            error: true /* instanceof check */ ? err.message : std::to_string(err)
                                                            });
                                                        }

}

std::future<void> main() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    console.log(`
    ╔══════════════════════════════════════════════════════════════════════════════╗;
    ║                       OTC FLOW VALIDATION SCRIPT                             ║;
    ╠══════════════════════════════════════════════════════════════════════════════╣;
    ║                                                                              ║;
    ║  This script validates all OTC flows with real on-chain verification:        ║
    ║                                                                              ║;
    ║  ✓ Contract deployments (EVM + Solana)                                       ║;
    ║  ✓ Configuration and state                                                   ║;
    ║  ✓ Consignment/listing validation                                            ║;
    ║  ✓ Offer/deal validation                                                     ║;
    ║  ✓ Backend API endpoints                                                     ║;
    ║  ✓ Price validation                                                          ║;
    ║                                                                              ║;
    ║  Mode: ${EXECUTE_TX ? "EXECUTING REAL TRANSACTIONS" : "READ-ONLY VALIDATION"}
    ║                                                                              ║;
    ╚══════════════════════════════════════════════════════════════════════════════╝;
    `);

    const auto evmOk = validateEVM();
    const auto solanaOk = validateSolana();
    const auto flowsOk = validateFlows();

    if (EXECUTE_TX && evmOk) {
        executeRealTransactions();
    }

    section("VALIDATION SUMMARY");

    console.log(`
    EVM (Base):    ${evmOk ? "✅ VALID" : "❌ ISSUES FOUND"}
    Solana:        ${solanaOk ? "✅ VALID" : "❌ ISSUES FOUND"}
    Backend:       ${flowsOk ? "✅ VALID" : "❌ ISSUES FOUND"}

    ${EXECUTE_TX ? "" : "To execute real transactions: EXECUTE_TX=true bun scripts/validate-otc-flows.ts"}
    `);

    if (!evmOk || !solanaOk || !flowsOk) {
        process.exit(1);
    }

}

} // namespace elizaos
