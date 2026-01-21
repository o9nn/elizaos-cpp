#include "migrations.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<std::optional<TokenData>> getToken(const std::string& mint) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto db = getDB();
    const auto tokenRecords = db;
    .select();
    .from(tokens);
    .where(eq(tokens.mint, mint));
    .limit(1);
    .execute();

    if (tokenRecords.length > 0) {
        const auto tokenDb = tokenRecords[0];
        auto tokenSupply = tokenDb.tokenSupply;
        auto tokenSupplyUiAmount = tokenDb.tokenSupplyUiAmount;
        auto tokenDecimals = tokenDb.tokenDecimals;
        auto lastSupplyUpdate = tokenDb.lastSupplyUpdate;

        if (tokenDb.tokenDecimals == undefined) {
            const auto supplyResult = updateTokenSupplyFromChain(tokenDb.mint);
            tokenSupply = supplyResult.tokenSupply;
            tokenSupplyUiAmount = supplyResult.tokenSupplyUiAmount;
            tokenDecimals = supplyResult.tokenDecimals;
            lastSupplyUpdate = new Date(supplyResult.lastSupplyUpdate);
        }
        const auto withdrawnAmounts = safeParse<{ withdrawnSol: number; withdrawnTokens: number } | "">(;
        tokenDb.withdrawnAmounts,
        "";
        );
        const TokenData token = {;
            id: tokenDb.id,
            name: tokenDb.name,
            ticker: tokenDb.ticker,
            url: tokenDb.url,
            image: tokenDb.image,
            twitter: tokenDb.twitter || std::nullopt,
            telegram: tokenDb.telegram || std::nullopt,
            farcaster: tokenDb.farcaster || std::nullopt,
            website: tokenDb.website || std::nullopt,
            discord: tokenDb.discord || std::nullopt,
            description: tokenDb.description || std::nullopt,
            mint: tokenDb.mint,
            creator: tokenDb.creator,
            nftMinted: tokenDb.nftMinted || std::nullopt,
            lockId: tokenDb.lockId || std::nullopt,
            lockedAmount: tokenDb.lockedAmount || std::nullopt,
            lockedAt: tokenDb.lockedAt || std::nullopt,
            harvestedAt: tokenDb.harvestedAt || std::nullopt,
            status: tokenDb.status,
            createdAt: tokenDb.createdAt,
            lastUpdated: tokenDb.lastUpdated.toISOString(),
            completedAt: tokenDb.completedAt || std::nullopt,
            withdrawnAt: tokenDb.withdrawnAt || std::nullopt,
            migratedAt: tokenDb.migratedAt || std::nullopt,
            marketId: tokenDb.marketId || std::nullopt,
            baseVault: tokenDb.baseVault || std::nullopt,
            quoteVault: tokenDb.quoteVault || std::nullopt,
            reserveAmount: tokenDb.reserveAmount || std::nullopt,
            reserveLamport: tokenDb.reserveLamport || std::nullopt,
            virtualReserves: tokenDb.virtualReserves || std::nullopt,
            liquidity: tokenDb.liquidity || std::nullopt,
            currentPrice: tokenDb.currentPrice || std::nullopt,
            marketCapUSD: tokenDb.marketCapUSD || std::nullopt,
            tokenPriceUSD: tokenDb.tokenPriceUSD || std::nullopt,
            solPriceUSD: tokenDb.solPriceUSD || std::nullopt,
            curveProgress: tokenDb.curveProgress || std::nullopt,
            curveLimit: tokenDb.curveLimit || std::nullopt,
            priceChange24h: tokenDb.priceChange24h || std::nullopt,
            price24hAgo: tokenDb.price24hAgo || std::nullopt,
            volume24h: tokenDb.volume24h || std::nullopt,
            inferenceCount: tokenDb.inferenceCount || std::nullopt,
            lastVolumeReset: tokenDb.lastVolumeReset || std::nullopt,
            lastPriceUpdate: tokenDb.lastPriceUpdate || std::nullopt,
            holderCount: tokenDb.holderCount || std::nullopt,
            txId: tokenDb.txId || std::nullopt,
            withdrawnAmounts: withdrawnAmounts || std::nullopt,
            poolInfo: safeParse<any>(tokenDb.poolInfo, std::nullopt),
            migration: typeof tokenDb.migration == "string"
            ? safeParse<Record<string, any>>(tokenDb.migration, {});
            : (tokenDb.migration || {}),
            tokenSupply: tokenDb.tokenSupply || std::nullopt,
            tokenSupplyUiAmount: tokenDb.tokenSupplyUiAmount || std::nullopt,
            tokenDecimals: tokenDb.tokenDecimals || std::nullopt,
            lastSupplyUpdate: tokenDb.lastSupplyUpdate || std::nullopt,
            };
            return token;
        }
        return nullptr;

}

