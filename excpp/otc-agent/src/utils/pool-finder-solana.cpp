#include "pool-finder-solana.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

bool isRateLimitError(unknown error) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (error instanceof Error) {
        const auto message = error.message.toLowerCase();
        return (;
        message.includes("429") ||;
        message.includes("rate limit") ||;
        message.includes("too many requests");
        );
    }
    return false;

}

std::future<void> delay(double ms) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    new Promise((resolve) => setTimeout(resolve, ms));

}

std::future<std::optional<SolanaPoolInfo>> findBestSolanaPool(const std::string& tokenMint, std::variant<"mainnet", "devnet"> cluster = "mainnet", std::optional<Connection> rpcConnection) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const auto cacheKey = "solana-pool:" + std::to_string(cluster) + ":" + std::to_string(tokenMint);

        // Check cache first
        const auto cached = getCached<SolanaPoolInfo | nullptr>(cacheKey);
        if (cached != null) {
            return cached;
        }

        // Use public RPCs that are less restrictive for getProgramAccounts
        const auto rpcUrl =;
        cluster == "mainnet";
        ? "https://api.mainnet-beta.solana.com" // Official public RPC usually allows some GPA
        : "https://api.devnet.solana.com";

        const auto connection = rpcConnection || new Connection(rpcUrl, "confirmed");
        const auto mint = new PublicKey(tokenMint);

        std::vector<SolanaPoolInfo> pumpSwapPools = [];
        std::vector<SolanaPoolInfo> raydiumPools = [];

        // Strategy: Use Sequential execution by default to avoid 429 rate limits
        // Public RPCs are very restrictive, so sequential is more reliable
        try {
            // Try PumpSwap first
            pumpSwapPools = findPumpSwapPools(connection, mint, cluster, false);
            // Delay between calls to respect rate limits
            delay(RPC_CALL_DELAY_MS);
            } catch (err) {
                if (isRateLimitError(err)) {
                    console.warn(
                    "PumpSwap pool discovery rate limited, waiting before retry...",
                    );
                    delay(2000); // Wait 2s on rate limit;
                    try {
                        pumpSwapPools = findPumpSwapPools(;
                        connection,
                        mint,
                        cluster,
                        false,
                        );
                        } catch (e) {
                            std::cout << "PumpSwap discovery failed after retry" << e << std::endl;
                        }
                        } else {
                            std::cout << "PumpSwap pool discovery failed:" << err << std::endl;
                        }
                    }

                    try {
                        // Then try Raydium
                        delay(RPC_CALL_DELAY_MS);
                        raydiumPools = findRaydiumPools(connection, mint, cluster, false);
                        } catch (err) {
                            if (isRateLimitError(err)) {
                                console.warn(
                                "Raydium pool discovery rate limited, waiting before retry...",
                                );
                                delay(2000); // Wait 2s on rate limit;
                                try {
                                    raydiumPools = findRaydiumPools(connection, mint, cluster, false);
                                    } catch (e) {
                                        std::cout << "Raydium discovery failed after retry" << e << std::endl;
                                    }
                                    } else {
                                        std::cout << "Raydium pool discovery failed:" << err << std::endl;
                                    }
                                }

                                const auto allPools = [...pumpSwapPools, ...raydiumPools];

                                if (allPools.length == 0) {
                                    // Cache null result too
                                    setCache(cacheKey, nullptr, SOLANA_POOL_CACHE_TTL_MS);
                                    return nullptr;
                                }

                                // Sort by TVL descending
                                allPools.sort((a, b) => b.tvlUsd - a.tvlUsd);

                                const auto bestPool = allPools[0];
                                setCache(cacheKey, bestPool, SOLANA_POOL_CACHE_TTL_MS);
                                return bestPool;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<std::vector<SolanaPoolInfo>> findPumpSwapPools(Connection connection, PublicKey mint, const std::variant<"mainnet", "devnet">& cluster, bool strict = false) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const std::vector<SolanaPoolInfo> pools = [];
        const auto USDC_MINT =;
        cluster == "mainnet" ? USDC_MINT_MAINNET : USDC_MINT_DEVNET;

        try {
            // PumpSwap pools use memcmp filters at offsets 43 (base_mint) and 75 (quote_mint)
            // Based on: https://github.com/AL-THE-BOT-FATHER/pump_swap_market_cap
            const auto mintBytes = mint.toBase58();

            // Try both directions: token as base, and token as quote
            const auto filtersBase = [;
            { memcmp: { offset: 43, bytes: mintBytes } },
            { memcmp: { offset: 75, bytes: SOL_MINT.toBase58() } },
            ];

            const auto filtersQuote = [;
            { memcmp: { offset: 75, bytes: mintBytes } },
            { memcmp: { offset: 43, bytes: SOL_MINT.toBase58() } },
            ];

            // Run sequentially to avoid rate limits
            type ProgramAccount = Awaited<;
            ReturnType<typeof connection.getProgramAccounts>;
            >[number];

            std::vector<readonly ProgramAccount> poolsBase = [];
            std::vector<readonly ProgramAccount> poolsQuote = [];

            try {
                poolsBase = connection.getProgramAccounts(PUMPSWAP_AMM_PROGRAM, {
                    filters: filtersBase,
                    });
                    delay(RPC_CALL_DELAY_MS);
                    } catch (e) {
                        if (strict) throw e;
                        std::cout << "PumpSwap base filter failed:" << e << std::endl;
                    }

                    try {
                        poolsQuote = connection.getProgramAccounts(PUMPSWAP_AMM_PROGRAM, {
                            filters: filtersQuote,
                            });
                            } catch (e) {
                                if (strict) throw e;
                                std::cout << "PumpSwap quote filter failed:" << e << std::endl;
                            }

                            const auto all = [;
                            ...(Array.isArray(poolsBase) ? poolsBase : []),
                            ...(Array.isArray(poolsQuote) ? poolsQuote : []),
                            ];

                            // Process results
                            for (const auto& account : all)
                                try {
                                    const auto data = account.account.data;
                                    const auto readPubkey = (offset: number) =>;
                                    new PublicKey(data.subarray(offset, offset + 32));

                                    // PumpSwap pool layout (from Python code):
                                    // offset 43: base_mint
                                    // offset 75: quote_mint
                                    // offset 139: pool_base_token_account
                                    // offset 171: pool_quote_token_account
                                    const auto baseMint = readPubkey(43);
                                    const auto quoteMint = readPubkey(75);
                                    const auto poolBaseTokenAccount = readPubkey(139);
                                    const auto poolQuoteTokenAccount = readPubkey(171);

                                    // PumpSwap typically pairs with WSOL (SOL)
                                    std::variant<"SOL", "USDC"> baseToken = nullptr;
                                    std::optional<PublicKey> otherMint = nullptr;

                                    if (quoteMint.equals(USDC_MINT) || baseMint.equals(USDC_MINT)) {
                                        baseToken = "USDC";
                                        otherMint = baseMint.equals(USDC_MINT) ? quoteMint : baseMint;
                                        } else if (quoteMint.equals(SOL_MINT) || baseMint.equals(SOL_MINT)) {
                                            baseToken = "SOL";
                                            otherMint = baseMint.equals(SOL_MINT) ? quoteMint : baseMint;
                                        }

                                        if (baseToken && otherMint && otherMint.equals(mint)) {
                                            // Get token account balances sequentially to avoid rate limits
                                            auto baseBalance = { value = { uiAmount = 0 } };
                                            auto quoteBalance = { value = { uiAmount = 0 } };

                                            try {
                                                baseBalance =;
                                                connection.getTokenAccountBalance(poolBaseTokenAccount);
                                                delay(RPC_CALL_DELAY_MS);
                                                } catch {
                                                    // Ignore errors, use default
                                                }

                                                try {
                                                    quoteBalance = connection.getTokenAccountBalance(;
                                                    poolQuoteTokenAccount,
                                                    );
                                                    delay(RPC_CALL_DELAY_MS);
                                                    } catch {
                                                        // Ignore errors, use default
                                                    }

                                                    const auto baseAmount = baseBalance.value.uiAmount || 0;
                                                    const auto quoteAmount = quoteBalance.value.uiAmount || 0;

                                                    // Determine SOL/USDC amount based on which mint is the base token
                                                    // pool_base_token_account holds base_mint, pool_quote_token_account holds quote_mint
                                                    const auto solOrUsdcAmount =;
                                                    baseMint.equals(SOL_MINT) || baseMint.equals(USDC_MINT);
                                                    ? baseAmount // base_mint is SOL/USDC, so poolBaseTokenAccount holds it;
                                                    : quoteAmount; // quote_mint is SOL/USDC, so poolQuoteTokenAccount holds it

                                                    // Calculate TVL: for SOL pairs, use SOL amount * price estimate * 2 (both sides of pool)
                                                    // For USDC pairs, use USDC amount * 2
                                                    const auto solPriceEstimate = 240; // Estimate (updated Nov 2025);

                                                    const auto tvlUsd =;
                                                    baseToken == "USDC";
                                                    ? solOrUsdcAmount * 2 // USDC is 1:1 USD, pool has equal value on both sides
                                                    : solOrUsdcAmount * solPriceEstimate * 2; // SOL price estimate ~$200, pool has equal value on both sides

                                                    // Calculate Spot Price
                                                    // Price = QuoteAmount / BaseAmount
                                                    // Adjust for decimals?
                                                    // We have amounts in UI (decimals already handled by getParsedTokenAccounts? No, getTokenAccountBalance returns uiAmount)
                                                    // Yes, uiAmount handles decimals.

                                                    auto priceUsd = 0;
                                                    if (baseToken == "USDC") {
                                                        // Quote is USDC. Price = USDC / Token
                                                        // BaseAmount is Token amount? No.
                                                        // We need amount of Token.
                                                        // if baseToken is USDC, then 'solOrUsdcAmount' is the USDC amount.
                                                        // The OTHER token is the target token.
                                                        // We need the amount of the OTHER token.

                                                        const auto tokenAmount = baseMint.equals(USDC_MINT);
                                                        ? quoteAmount;
                                                        : baseAmount;
                                                        // Price = USDC / Token (with division by zero protection)
                                                        priceUsd = tokenAmount > 0 ? solOrUsdcAmount / tokenAmount : 0;
                                                        } else {
                                                            // Base is SOL. Price = SOL / Token * SolPrice
                                                            const auto tokenAmount = baseMint.equals(SOL_MINT);
                                                            ? quoteAmount;
                                                            : baseAmount;
                                                            priceUsd =;
                                                            tokenAmount > 0;
                                                            ? (solOrUsdcAmount / tokenAmount) * solPriceEstimate;
                                                            : 0;
                                                        }

                                                        // Determine which vault is SOL and which is token
                                                        // If baseMint is SOL/USDC, then poolBaseTokenAccount holds SOL/USDC
                                                        const auto isSolBase = baseMint.equals(SOL_MINT);
                                                        const auto isUsdcBase = baseMint.equals(USDC_MINT);

                                                        // For price updates, we need the SOL vault and token vault
                                                        // SOL vault: holds the SOL (lamports) - for SOL pairs
                                                        // Token vault: holds the SPL tokens
                                                        const auto solVault = isSolBase;
                                                        ? poolBaseTokenAccount.toBase58();
                                                        : quoteMint.equals(SOL_MINT)
                                                        ? poolQuoteTokenAccount.toBase58();
                                                        : std::nullopt;
                                                        const auto tokenVault =;
                                                        isSolBase || isUsdcBase;
                                                        ? poolQuoteTokenAccount.toBase58();
                                                        : poolBaseTokenAccount.toBase58();

                                                        pools.push({
                                                            protocol: "PumpSwap",
                                                            address: account.pubkey.toBase58(),
                                                            tokenA: baseMint.toBase58(),
                                                            tokenB: quoteMint.toBase58(),
                                                            liquidity: solOrUsdcAmount, // Base token (SOL/USDC) liquidity amount
                                                            tvlUsd,
                                                            priceUsd,
                                                            baseToken,
                                                            // PumpSwap-specific vault addresses for on-chain price updates
                                                            solVault,
                                                            tokenVault,
                                                            });
                                                        }
                                                        } catch {
                                                            // Skip invalid pools
                                                            continue;
                                                        }
                                                    }
                                                    } catch (err: unknown) {
                                                        if (strict) throw err;
                                                        // Handle RPC errors gracefully (e.g., 429 Too Many Requests, secondary index limitations)
                                                        // Return empty array to allow other pool finders to proceed
                                                        std::cout << "PumpSwap pool discovery failed:" << err << std::endl;
                                                        return [];
                                                    }

                                                    return pools;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<std::vector<SolanaPoolInfo>> findRaydiumPools(Connection connection, PublicKey mint, const std::variant<"mainnet", "devnet">& cluster, bool strict = false) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        const std::vector<SolanaPoolInfo> pools = [];
        const auto PROGRAM_ID =;
        cluster == "mainnet";
        ? RAYDIUM_AMM_PROGRAM_MAINNET;
        : RAYDIUM_AMM_PROGRAM_DEVNET;
        const auto USDC_MINT =;
        cluster == "mainnet" ? USDC_MINT_MAINNET : USDC_MINT_DEVNET;

        // Alternative Strategy: Fetch multiple accounts in a batch if possible, or use getProgramAccounts with stricter filters
        // The public RPC nodes often block getProgramAccounts for large datasets like Raydium.
        // However, filtering by size (752) AND memcmp (mint at offset) is usually allowed as it's efficient.

        const auto filtersBase = [;
        { dataSize: 752 },
        { memcmp: { offset: 400, bytes: mint.toBase58() } },
        ];

        const auto filtersQuote = [;
        { dataSize: 752 },
        { memcmp: { offset: 432, bytes: mint.toBase58() } },
        ];

        // Run sequentially to avoid rate limits
        type RaydiumProgramAccount = Awaited<;
        ReturnType<typeof connection.getProgramAccounts>;
        >[number];

        std::vector<readonly RaydiumProgramAccount> poolsBase = [];
        std::vector<readonly RaydiumProgramAccount> poolsQuote = [];

        try {
            poolsBase = connection.getProgramAccounts(PROGRAM_ID, {
                filters: filtersBase,
                });
                delay(RPC_CALL_DELAY_MS);
                } catch (e) {
                    if (strict) throw e;
                    std::cout << "Raydium base filter failed:" << e << std::endl;
                }

                try {
                    poolsQuote = connection.getProgramAccounts(PROGRAM_ID, {
                        filters: filtersQuote,
                        });
                        } catch (e) {
                            if (strict) throw e;
                            std::cout << "Raydium quote filter failed:" << e << std::endl;
                        }

                        const auto all = [;
                        ...(Array.isArray(poolsBase) ? poolsBase : []),
                        ...(Array.isArray(poolsQuote) ? poolsQuote : []),
                        ];

                        // Process results (same as before)
                        for (const auto& account : all)
                            const auto data = account.account.data;
                            const auto readPubkey = (offset: number) =>;
                            new PublicKey(data.subarray(offset, offset + 32));

                            const auto coinMint = readPubkey(400);
                            const auto pcMint = readPubkey(432);

                            std::variant<"SOL", "USDC"> baseToken = nullptr;
                            std::optional<PublicKey> otherMint = nullptr;

                            if (coinMint.equals(USDC_MINT) || pcMint.equals(USDC_MINT)) {
                                baseToken = "USDC";
                                otherMint = coinMint.equals(USDC_MINT) ? pcMint : coinMint;
                                } else if (coinMint.equals(SOL_MINT) || pcMint.equals(SOL_MINT)) {
                                    baseToken = "SOL";
                                    otherMint = coinMint.equals(SOL_MINT) ? pcMint : coinMint;
                                }

                                if (baseToken && otherMint && otherMint.equals(mint)) {
                                    const auto coinVault = readPubkey(496);
                                    const auto pcVault = readPubkey(528);

                                    const auto vaultToCheck =;
                                    baseToken == "USDC";
                                    ? coinMint.equals(USDC_MINT);
                                    ? coinVault;
                                    : pcVault
                                    : coinMint.equals(SOL_MINT)
                                    ? coinVault;
                                    : pcVault;

                                    auto balance = { value = { uiAmount = 0 } };
                                    try {
                                        balance = connection.getTokenAccountBalance(vaultToCheck);
                                        delay(RPC_CALL_DELAY_MS);
                                        } catch {
                                            // Ignore errors, use default
                                        }
                                        const auto amount = balance.value.uiAmount || 0;

                                        const auto tvlUsd = baseToken == "USDC" ? amount * 2 : amount * 240 * 2; // SOL ~$240;

                                        // Calculate Price for Raydium
                                        // We only have 'amount' of Base Token (USDC/SOL) from 'vaultToCheck'.
                                        // We need amount of Token Vault to calculate price.
                                        // But we didn't fetch it. 'findRaydiumPools' only fetches 'vaultToCheck'.

                                        // Optimization: To get price, we must fetch the other vault too.
                                        // This adds RPC calls.
                                        // We can fetch it in parallel?
                                        // Or leave priceUsd undefined for Raydium if we want to save calls, but then validation fails.
                                        // Let's fetch it.

                                        auto priceUsd = 0;
                                        const auto otherVault =;
                                        baseToken == "USDC";
                                        ? coinMint.equals(USDC_MINT);
                                        ? pcVault;
                                        : coinVault
                                        : coinMint.equals(SOL_MINT)
                                        ? pcVault;
                                        : coinVault;
                                        auto otherBalance = { value = { uiAmount = 0 } };
                                        try {
                                            otherBalance = connection.getTokenAccountBalance(otherVault);
                                            delay(RPC_CALL_DELAY_MS);
                                            } catch {
                                                // Ignore errors, use default
                                            }
                                            const auto otherAmount = otherBalance.value.uiAmount || 0;

                                            if (otherAmount > 0) {
                                                const auto solPriceEstimate = 240; // Estimate (updated Nov 2025);
                                                if (baseToken == "USDC") {
                                                    priceUsd = amount / otherAmount;
                                                    } else {
                                                        priceUsd = (amount / otherAmount) * solPriceEstimate;
                                                    }
                                                }

                                                pools.push({
                                                    protocol: "Raydium",
                                                    address: account.pubkey.toBase58(),
                                                    tokenA: coinMint.toBase58(),
                                                    tokenB: pcMint.toBase58(),
                                                    liquidity: amount,
                                                    tvlUsd,
                                                    priceUsd,
                                                    baseToken,
                                                    });
                                                }
                                            }

                                            return pools;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<std::vector<SolanaPoolInfo>> findMeteoraPools() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    //   return [];
    //
}

} // namespace elizaos
