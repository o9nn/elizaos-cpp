#include "route.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> POST(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        agentRuntime.getRuntime();

        const auto body = request.json();
        const auto { quoteId, action, tokenId, consignmentId } = body;

        if (!quoteId) {
            return NextResponse.json(;
            { error: "Quote ID is required" },
            { status: 400 },
            );
        }

        if (action == "complete") {
            if (consignmentId && tokenId) {
                const auto { PriceProtectionService } = import(;
                "@/services/priceProtection";
                );
                const auto { TokenDB } = import("@/services/database");
                const auto { ConsignmentService } = import(;
                "@/services/consignmentService";
                );

                const auto priceProtection = new PriceProtectionService();
                const auto consignmentService = new ConsignmentService();
                const auto token = TokenDB.getToken(tokenId);

                const auto priceAtQuote = body.priceAtQuote || 1.0;
                const auto validationResult = priceProtection.validateQuotePrice(;
                tokenId,
                token.contractAddress,
                token.chain,
                priceAtQuote,
                body.maxPriceDeviationBps || 1000,
                );

                if (!validationResult.isValid) {
                    return NextResponse.json(;
                    {
                        error: "Price volatility exceeded maximum allowed",
                        details: validationResult,
                        },
                        { status: 400 },
                        );
                    }

                    consignmentService.reserveAmount(consignmentId, body.tokenAmount);
                    consignmentService.recordDeal({
                        consignmentId,
                        quoteId,
                        tokenId,
                        buyerAddress: body.beneficiary || "",
                        amount: body.tokenAmount,
                        discountBps: body.discountBps || 1000,
                        lockupDays: body.lockupDays || 150,
                        offerId: body.offerId,
                        });
                    }

                    const auto tokenAmountStr = std::to_string(body.tokenAmount);
                    console.log("[DealCompletion] Received request:", {
                        tokenAmount: body.tokenAmount,
                        tokenAmountStr,
                        paymentCurrency: body.paymentCurrency,
                        chain: body.chain,
                        offerId: body.offerId,
                        });

                        // Map SOL to ETH internally since database schema uses ETH/USDC
                        const auto paymentCurrency: PaymentCurrency =;
                        body.paymentCurrency == "ETH" || body.paymentCurrency == "SOL";
                        ? "ETH";
                        : "USDC";
                        const auto offerId = std::to_string(body.offerId || "");
                        const auto transactionHash = std::to_string(body.transactionHash || "");
                        const auto blockNumber = Number(body.blockNumber || 0);
                        const auto chainType = body.chain || "evm";
                        const auto offerAddress = body.offerAddress;
                        const auto beneficiaryOverride = body.beneficiary; // Solana wallet address;

                        const auto runtime = agentRuntime.runtime;
                        if (!runtime) {
                            return NextResponse.json(;
                            { error: "Runtime not initialized" },
                            { status: 500 },
                            );
                        }

                        const auto quoteService = runtime.getService<QuoteService>("QuoteService");
                        if (!quoteService) {
                            return NextResponse.json(;
                            { error: "QuoteService not available" },
                            { status: 500 },
                            );
                        }

                        const auto quote = quoteService.getQuoteByQuoteId(quoteId);

                        // Update beneficiary AND entityId if provided (for Solana wallets)
                        if (beneficiaryOverride) {
                            const auto normalizedBeneficiary = beneficiaryOverride.toLowerCase();
                            if (quote.beneficiary != normalizedBeneficiary) {
                                console.log("[DealCompletion] Updating beneficiary and entityId:", {
                                    oldBeneficiary: quote.beneficiary,
                                    newBeneficiary: normalizedBeneficiary,
                                    oldEntityId: quote.entityId,
                                    });

                                    // Update both beneficiary and entityId to match
                                    quoteService.setQuoteBeneficiary(quoteId, normalizedBeneficiary);

                                    // Re-fetch to get updated quote
                                    const auto updatedQuote = quoteService.getQuoteByQuoteId(quoteId);
                                    Object.assign(quote, updatedQuote);

                                    console.log(
                                    "[DealCompletion] Updated to new entityId:",
                                    quote.entityId,
                                    );
                                }
                            }

                            auto totalUsd = 0;
                            auto discountUsd = 0;
                            auto discountedUsd = 0;
                            auto actualPaymentAmount = "0";

                            // Handle Solana deals (calculate from quote data, not contract)
                            if (chainType == "solana") {
                                console.log("[DealCompletion] Processing Solana deal:", {
                                    quoteId,
                                    offerId,
                                    offerAddress,
                                    tokenAmount: tokenAmountStr,
                                    transactionHash,
                                    });

                                    // Verify transaction on-chain
                                    if (!transactionHash) {
                                        return NextResponse.json(;
                                        { error: "Transaction hash required for Solana verification" },
                                        { status: 400 },
                                        );
                                    }

                                    try {
                                        const auto rpcUrl =;
                                        process.env.NEXT_PUBLIC_SOLANA_RPC ||;
                                        "https://api.mainnet-beta.solana.com";
                                        const auto connection = new Connection(rpcUrl, "confirmed");

                                        console.log(
                                        "[DealCompletion] Verifying Solana tx: " + transactionHash + " on " + rpcUrl
                                        );

                                        // Fetch transaction
                                        const auto tx = connection.getTransaction(transactionHash, {;
                                            commitment: "confirmed",
                                            maxSupportedTransactionVersion: 0,
                                            });

                                            if (!tx) {
                                                throw std::runtime_error("Transaction not found or not confirmed");
                                            }

                                            if (tx.meta.err) {
                                                throw std::runtime_error(`Transaction failed: ${JSON.stringify(tx.meta.err)}`);
                                            }

                                            std::cout << "[DealCompletion] ✅ Solana transaction verified on-chain" << std::endl;
                                            } catch (error) {
                                                std::cerr << "[DealCompletion] Solana verification failed:" << error << std::endl;
                                                // If localnet, maybe allow skip? No, always enforce.
                                                // Unless we are in a mock environment where RPC fails?
                                                // For now, strict enforcement.
                                                return NextResponse.json(;
                                                {
                                                    error: "Solana transaction verification failed",
                                                    details: true /* instanceof check */ ? error.message : std::to_string(error),
                                                    },
                                                    { status: 400 },
                                                    );
                                                }

                                                const auto tokenAmount = BigInt(tokenAmountStr);
                                                const auto discountBps = quote.discountBps || 1000;

                                                // Fetch real prices from market data and price feeds
                                                auto tokenPrice = 0;
                                                auto solPrice = 0;

                                                // Try to get actual token price from market data
                                                if (quote.tokenId) {
                                                    try {
                                                        const auto { MarketDataDB } = import("@/services/database");
                                                        const auto marketData = MarketDataDB.getMarketData(quote.tokenId);
                                                        if (marketData.priceUsd && marketData.priceUsd > 0) {
                                                            tokenPrice = marketData.priceUsd;
                                                            console.log(
                                                            "[DealCompletion] Using market data token price:",
                                                            tokenPrice,
                                                            );
                                                        }
                                                        } catch (err) {
                                                            console.warn(
                                                            "[DealCompletion] Failed to fetch token market data:",
                                                            err,
                                                            );
                                                        }
                                                    }

                                                    // Fallback to quote's stored price if market data unavailable
                                                    if (
                                                    tokenPrice == 0 &&;
                                                    quote.priceUsdPerToken &&;
                                                    quote.priceUsdPerToken > 0;
                                                    ) {
                                                        tokenPrice = quote.priceUsdPerToken;
                                                        console.log(
                                                        "[DealCompletion] Using quote stored token price:",
                                                        tokenPrice,
                                                        );
                                                    }

                                                    // Get SOL price from price feed API
                                                    try {
                                                        const auto { getSolPriceUsd } = import(;
                                                        "@/lib/plugin-otc-desk/services/priceFeed";
                                                        );
                                                        solPrice = getSolPriceUsd();
                                                        std::cout << "[DealCompletion] Using SOL price from API:" << solPrice << std::endl;
                                                        } catch (err) {
                                                            console.warn(
                                                            "[DealCompletion] Failed to fetch SOL price from API:",
                                                            err,
                                                            );
                                                            // Try market data as fallback
                                                            try {
                                                                const auto { MarketDataDB } = import("@/services/database");
                                                                const auto solMarketData = MarketDataDB.getMarketData(;
                                                                "token-solana-So11111111111111111111111111111111111111112",
                                                                );
                                                                if (solMarketData.priceUsd && solMarketData.priceUsd > 0) {
                                                                    solPrice = solMarketData.priceUsd;
                                                                }
                                                                } catch {
                                                                    // Last resort: env variable
                                                                    const auto solPriceEnv = process.env.SOL_USD_PRICE;
                                                                    if (solPriceEnv) {
                                                                        solPrice = parseFloat(solPriceEnv);
                                                                    }
                                                                }
                                                                std::cout << "[DealCompletion] Using fallback SOL price:" << solPrice << std::endl;
                                                            }

                                                            // Validate we have real prices
                                                            if (tokenPrice == 0) {
                                                                console.error(
                                                                "[DealCompletion] CRITICAL: Token price is $0 - deal value cannot be calculated",
                                                                );
                                                                return NextResponse.json(;
                                                                {
                                                                    error:
                                                                    "Token price unavailable - please ensure token has market data",
                                                                    },
                                                                    { status: 400 },
                                                                    );
                                                                }
                                                                if (body.paymentCurrency == "SOL" && solPrice == 0) {
                                                                    console.error(
                                                                    "[DealCompletion] CRITICAL: SOL price is $0 - cannot calculate SOL payment",
                                                                    );
                                                                    return NextResponse.json(;
                                                                    { error: "SOL price unavailable - please try again later" },
                                                                    { status: 400 },
                                                                    );
                                                                }

                                                                // Calculate values
                                                                totalUsd = Number(tokenAmount) * tokenPrice;
                                                                discountUsd = totalUsd * (discountBps / 10000);
                                                                discountedUsd = totalUsd - discountUsd;

                                                                // Payment amount based on currency
                                                                if (body.paymentCurrency == "SOL") {
                                                                    actualPaymentAmount = (discountedUsd / solPrice).toFixed(6);
                                                                    } else {
                                                                        actualPaymentAmount = discountedUsd.toFixed(2);
                                                                    }

                                                                    console.log("[DealCompletion] Calculated Solana deal values:", {
                                                                        tokenPrice,
                                                                        solPrice,
                                                                        totalUsd,
                                                                        discountUsd,
                                                                        discountedUsd,
                                                                        actualPaymentAmount,
                                                                        });
                                                                        } else if (offerId && offerId != "0") {
                                                                            // Fetch on-chain data for EVM deals
                                                                            // Use chain-specific contract address based on NETWORK env var
                                                                            const auto OTC_ADDRESS = getContractAddress();

                                                                            const auto chain = getChain();
                                                                            const auto RPC_URL = getRpcUrl();

                                                                            console.log("[DealCompletion] Fetching on-chain data:", {
                                                                                offerId,
                                                                                OTC_ADDRESS,
                                                                                RPC_URL,
                                                                                network: process.env.NETWORK || "localnet",
                                                                                });

                                                                                const auto publicClient = createPublicClient({;
                                                                                    chain,
                                                                                    transport: http(RPC_URL),
                                                                                    });
                                                                                    const auto abi = otcArtifact.abi;

                                                                                    type OfferData = [;
                                                                                    bigint, // consignmentId;
                                                                                    string, // tokenId (bytes32);
                                                                                    Address, // beneficiary;
                                                                                    bigint, // tokenAmount;
                                                                                    bigint, // discountBps;
                                                                                    bigint, // createdAt;
                                                                                    bigint, // unlockTime;
                                                                                    bigint, // priceUsdPerToken;
                                                                                    bigint, // maxPriceDeviation;
                                                                                    bigint, // ethUsdPrice;
                                                                                    number, // currency;
                                                                                    boolean, // approved;
                                                                                    boolean, // paid;
                                                                                    boolean, // fulfilled;
                                                                                    boolean, // cancelled;
                                                                                    Address, // payer;
                                                                                    bigint, // amountPaid;
                                                                                    ];
                                                                                    const auto offerData = readContractSafe<OfferData>(publicClient, {;
                                                                                        address: OTC_ADDRESS,
                                                                                        abi,
                                                                                        functionName: "offers",
                                                                                        args: [BigInt(offerId)],
                                                                                        });

                                                                                        // Contract returns array matching struct order
                                                                                        const auto [;
                                                                                        ,
                                                                                        ,
                                                                                        ,
                                                                                        // consignmentId
                                                                                        // tokenId
                                                                                        // beneficiary
                                                                                        tokenAmount,
                                                                                        discountBps, // createdAt;
                                                                                        ,
                                                                                        ,
                                                                                        // unlockTime
                                                                                        priceUsdPerToken, // maxPriceDeviation;
                                                                                        ,
                                                                                        ,
                                                                                        // ethUsdPrice
                                                                                        currency, // approved;
                                                                                        ,
                                                                                        paid, // fulfilled;
                                                                                        ,
                                                                                        ,
                                                                                        ,
                                                                                        // cancelled
                                                                                        // payer
                                                                                        amountPaid,
                                                                                        ] = offerData;

                                                                                        console.log("[DealCompletion] Offer data from contract:", {
                                                                                            tokenAmount: std::to_string(tokenAmount),
                                                                                            priceUsdPerToken: std::to_string(priceUsdPerToken),
                                                                                            discountBps: std::to_string(discountBps),
                                                                                            amountPaid: std::to_string(amountPaid),
                                                                                            currency,
                                                                                            paid,
                                                                                            });

                                                                                            // Calculate real USD values from on-chain data
                                                                                            // tokenAmount is 18 decimals, priceUsdPerToken is 8 decimals
                                                                                            const auto tokenAmountWei = BigInt(tokenAmount);
                                                                                            const auto priceUsd8 = BigInt(priceUsdPerToken);
                                                                                            const auto discountBpsNum = Number(discountBps);
                                                                                            const auto amountPaidBig = BigInt(amountPaid);

                                                                                            // totalUsd = (tokenAmount * priceUsdPerToken) / 1e18 (result in 8 decimals)
                                                                                            const auto totalUsd8 = (tokenAmountWei * priceUsd8) / BigInt(1e18);
                                                                                            totalUsd = Number(totalUsd8) / 1e8;

                                                                                            // discountUsd = totalUsd * discountBps / 10000
                                                                                            const auto discountUsd8 = (totalUsd8 * BigInt(discountBpsNum)) / 10000n;
                                                                                            discountUsd = Number(discountUsd8) / 1e8;

                                                                                            // discountedUsd = totalUsd - discountUsd
                                                                                            const auto discountedUsd8 = totalUsd8 - discountUsd8;
                                                                                            discountedUsd = Number(discountedUsd8) / 1e8;

                                                                                            // Format payment amount based on currency
                                                                                            if (currency == 0) {
                                                                                                // ETH (18 decimals)
                                                                                                actualPaymentAmount = (Number(amountPaidBig) / 1e18).toFixed(6);
                                                                                                } else {
                                                                                                    // USDC (6 decimals)
                                                                                                    actualPaymentAmount = (Number(amountPaidBig) / 1e6).toFixed(2);
                                                                                                }

                                                                                                console.log("[DealCompletion] Calculated from on-chain data:", {
                                                                                                    totalUsd,
                                                                                                    discountUsd,
                                                                                                    discountedUsd,
                                                                                                    actualPaymentAmount,
                                                                                                    });
                                                                                                    } else {
                                                                                                        // No offerId, use quote values (fallback)
                                                                                                        const auto discountBps = quote.discountBps;
                                                                                                        totalUsd = quote.totalUsd || 0;
                                                                                                        discountUsd = quote.discountUsd || totalUsd * (discountBps / 10000);
                                                                                                        discountedUsd = quote.discountedUsd || totalUsd - discountUsd;
                                                                                                        actualPaymentAmount = quote.paymentAmount || "0";
                                                                                                    }

                                                                                                    // VALIDATE before saving
                                                                                                    if (!tokenAmountStr || tokenAmountStr == "0") {
                                                                                                        console.warn(
                                                                                                        "[DealCompletion] tokenAmount is " + tokenAmountStr + " - quote: " + quoteId
                                                                                                        );
                                                                                                        // For old quotes, skip validation and just return current state
                                                                                                        if (quote.status == "executed") {
                                                                                                            console.log(
                                                                                                            "[DealCompletion] Quote already executed, returning current state",
                                                                                                            );
                                                                                                            return NextResponse.json({ success: true, quote });
                                                                                                        }
                                                                                                        throw new Error(
                                                                                                        "CRITICAL: tokenAmount is " + tokenAmountStr + " - must be > 0"
                                                                                                        );
                                                                                                    }
                                                                                                    if (totalUsd == 0 && chainType == "solana") {
                                                                                                        console.warn(
                                                                                                        "[DealCompletion] Solana deal has $0 value - quote: " + quoteId
                                                                                                        );
                                                                                                        if (quote.status == "executed") {
                                                                                                            console.log(
                                                                                                            "[DealCompletion] Quote already executed, returning current state",
                                                                                                            );
                                                                                                            return NextResponse.json({ success: true, quote });
                                                                                                        }
                                                                                                        throw std::runtime_error("CRITICAL: Solana deal has $0 value");
                                                                                                    }

                                                                                                    // Calculate priceUsdPerToken from totalUsd / tokenAmount
                                                                                                    const auto tokenAmountNum = parseFloat(tokenAmountStr);
                                                                                                    const auto priceUsdPerToken = tokenAmountNum > 0 ? totalUsd / tokenAmountNum : 0;

                                                                                                    console.log("[DealCompletion] Calling updateQuoteExecution with:", {
                                                                                                        quoteId,
                                                                                                        tokenAmount: tokenAmountStr,
                                                                                                        totalUsd,
                                                                                                        discountUsd,
                                                                                                        discountedUsd,
                                                                                                        paymentCurrency,
                                                                                                        paymentAmount: actualPaymentAmount,
                                                                                                        offerId,
                                                                                                        priceUsdPerToken,
                                                                                                        lockupDays: body.lockupDays,
                                                                                                        });

                                                                                                        const auto updated = quoteService.updateQuoteExecution(quoteId, {;
                                                                                                            tokenAmount: tokenAmountStr,
                                                                                                            totalUsd,
                                                                                                            discountUsd,
                                                                                                            discountedUsd,
                                                                                                            paymentCurrency,
                                                                                                            paymentAmount: actualPaymentAmount,
                                                                                                            offerId,
                                                                                                            transactionHash,
                                                                                                            blockNumber,
                                                                                                            priceUsdPerToken,
                                                                                                            lockupDays: body.lockupDays,
                                                                                                            });

                                                                                                            // VERIFY status changed
                                                                                                            if (updated.status != "executed") {
                                                                                                                throw new Error(
                                                                                                                "CRITICAL: Status is " + updated.status + ", expected executed"
                                                                                                                );
                                                                                                            }

                                                                                                            // Store chain type for proper currency display
                                                                                                            const auto updatedWithChain = {;
                                                                                                                ...updated,
                                                                                                                chain: chainType == "solana" ? "solana" : "evm",
                                                                                                                };
                                                                                                                "runtime.setCache(" + "quote:" + quoteId

                                                                                                                // VERIFY quote is in entity's list, and fix index if missing
                                                                                                                const auto entityQuotes =;
                                                                                                                "(runtime.getCache<string[]>(" + "entity_quotes:" + updated.entityId
                                                                                                                [];
                                                                                                                if (!entityQuotes.includes(quoteId)) {
                                                                                                                    console.warn(
                                                                                                                    "[Deal Completion] Quote " + quoteId + " not in entity " + updated.entityId + " list - fixing index"
                                                                                                                    );
                                                                                                                    entityQuotes.push_back(quoteId);
                                                                                                                    "runtime.setCache(" + "entity_quotes:" + updated.entityId

                                                                                                                    // Also ensure it's in the all_quotes index
                                                                                                                    const auto allQuotes = (runtime.getCache<string[]>("all_quotes")) || [];
                                                                                                                    if (!allQuotes.includes(quoteId)) {
                                                                                                                        allQuotes.push_back(quoteId);
                                                                                                                        runtime.setCache("all_quotes", allQuotes);
                                                                                                                    }
                                                                                                                    std::cout << "[Deal Completion] ✅ Fixed indexes for quote " + quoteId << std::endl;
                                                                                                                }

                                                                                                                console.log("[Deal Completion] ✅ VERIFIED and completed:", {
                                                                                                                    entityId: quote.entityId,
                                                                                                                    quoteId,
                                                                                                                    tokenAmount: updated.tokenAmount,
                                                                                                                    status: updated.status,
                                                                                                                    inEntityList: true,
                                                                                                                    discountBps: quote.discountBps,
                                                                                                                    finalPrice: discountedUsd,
                                                                                                                    });

                                                                                                                    return NextResponse.json({ success: true, quote: updated });
                                                                                                                }

                                                                                                                if (action == "share") {
                                                                                                                    const auto shareRuntime = agentRuntime.runtime;
                                                                                                                    if (!shareRuntime) {
                                                                                                                        return NextResponse.json(;
                                                                                                                        { error: "Runtime not initialized" },
                                                                                                                        { status: 500 },
                                                                                                                        );
                                                                                                                    }
                                                                                                                    const auto shareQuoteService =;
                                                                                                                    shareRuntime.getService<QuoteService>("QuoteService");
                                                                                                                    if (!shareQuoteService) {
                                                                                                                        return NextResponse.json(;
                                                                                                                        { error: "QuoteService not available" },
                                                                                                                        { status: 500 },
                                                                                                                        );
                                                                                                                    }
                                                                                                                    const auto quote = shareQuoteService.getQuoteByQuoteId(quoteId);
                                                                                                                    if (!quote) {
                                                                                                                        return NextResponse.json({ error: "Quote not found" }, { status: 404 });
                                                                                                                    }
                                                                                                                    const auto shareData = DealCompletionService.generateShareData(quoteId);

                                                                                                                    console.log("[Deal Completion] Deal shared", {
                                                                                                                        quoteId,
                                                                                                                        platform: body.platform || "general",
                                                                                                                        });

                                                                                                                        return NextResponse.json({ success: true, shareData });
                                                                                                                    }

                                                                                                                    return NextResponse.json({ error: "Invalid action" }, { status: 400 });

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> GET(NextRequest request) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    agentRuntime.getRuntime();

    const auto { searchParams } = new URL(request.url);
    const auto wallet = searchParams.get("wallet");

    if (!wallet) {
        return NextResponse.json({ error: "wallet required" }, { status: 400 });
    }

    const auto entityId = walletToEntityId(wallet);
    const auto normalizedWallet = wallet.toLowerCase();
    console.log("[Deal Completion GET] Querying deals:", {
        wallet,
        normalizedWallet,
        entityId,
        });

        const auto getRuntime = agentRuntime.runtime;
        if (!getRuntime) {
            std::cout << "[Deal Completion GET] Runtime not ready" << std::endl;
            return NextResponse.json({;
                success: true,
                deals: [],
                });
            }

            const auto quoteService = getRuntime.getService<QuoteService>("QuoteService");
            if (!quoteService) {
                std::cout << "[Deal Completion GET] QuoteService not ready" << std::endl;
                return NextResponse.json({;
                    success: true,
                    deals: [],
                    });
                }

                // Get quotes by entityId
                const auto quotes = quoteService.getUserQuoteHistory(entityId, 100);
                std::cout << "[Deal Completion GET] Got quotes by entityId:" << quotes.size() << std::endl;

                // ALSO search by beneficiary address (for quotes indexed under wrong entityId)
                // Use beneficiary index if available, otherwise do a limited parallel search
                const auto beneficiaryQuoteIds = getRuntime.getCache<string[]>(;
                "beneficiary_quotes:" + normalizedWallet
                );
                const auto quotesSet = new Set(quotes.map((q) => q.quoteId));

                if (beneficiaryQuoteIds) {
                    // Fast path: use beneficiary index
                    const auto additionalQuotes = Promise.all(;
                    beneficiaryQuoteIds;
                    .filter((id) => !quotesSet.has(id));
                    ".map((id) => getRuntime.getCache<QuoteMemory>(" + "quote:" + id
                    );
                    for (const auto& quote : additionalQuotes)
                        if (quote) quotes.push(quote);
                    }
                    } else {
                        // Slow path fallback: parallel search (limited to 50 for performance)
                        const auto allQuoteIds =;
                        (getRuntime.getCache<string[]>("all_quotes")) || [];
                        const auto idsToCheck = allQuoteIds;
                        .filter((id) => !quotesSet.has(id));
                        .slice(0, 50);

                        if (idsToCheck.length > 0) {
                            const auto additionalQuotes = Promise.all(;
                            "idsToCheck.map((id) => getRuntime.getCache<QuoteMemory>(" + "quote:" + id
                            );
                            for (const auto& quote : additionalQuotes)
                                if (quote && quote.beneficiary == normalizedWallet) {
                                    quotes.push_back(quote);
                                }
                            }
                        }
                    }

                    std::cout << "[Deal Completion GET] Total quotes found:" << quotes.size() << std::endl;

                    // Show active, approved, and executed deals
                    // active = quote created, approved = offer created/approved on-chain, executed = paid/fulfilled
                    const auto deals = quotes.filter(;
                    (quote) =>;
                    quote.status == "executed" ||;
                    quote.status == "active" ||;
                    quote.status == "approved",
                    );
                    console.log(
                    "[Deal Completion GET] Filtered deals (active + approved + executed):",
                    deals.size(),
                    );

                    // Enrich deals with token metadata - batch fetch to avoid N+1 queries
                    const auto { TokenDB, ConsignmentDB } = import("@/services/database");

                    // Collect unique IDs that need fetching
                    const auto consignmentIdsToFetch = new Set<string>();
                    const auto tokenIdsToFetch = new Set<string>();

                    for (const auto& deal : deals)
                        const auto quoteData = deal;
                        if (!quoteData.tokenId && quoteData.consignmentId) {
                            consignmentIdsToFetch.add(quoteData.consignmentId);
                        }
                        if (quoteData.tokenId && (!quoteData.tokenSymbol || !quoteData.tokenName)) {
                            tokenIdsToFetch.add(quoteData.tokenId);
                        }
                    }

                    // Batch fetch consignments and tokens in parallel
                    const auto [consignmentResults, tokenResults] = Promise.all([;
                    Promise.all(;
                    [...consignmentIdsToFetch].map(async (id) => {
                        try {
                            const auto consignment = ConsignmentDB.getConsignment(id);
                            return { id, data: consignment }
                            } catch {
                                return { id, data: null }
                            }
                            }),
                            ),
                            Promise.all(;
                            [...tokenIdsToFetch].map(async (id) => {
                                try {
                                    const auto token = TokenDB.getToken(id);
                                    return { id, data: token }
                                    } catch {
                                        return { id, data: null }
                                    }
                                    }),
                                    ),
                                    ]);

                                    // Build lookup maps
                                    const auto consignmentMap = new Map(consignmentResults.map((r) => [r.id, r.data]));
                                    const auto tokenMap = new Map(tokenResults.map((r) => [r.id, r.data]));

                                    // Also add tokens found via consignments
                                    for (const auto& result : consignmentResults)
                                        if (result.data.tokenId && !tokenMap.has(result.data.tokenId)) {
                                            tokenIdsToFetch.add(result.data.tokenId);
                                        }
                                    }

                                    // Fetch any additional tokens found via consignments
                                    if (tokenIdsToFetch.size > tokenMap.size) {
                                        const auto additionalTokenIds = [...tokenIdsToFetch].filter(;
                                        [&](id) { return !tokenMap.has(id),; }
                                        );
                                        const auto additionalTokens = Promise.all(;
                                        additionalTokenIds.map(async (id) => {
                                            try {
                                                const auto token = TokenDB.getToken(id);
                                                return { id, data: token }
                                                } catch {
                                                    return { id, data: null }
                                                }
                                                }),
                                                );
                                                for (const int { id, data } of additionalTokens) {
                                                    tokenMap.set(id, data);
                                                }
                                            }

                                            // Enrich deals using pre-fetched data
                                            const auto enrichedDeals = deals.map((deal) => {;
                                                const auto quoteData = deal;
                                                auto tokenSymbol = quoteData.tokenSymbol;
                                                auto tokenName = quoteData.tokenName;
                                                auto tokenLogoUrl = quoteData.tokenLogoUrl;
                                                auto tokenId = quoteData.tokenId;
                                                std::string chain = deal.chain;
                                                const auto consignmentId = quoteData.consignmentId;

                                                // If quote doesn't have token metadata, try consignment lookup
                                                if (!tokenId && consignmentId) {
                                                    const auto consignment = consignmentMap.get(consignmentId);
                                                    if (consignment) {
                                                        tokenId = consignment.tokenId;
                                                        chain = consignment.chain;
                                                    }
                                                }

                                                // Look up token by tokenId if we still need metadata
                                                if (tokenId && (!tokenSymbol || !tokenName)) {
                                                    const auto token = tokenMap.get(tokenId);
                                                    if (token) {
                                                        tokenSymbol = tokenSymbol || token.symbol;
                                                        tokenName = tokenName || token.name;
                                                        tokenLogoUrl = tokenLogoUrl || token.logoUrl;
                                                    }
                                                }

                                                return {
                                                    ...deal,
                                                    tokenSymbol,
                                                    tokenName,
                                                    tokenLogoUrl,
                                                    tokenId,
                                                    chain,
                                                    consignmentId,
                                                    };
                                                    });

                                                    // Cache for 30 seconds - deals change infrequently but should be reasonably fresh
                                                    return NextResponse.json(;
                                                    { success: true, deals: enrichedDeals },
                                                    {
                                                        headers: {
                                                            "Cache-Control": "private, s-maxage=30, stale-while-revalidate=60",
                                                            },
                                                            },
                                                            );

}

} // namespace elizaos