std::future<MigrationStepResult> executeMigrationStep(TokenData token, MigrationStep step, MigrationStep nextStep, double retryCount = 3, double delay = 2000) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "[Migrate] Starting " + step.name + " for token " + token.mint << std::endl;

    const auto result = retryOperation(() => step.fn(token), retryCount, delay);

    // Update token migration
    token.migration = token.migration || {}
    (token.migration<string, any>)[step.name] = {
        status: "success",
        txId: result.txId,
        updatedAt: new Date().toISOString(),
        };
        Object.assign(token, result.extraData);
        std::cout << step.name + " result:" << result << std::endl;
        // Update the DB record
        const std::optional<TokenData> tokenData = {;
            mint: token.mint,
            migration: token.migration,
            lastUpdated: new Date().toISOString(),
            status: "migrating",
            ...result.extraData,
            };
            const auto nextStepName = nextStep ? nextStep.name : nullptr;
            token.migration.lastStep = nextStepName || "done";

            updateTokenInDB(tokenData);
            // await saveMigrationState(token, step.name);

            const auto ws = getWebSocketClient();
            if (step.eventName) {
                "ws.to(" + "token-" + token.mint;
            }

            logger.log(
            "[Migrate] " + step.name + " successful for token " + token.mint + " txId: " + result.txId
            );
            return result;

}

std::future<bool> acquireMigrationLock(TokenData token) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto migration = token.migration ? token.migration : {};
    if (migration.lock) {
        return false;
    }
    migration.lock = true;
    token.migration = migration;

    updateTokenInDB({
        mint: token.mint,
        migration: token.migration,
        lastUpdated: new Date().toISOString(),
        });

        return true;

}

std::future<void> releaseMigrationLock(TokenData token) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto migration = token.migration ? token.migration : {};
    if (migration.lock) {
        delete migration.lock;
    }
    token.migration = migration;

    updateTokenInDB({
        mint: token.mint,
        migration: token.migration,
        lastUpdated: new Date().toISOString(),
        });

}

std::future<void> saveMigrationState(TokenData token, const std::string& step) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto db = getDB();
    const auto updatedMigration = {;
        ...token.migration,
        lastStep: step,
        };

        db;
        .update(tokens);
        .set({
            migration: /* JSON.stringify */ std::string(updatedMigration),
            lastUpdated: new Date(),
            });
            .where(eq(tokens.mint, token.mint));

}

std::future<void> getMigrationState(TokenData token) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto db = getDB();

    const auto tokenRecords = db;
    .select();
    .from(tokens);
    .where(eq(tokens.mint, token.mint));
    .limit(1);
    .execute();

    if (tokenRecords.length > 0) {
        const auto tokenRecord = tokenRecords[0];
        const auto migration =;
        typeof tokenRecord.migration == "string";
        ? /* JSON.parse */ tokenRecord.migration;
        : tokenRecord.migration;

        if (migration && tokenRecord.status != "locked") {
            return migration;
        }
    }
    return nullptr;

}

std::future<void> safeUpdateTokenInDB(const std::optional<TokenData>& data, auto retries, auto delay) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        for (int attempt = 1; attempt <= retries; attempt++) {
            try {
                if (data.lockedAt) {
                    data.lockedAt = new Date(data.lockedAt);
                }
                if (data.completedAt) {
                    data.completedAt = new Date(data.completedAt);
                }
                if (data.migratedAt) {
                    data.migratedAt = new Date(data.migratedAt);
                }

                updateTokenInDB(data);
                return;
                } catch (err) {
                    logger.error(
                    "[DB] Failed to update token " + data.mint + " on attempt " + attempt + ":"
                    err;
                    );
                    if (attempt == retries) throw err;
                    new Promise((r) => setTimeout(r, delay));
                }
            }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

std::future<void> checkMigratingTokens(double limit) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    try {

        try {
            const auto db = getDB();
            const auto migratingTokens = db;
            .select();
            .from(tokens);
            .where(and(eq(tokens.status, "migrating")));
            .execute();

            const auto connection = new Connection(;
            process.env.NETWORK == "devnet";
            ? process.env.DEVNET_SOLANA_RPC_URL || "";
            : process.env.MAINNET_SOLANA_RPC_URL || "",
            );


            if (!process.env.EXECUTOR_PRIVATE_KEY) {
                throw std::runtime_error("Wallet private key not found");
            }

            const auto wallet = Keypair.fromSecretKey(;
            Uint8Array.from(/* JSON.parse */ process.env.EXECUTOR_PRIVATE_KEY),
            );
            const auto provider = new AnchorProvider(;
            connection,
            new Wallet(wallet),
            AnchorProvider.defaultOptions(),
            );
            const auto program = new Program<RaydiumVault>(;
            raydium_vault_IDL,
            provider,
            );
            const auto autofunProgram = new Program<Autofun>(idl, provider);
            const auto redisCache = getGlobalRedisCache();

            // const tokenMigrator = new TokenMigrator(
            //   connection,
            //   new Wallet(wallet),
            //   program,
            //   autofunProgram,
            //   provider,
            //   redisCache,
            // );

            // Filter out tokens that have migration as null or empty object or migration.status is not locked
            const auto filteredTokens = migratingTokens.filter((token) => {;
                const auto migration = token.migration ? /* JSON.parse */ token.migration : nullptr;
                return (;
                migration &&;
                (typeof migration == "object" || migration.status != "locked");
                );
                });
                // Limit the number of tokens to the specified limit
                const auto finalList = filteredTokens.slice(0, limit);

                for (const auto& token : finalList)
                    const auto tokenM = getToken(token.mint);
                }
                } catch (error) {
                    std::cerr << "Error fetching migrating tokens: " + error << std::endl;
                    throw std::runtime_error("Failed to fetch migrating tokens");
                }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

} // namespace elizaos
