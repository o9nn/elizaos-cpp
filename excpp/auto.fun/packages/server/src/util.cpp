#include "util.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

double convertToBasisPoints(double feePercent) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (feePercent >= 1) {
        return feePercent;
    }
    return Math.floor(feePercent * 10000);

}

double calculateAmountOutSell(double reserveLamport, double amount, double _tokenDecimals, double platformSellFee, double reserveToken) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto feeBasisPoints = convertToBasisPoints(platformSellFee);
    const auto amountBN = new BN(amount);

    // Apply fee: adjusted_amount = amount * (10000 - fee_basis_points) / 10000
    const auto adjustedAmount = amountBN;
    .mul(new BN(10000 - feeBasisPoints));
    .div(new BN(10000));

    // For selling tokens: amount_out = reserve_lamport * adjusted_amount / (reserve_token + adjusted_amount)
    const auto numerator = new BN(reserveLamport).mul(adjustedAmount);
    const auto denominator = new BN(reserveToken).add(adjustedAmount);

    return numerator.div(denominator).toNumber();

}

double calculateAmountOutBuy(double reserveToken, double amount, double _solDecimals, double reserveLamport, double platformBuyFee) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto feeBasisPoints = convertToBasisPoints(platformBuyFee);
    const auto amountBN = new BN(amount);

    // Apply fee: adjusted_amount = amount * (10000 - fee_basis_points) / 10000
    const auto adjustedAmount = amountBN;
    .mul(new BN(10000 - feeBasisPoints));
    .div(new BN(10000));

    const auto numerator = new BN(reserveToken).mul(adjustedAmount);
    const auto denominator = new BN(reserveLamport).add(adjustedAmount);

    return numerator.div(denominator).toNumber();

}

