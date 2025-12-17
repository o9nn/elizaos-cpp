#include "resumeMigrationsOnStart.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::future<void> createMigrator() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    //    const connection = new Connection(RPC_URL, 'confirmed');
    //    const wallet = Keypair.fromSecretKey(
    //       Uint8Array.from(JSON.parse(process.env.EXECUTOR_PRIVATE_KEY!)),
    //    );
    //    const provider = new AnchorProvider(connection, new Wallet(wallet), AnchorProvider.defaultOptions());

    //    const program = new Program<RaydiumVault>(
    //       raydium_vault_IDL as any,
    //       provider,
    //    );
    //    const autofunProgram = new Program<Autofun>(idl as any, provider);


    //    const redisCache = await getGlobalRedisCache();
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

    //    const redisCache = await getGlobalRedisCache();
    //    const now = new Date().toISOString();
    //    console.log(`[${now}] Running migration resume tick`);
    //    try {
    //       const keys = await redisCache.keys('migration:*:currentStep');
    //       if (!keys.length) {
    //          console.log(`[${now}] No in‑flight migrations found`);
    //          return;
    //       }
    //       const limit = pLimit(CONCURRENCY);
    //       const tasks = keys.map((key) =>
    //          limit(async () => {
    //             const parts = key.split(':');
    //             const mint = parts[2];
    //             console.log({ mint })

    //             try {
    //                const { ranStep, nextStep } = await migrator.resumeOneStep(mint);

    //                if (ranStep) {
    //                   console.log(`[${now}] ${mint}: ran step '${ranStep}', next: '${nextStep ?? 'none'}'`);
    //                }


    //             } catch (err) {
    //                console.error(`Error resuming ${mint}:`, err);
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

    //    const migrator = await createMigrator();
    //    // 1️⃣ Immediate sweep
    //    await resumeTick(migrator);
    //    // 2️⃣ Schedule repeating job
    //    console.log('🚀 Scheduling migration-resume cron (every 2 minutes)');
    //    cron.schedule('*/2 * * * *', () => resumeTick(migrator));
    //
}

} // namespace elizaos
