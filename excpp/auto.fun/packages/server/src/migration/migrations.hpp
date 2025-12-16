#include ".cron.hpp"
#include ".db.hpp"
#include ".redis.hpp"
#include ".tokenSupplyHelpers.hpp"
#include ".tokenSupplyHelpers/customWallet.hpp"
#include ".util.hpp"
#include ".websocket-client.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#pragma once

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

;
;
;
;
;
;
;
;
;
;
;
;
;
;
;
;

const idl: Autofun = JSON.parse(JSON.stringify(idlJson));
const raydium_vault_IDL: RaydiumVault = JSON.parse(JSON.stringify(raydium_vault_IDL_JSON));
struct LockResult {
    std::string txId;
};


struct MigrationStepResult {
    std::string txId;
    std::optional<std::unordered_map<std::string, std::any>> extraData;
};


using MigrationStepFn = (
  token: TokenData,
) => Promise<MigrationStepResult>;

struct MigrationStep {
    std::string name;
    std::optional<std::string> description;
    MigrationStepFn fn;
    std::optional<std::string> eventName;
};


 catch (err) {
    logger.error(`Failed to JSON.parse Redis field: ${raw}`, err);
    return defaultValue;
  }
}

std::future<TokenData | null> getToken(const std::string& mint);
    const withdrawnAmounts = safeParse<{ withdrawnSol: number; withdrawnTokens: number } | "">(
      tokenDb.withdrawnAmounts,
      ""
    )
    const token: TokenData = {
      id: tokenDb.id,
      name: tokenDb.name,
      ticker: tokenDb.ticker,
      url: tokenDb.url,
      image: tokenDb.image,
      twitter: tokenDb.twitter ?? undefined,
      telegram: tokenDb.telegram ?? undefined,
      farcaster: tokenDb.farcaster ?? undefined,
      website: tokenDb.website ?? undefined,
      discord: tokenDb.discord ?? undefined,
      description: tokenDb.description ?? undefined,
      mint: tokenDb.mint,
      creator: tokenDb.creator,
      nftMinted: tokenDb.nftMinted ?? undefined,
      lockId: tokenDb.lockId ?? undefined,
      lockedAmount: tokenDb.lockedAmount ?? undefined,
      lockedAt: tokenDb.lockedAt ?? undefined,
      harvestedAt: tokenDb.harvestedAt ?? undefined,
      status: tokenDb.status,
      createdAt: tokenDb.createdAt,
      lastUpdated: tokenDb.lastUpdated.toISOString(),
      completedAt: tokenDb.completedAt ?? undefined,
      withdrawnAt: tokenDb.withdrawnAt ?? undefined,
      migratedAt: tokenDb.migratedAt ?? undefined,
      marketId: tokenDb.marketId ?? undefined,
      baseVault: tokenDb.baseVault ?? undefined,
      quoteVault: tokenDb.quoteVault ?? undefined,
      reserveAmount: tokenDb.reserveAmount ?? undefined,
      reserveLamport: tokenDb.reserveLamport ?? undefined,
      virtualReserves: tokenDb.virtualReserves ?? undefined,
      liquidity: tokenDb.liquidity ?? undefined,
      currentPrice: tokenDb.currentPrice ?? undefined,
      marketCapUSD: tokenDb.marketCapUSD ?? undefined,
      tokenPriceUSD: tokenDb.tokenPriceUSD ?? undefined,
      solPriceUSD: tokenDb.solPriceUSD ?? undefined,
      curveProgress: tokenDb.curveProgress ?? undefined,
      curveLimit: tokenDb.curveLimit ?? undefined,
      priceChange24h: tokenDb.priceChange24h ?? undefined,
      price24hAgo: tokenDb.price24hAgo ?? undefined,
      volume24h: tokenDb.volume24h ?? undefined,
      inferenceCount: tokenDb.inferenceCount ?? undefined,
      lastVolumeReset: tokenDb.lastVolumeReset ?? undefined,
      lastPriceUpdate: tokenDb.lastPriceUpdate ?? undefined,
      holderCount: tokenDb.holderCount ?? undefined,
      txId: tokenDb.txId ?? undefined,
      withdrawnAmounts: withdrawnAmounts ?? undefined,
      poolInfo: safeParse<any>(tokenDb.poolInfo, undefined),
      migration: typeof tokenDb.migration === "string"
        ? safeParse<Record<string, any>>(tokenDb.migration, {})
        : (tokenDb.migration ?? {}),
      tokenSupply: tokenDb.tokenSupply ?? undefined,
      tokenSupplyUiAmount: tokenDb.tokenSupplyUiAmount ?? undefined,
      tokenDecimals: tokenDb.tokenDecimals ?? undefined,
      lastSupplyUpdate: tokenDb.lastSupplyUpdate ?? undefined,
    };
    return token;
  }
  return null;
}