std::future<void> getTxIdAndCreatorFromTokenAddress(const std::string& tokenAddress) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        std::cout << "tokenAddress: " + std::to_string(tokenAddress) << std::endl;

        // Get a Solana config with the right environment
        const auto solanaConfig = initSolanaConfig();

        const auto transactionHistory =;
        solanaConfig.connection.getSignaturesForAddress(;
        new PublicKey(tokenAddress);
        );

        if (transactionHistory.length > 0) {
            const auto tokenCreationTxId =;
            transactionHistory[transactionHistory.length - 1].signature;
            const auto transactionDetails =;
            solanaConfig.connection.getTransaction(tokenCreationTxId);

            if (
            transactionDetails &&;
            transactionDetails.transaction &&;
            transactionDetails.transaction.message;
            ) {
                // The creator address is typically the first account in the transaction's account keys
                const auto creatorAddress =;
                transactionDetails.transaction.message.accountKeys[0].toBase58();
                return { tokenCreationTxId, creatorAddress }
            }
        }

        throw std::runtime_error(`No transaction found for token address: ${tokenAddress}`);

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<std::optional<Token>> createNewTokenData(const std::string& txId, const std::string& tokenAddress, const std::string& creatorAddress) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            // Get a Solana config with the right environment
            const auto solanaConfig = initSolanaConfig();

            std::cout << "solanaConfig" << solanaConfig << std::endl;

            const auto metadata = fetchMetadataWithBackoff(;
            solanaConfig.umi,
            tokenAddress;
            );
            std::cout << "Fetched metadata for token " + std::to_string(tokenAddress) + ":" << std::endl;

            const auto [bondingCurvePda] = PublicKey.findProgramAddressSync(;
            [Buffer.from(SEED_BONDING_CURVE), new PublicKey(tokenAddress).toBytes()],
            solanaConfig.programId;
            );
            if (!solanaConfig.wallet) {
                throw std::runtime_error("Wallet not found in Solana config");
            }
            const auto program = getProgram(;
            solanaConfig.connection,
            new Wallet(solanaConfig.wallet);
            );
            // Fetch the account data directly using the connection instead of Anchor program
            const auto bondingCurveAccount =;
            program.account.bondingCurve.fetchNullable(bondingCurvePda);

            std::optional<TokenMetadataJson> additionalMetadata = nullptr;
            try {
                const auto response = fetch(metadata.metadata.uri);
                additionalMetadata = (response.json());
                } catch (error) {
                    logger.error(
                    "Failed to fetch IPFS metadata from URI: " + std::to_string(metadata.metadata.uri)
                    error;
                    );
                }

                // Get TOKEN_DECIMALS from env if available, otherwise use default
                const auto TOKEN_DECIMALS = Number(process.env.DECIMALS || 6);

                const auto solPrice = getSOLPrice();

                if (!bondingCurveAccount) {
                    throw new Error(
                    "Bonding curve account not found for token " + std::to_string(tokenAddress);
                    );
                }
                std::cout << "bondingCurveAccount" << bondingCurveAccount << std::endl;
                std::cout << "reserveToken" << Number(bondingCurveAccount.reserveToken) << std::endl;
                std::cout << "reserveLamport" << Number(bondingCurveAccount.reserveLamport) << std::endl;
                std::cout << "curveLimit" << Number(bondingCurveAccount.curveLimit) << std::endl;

                const auto currentPrice =;
                Number(bondingCurveAccount.reserveToken) > 0;
                ? Number(bondingCurveAccount.reserveLamport) /;
                1e9 /;
                (Number(bondingCurveAccount.reserveToken) /;
                Math.pow(10, TOKEN_DECIMALS));
                : 0;
                std::cout << "currentPrice" << currentPrice << std::endl;

                const auto tokenPriceInSol = currentPrice / Math.pow(10, TOKEN_DECIMALS);
                std::cout << "tokenPriceInSol" << tokenPriceInSol << std::endl;
                const auto tokenPriceUSD =;
                currentPrice > 0;
                ? tokenPriceInSol * solPrice * Math.pow(10, TOKEN_DECIMALS);
                : 0;
                std::cout << "tokenPriceUSD" << tokenPriceUSD << std::endl;

                // Get TOKEN_SUPPLY from env if available, otherwise use default
                const auto supply = updateTokenSupplyFromChain(tokenAddress);
                const auto tokenSupply = supply.tokenSupply;
                ? Number(supply.tokenSupply);
                : Number(process.env.TOKEN_SUPPLY);
                const auto marketCapUSD =;
                (tokenSupply / Math.pow(10, TOKEN_DECIMALS)) * tokenPriceUSD;
                std::cout << "marketCapUSD" << marketCapUSD << std::endl;

                // Get virtual reserves from env if available, otherwise use default
                const auto virtualReserves = process.env.VIRTUAL_RESERVES;
                ? Number(process.env.VIRTUAL_RESERVES);
                : 100000000;

                // Get curve limit from env if available, otherwise use default
                const auto curveLimit = Number(process.env.CURVE_LIMIT);

                const std::optional<Token> tokenData = {;
                    id: tokenAddress, // Use mint key
                    name: metadata.metadata.name,
                    ticker: metadata.metadata.symbol,
                    url: metadata.metadata.uri,
                    image: additionalMetadata.image || "",
                    twitter: additionalMetadata.twitter || "",
                    telegram: additionalMetadata.telegram || "",
                    farcaster: additionalMetadata.farcaster || "",
                    website: additionalMetadata.website || "",
                    description: additionalMetadata.description || "",
                    mint: tokenAddress,
                    creator: creatorAddress,
                    reserveAmount: Number(bondingCurveAccount.reserveToken),
                    reserveLamport: Number(bondingCurveAccount.reserveLamport),
                    virtualReserves: virtualReserves,
                    liquidity:
                    (Number(bondingCurveAccount.reserveLamport) / 1e9) * solPrice +;
                    (Number(bondingCurveAccount.reserveToken) /;
                    Math.pow(10, TOKEN_DECIMALS)) *;
                    tokenPriceUSD,
                    currentPrice:
                    Number(bondingCurveAccount.reserveLamport) /;
                    1e9 /;
                    (Number(bondingCurveAccount.reserveToken) /;
                    Math.pow(10, TOKEN_DECIMALS)),
                    marketCapUSD: marketCapUSD,
                    tokenPriceUSD: tokenPriceUSD,
                    solPriceUSD: solPrice,
                    curveProgress:
                    ((Number(bondingCurveAccount.reserveLamport) - virtualReserves) /;
                    (curveLimit - virtualReserves)) *;
                    100,
                    curveLimit: curveLimit,
                    status: "active",
                    priceChange24h: 0,
                    price24hAgo: tokenPriceUSD,
                    volume24h: 0,
                    inferenceCount: 0,
                    holderCount: 0,
                    marketId: nullptr,
                    txId,
                    tokenSupply: tokenSupply.toString(),
                    tokenSupplyUiAmount: tokenSupply / Math.pow(10, TOKEN_DECIMALS),
                    tokenDecimals: TOKEN_DECIMALS,
                    lastSupplyUpdate: new Date(),
                    createdAt: new Date(),
                    lastUpdated: new Date(),
                    };

                    return tokenData;
                    } catch (error) {
                        std::cerr << "Error processing new token log:" << error << std::endl;
                        throw std::runtime_error("Error processing new token log: " + error);
                    }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<std::vector<Token>> bulkUpdatePartialTokens(const std::vector<Token>& tokens) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!tokens || tokens.length == 0) {
        return [];
    }

    // Get SOL price once for all tokens
    const auto solPrice = getSOLPrice();

    // Process each token in parallel
    const auto updatedTokensPromises = tokens.map((token) =>;
    calculateTokenMarketData(token, solPrice);
    );

    // Wait for all updates to complete
    return Promise.all(updatedTokensPromises);

}

