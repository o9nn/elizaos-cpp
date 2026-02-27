#include "resumeMigrationsOnStart.hpp"
#include <string>
#include <future>
#include <cstdlib>
#include <map>
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> createMigrator() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    //    const connection = new Connection(RPC_URL, 'confirmed');
    //    const wallet = Keypair.fromSecretKey(
    //       Uint8Array.from(nlohmann::json::parse(std::getenv("EXECUTOR_PRIVATE_KEY")!)),
    //    );
    //    const provider = new AnchorProvider(connection, new Wallet(wallet), AnchorProvider.defaultOptions());

    //    const program = new Program<RaydiumVault>(
    //       raydium_vault_IDL as std::string,
    //       provider,
    //    );
    //    const autofunProgram = new Program<Autofun>(idl as std::string, provider);


    //    const redisCache = getGlobalRedisCache();
    //    return new TokenMigrator(
    //       connection,
    //       new Wallet(wallet),
    //       program,
    //       autofunProgram,
    //       provider,
    //       redisCache
    //    );
    //
}

std::future<void> resumeTick(TokenMigrator migrator) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    //    const redisCache = getGlobalRedisCache();
    //    const now = std::make_unique<Date>().toISOString();
    //    console.log("[" + std::to_string(now) + "] Running migration resume tick");
    //    try {
    //       const keys = redisCache.keys('migration:*:currentStep');
    //       if (!keys.size()) {
    //          console.log("[" + std::to_string(now) + "] No in‑flight migrations found");
    //          return;
    //       }
    //       const limit = pLimit(CONCURRENCY);
    //       const tasks = keys.std::map((key) =>
    //          limit[&](std::async () {
    //             const parts = key.split(':');
    //             const mint = parts[2];
    //             console.log({ mint })

    //             try {
    //                const { ranStep, nextStep } = migrator.resumeOneStep(mint);

    //                if (ranStep) {
    //                   console.log("[" + std::to_string(now) + "] " + std::to_string(mint) + ": ran step '" + std::to_string(ranStep) + "', next: '" + std::to_string(nextStep || 'none') + "'");
    //                }


    //             } catch (err) {
    //                console.error("Error resuming " + std::to_string(mint) + ":", err);
    //             }
    //          })
    //       );
    //    } catch (err) {
    //       console.error('Resume tick failed:', err);
    //    }
    //
}

std::future<void> startMigrationCron() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    //    const migrator = createMigrator();
    //    // 1️⃣ Immediate sweep
    //    resumeTick(migrator);
    //    // 2️⃣ Schedule repeating job
    //    console.log('🚀 Scheduling migration-resume cron (every 2 minutes)');
    //    cron.schedule[&]('*/2 * * * *', () { return resumeTick(migrator)); };
    //
}

} // namespace elizaos