std::future<MigrationStepResult> executeMigrationStep(TokenData token, MigrationStep step, MigrationStep nextStep, number = 3 retryCount, number = 2000 delay); for token ${token.mint}`);

  const result = await retryOperation(() => step.fn(token), retryCount, delay);

  // Update token migration
  token.migration = token.migration ?? {};
  (token.migration as Record<string, any>)[step.name] = {
    status: "success",
    txId: result.txId,
    updatedAt: new Date().toISOString(),
  };
  Object.assign(token, result.extraData);
  console.log(`${step.name} result:`, result);
  // Update the DB record
  const tokenData: Partial<TokenData> = {
    mint: token.mint,
    migration: token.migration,
    lastUpdated: new Date().toISOString(),
    status: "migrating",
    ...result.extraData,
  };
  const nextStepName = nextStep ? nextStep.name : null;
  token.migration.lastStep = nextStepName ?? "done";

  await updateTokenInDB(tokenData);
  // await saveMigrationState(token, step.name);

  const ws = getWebSocketClient();
  if (step.eventName) {
    ws.to(`token-${token.mint}`).emit(step.eventName, token);
  }

  logger.log(
    `[Migrate] ${step.name} successful for token ${token.mint} txId: ${result.txId}`,
  );
  return result;
}

std::future<bool> acquireMigrationLock(TokenData token);;
  if (migration.lock) {
    return false;
  }
  migration.lock = true;
  token.migration = migration;

  await updateTokenInDB({
    mint: token.mint,
    migration: token.migration,
    lastUpdated: new Date().toISOString(),
  });

  return true;
}
std::future<void> releaseMigrationLock(TokenData token);;
  if (migration.lock) {
    delete migration.lock;
  }
  token.migration = migration;

  await updateTokenInDB({
    mint: token.mint,
    migration: token.migration,
    lastUpdated: new Date().toISOString(),
  });
}
async ;

  await db
    .update(tokens)
    .set({
      migration: JSON.stringify(updatedMigration),
      lastUpdated: new Date(),
    })
    .where(eq(tokens.mint, token.mint));
}

async 
  }
  return null;
}

std::future<void> safeUpdateTokenInDB(const std::optional<TokenData>& data, auto retries = 3, auto delay = 2000);
      if (data.completedAt) {
        data.completedAt = new Date(data.completedAt)
      }
      if (data.migratedAt) {
        data.migratedAt = new Date(data.migratedAt)
      }

      await updateTokenInDB(data);
      return;
    } catch (err) {
      logger.error(
        `[DB] Failed to update token ${data.mint} on attempt ${attempt}:`,
        err
      );
      if (attempt === retries) throw err;
      await new Promise((r) => setTimeout(r, delay));
    }
  }
}

async 

    const wallet = Keypair.fromSecretKey(
      Uint8Array.from(JSON.parse(process.env.EXECUTOR_PRIVATE_KEY)),
    );
    const provider = new AnchorProvider(
      connection,
      new Wallet(wallet),
      AnchorProvider.defaultOptions(),
    );
    const program = new Program<RaydiumVault>(
      raydium_vault_IDL as any,
      provider,
    );
    const autofunProgram = new Program<Autofun>(idl, provider) as any;
    const redisCache = await getGlobalRedisCache();

    // const tokenMigrator = new TokenMigrator(
    //   connection,
    //   new Wallet(wallet),
    //   program,
    //   autofunProgram,
    //   provider,
    //   redisCache,
    // );

    // Filter out tokens that have migration as null or empty object or migration.status is not locked
    const filteredTokens = migratingTokens.filter((token) => {
      const migration = token.migration ? JSON.parse(token.migration) : null;
      return (
        migration &&
        (typeof migration === "object" || migration.status !== "locked")
      );
    });
    // Limit the number of tokens to the specified limit
    const finalList = filteredTokens.slice(0, limit);

    for (const token of finalList) {
      const tokenM = await getToken(token.mint);
    }
  } catch (error) {
    logger.error(`Error fetching migrating tokens: ${error}`);
    throw new Error("Failed to fetch migrating tokens");
  }
}

} // namespace elizaos