std::future<> execWithdrawTx(Transaction tx, Connection connection, const std::any& wallet, auto maxRetries) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    signature: string; logs: string[]
}

std::vector<std::string> splitIntoLines(std::optional<std::string> text) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!text) return undefined;
    return text;
    .split("\n");
    .map((line) => line.trim().replace("\n", ""));
    .filter((line) => line.length > 0);

}

std::future<void> getFeaturedMaxValues(const std::any& db) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Get max values for normalization with a subquery
    try {
        const auto maxValues = db;
        .select({
            "MAX(COALESCE(" + std::to_string(tokens.volume24h) + ", 0))"
            "MAX(COALESCE(" + std::to_string(tokens.holderCount) + ", 0))"
            });
            .from(tokens);
            std::to_string(tokens.status) + " != 'pending'";

            // Extract max values, default to 1 to avoid division by zero
            return {
                maxVolume: Number(maxValues[0].maxVolume) || 1,
                maxHolders: Number(maxValues[0].maxHolders) || 1,
                };
                } catch (error) {
                    std::cerr << "Error getting max values for featured sort:" << error << std::endl;
                    return { maxVolume: 1, maxHolders: 1 }; // Default values on error
                }

}

void getFeaturedScoreExpression(double maxVolume, double maxHolders) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // Use provided max values, defaulting to 1 to avoid division by zero
    const auto normalizedMaxVolume = maxVolume || 1;
    const auto normalizedMaxHolders = maxHolders || 1;

    // Return the weighted score SQL expression
    return sql`(;
    (COALESCE(${tokens.volume24h}, 0) / ${normalizedMaxVolume} * 0.7) +;
    (COALESCE(${tokens.holderCount}, 0) / ${normalizedMaxHolders} * 0.3);
    )`;

}

double calculateFeaturedScore(std::optional<std::any> token, double maxVolume, double maxHolders) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto normalizedMaxVolume = maxVolume || 1;
    const auto normalizedMaxHolders = maxHolders || 1;

    const auto volume = token.volume24h || 0;
    const auto holders = token.holderCount || 0;

    return (;
    (volume / normalizedMaxVolume) * 0.7 +;
    (holders / normalizedMaxHolders) * 0.3;
    );

}

void applyFeaturedSort(const std::any& tokensQuery, double maxVolume, double maxHolders, const std::string& sortOrder) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto featuredScore = getFeaturedScoreExpression(maxVolume, maxHolders);

    if (sortOrder.toLowerCase() == "desc") {
        return tokensQuery.orderBy(desc(featuredScore));
        } else {
            return tokensQuery.orderBy(featuredScore);
        }

}

} // namespace elizaos
