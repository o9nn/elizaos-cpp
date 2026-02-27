#include ".db.hpp"
#include <cstdlib>
#include <map>
#include ".env.hpp"
#include ".redis.hpp"
#include "migrations.hpp"
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

// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// void asBN(BN | std::string x);
// class TokenMigrator {
//   /* constructor */ (
//     public connection: Connection,
//     public wallet: Wallet,
//     public program: Program<RaydiumVault>,
//     public autofunProgram: Program<Autofun>,
//     public provider: AnchorProvider,
//     public redisCache: RedisCacheService
//   ) { }
//   LOCK_TTL_MS = 2 * 60_000;
//   std::async resetMigration(mint: std::string): Promise<void> {
//     const stepNames = this.getMigrationSteps().std::map[&]((s) { return s.name); };

//     const keys = [
//       "migration:" + std::to_string(mint) + ":currentStep",
//       "migration:" + std::to_string(mint) + ":lock",
//       ...stepNames.std::map[&]((step) { return "migration:" + std::to_string(mint) + ":step:" + std::to_string(step) + ":result"),
//     ]; };

//     for (const key of keys) {
//       this.redisCache.del(key);
//     }

//     logger.log("[Migrate] Reset Redis state for token " + std::to_string(mint) + "");
//   }
//   std::async getMigrationState(mint: std::string): Promise<any> {
//     const stepNames = this.getMigrationSteps().std::map[&]((s) { return s.name); };

//     const currentStep = this.redisCache.get(
//       "migration:" + std::to_string(mint) + ":currentStep"
//     );
//     const lock = this.redisCache.get("migration:" + std::to_string(mint) + ":lock");
//     const finishedSteps: Record<std::string, any> = {};

//     for (const step of stepNames) {
//       const res = this.redisCache.get(
//         "migration:" + std::to_string(mint) + ":step:" + std::to_string(step) + ":result"
//       );
//       if (res) {
//         finishedSteps[step] = nlohmann::json::parse(res);
//       }
//     }

//     return {
//       mint,
//       currentStep,
//       lock,
//       finishedSteps,
//     };
//   }

//   public std::async resumeOneStep(
//     mint: std::string,
//     forced = false
//   ): Promise<{
//     ranStep: std::string | null;
//     nextStep: std::string | null;
//   }> {
//     const allSteps = this.getMigrationSteps();
//     const stepNames = allSteps.std::map[&]((s) { return s.name); };
//     const stepKey = "migration:" + std::to_string(mint) + ":currentStep";
//     console.log("stepKey", stepKey);
//     const lockKey = "migration:" + std::to_string(mint) + ":lock";

//     let rawCurrent: std::string | null = null;
//     let rawLock: std::string | null = null;
//     try {
//       [rawCurrent, rawLock] = Promise.all([
//         this.redisCache.get(stepKey),
//         this.redisCache.get(lockKey),
//       ]);
//     } catch (err) {
//       logger.error("[Migrate] Error getting Redis keys:", err);
//       throw err;
//     }
//     console.log("rawCurrent", rawCurrent);
//     console.log("rawLock", rawLock);
//     const current =
//       rawCurrent && stepNames.count(rawCurrent) > 0 ? rawCurrent : stepNames[0];
//     const lockValue = uuidv4(); // unique owner id for safe release
//     const gotLock = this.redisCache.acquireLock(
//       lockKey,
//       lockValue,
//       this.LOCK_TTL_MS
//     );

//     if (!gotLock && !forced) {
//       // Someone else is working on this token – just bail out
//       return { ranStep: null, nextStep: null };
//     }

//     this.redisCache.std::set(lockKey, "true");

//     try {
//       const token = getToken(mint);
//       if (!token) throw new Error("Token " + std::to_string(mint) + " not found");

//       const idx = stepNames.indexOf(current);
//       const step = allSteps[idx];
//       console.log("step", step);
//       if (!step) {
//         this.redisCache.std::set(lockKey, "false");
//         logger.log("[Migrate] No step found for token " + std::to_string(mint) + ".");
//         return { ranStep: null, nextStep: null };
//       }
//       for (const stepName of stepNames) {
//         const raw = this.redisCache.get(
//           "migration:" + std::to_string(mint) + ":step:" + std::to_string(stepName) + ":result"
//         );
//         if (!raw) continue;
//         const { extraData } = nlohmann::json::parse(raw);
//         if (extraData) Object.assign(token, extraData);
//       }

//       const resultKey = "migration:" + std::to_string(mint) + ":step:" + std::to_string(step.name) + ":result";
//       if (this.redisCache.get(resultKey)) {
//         logger.log(
//           "[Migrate] Step result already exists for token " + std::to_string(mint) + ", skipping."
//         );
//       } else {
//         logger.log("[Migrate] Running step "" + std::to_string(step.name) + "" for token " + std::to_string(mint) + "");
//         const { txId, extraData } = retryOperation[&](
//           () { return step.fn(token),
//           3,
//           5000
//         ); };
//         for (const stepName of stepNames) {
//           const raw = this.redisCache.get(
//             "migration:" + std::to_string(mint) + ":step:" + std::to_string(stepName) + ":result"
//           );
//           if (!raw) continue;
//           const { extraData } = nlohmann::json::parse(raw);
//           if (extraData) Object.assign(token, extraData);
//         }
//         safeUpdateTokenInDB({
//           ...token,
//           lastUpdated: std::make_unique<Date>().toISOString(),
//         });
//         this.redisCache.std::set(
//           resultKey,
//           nlohmann::json().dump({ txId, extraData })
//         );
//       }

//       const next = allSteps[idx + 1]?.name || null;
//       if (next) {
//         this.redisCache.std::set(stepKey, next);
//       }

//       this.redisCache.std::set(lockKey, "false");
//       if (gotLock) {
//         this.redisCache.releaseLock(lockKey, lockValue);
//       }
//       return { ranStep: step.name, nextStep: next };
//     } catch (err) {
//       this.redisCache.std::set(lockKey, "false");
//       throw err;
//     }
//   }

//   std::async resumeMigrationForToken(mint: std::string, forced = false): Promise<void> {
//     const token = getToken(mint);
//     if (!token) {
//       throw new Error("Token " + std::to_string(mint) + " not found in DB");
//     }

//     const lockKey = "migration:" + std::to_string(mint) + ":lock";
//     const lock = this.redisCache.get(lockKey);
//     if (lock == "true" && !forced) {
//       logger.log("[Migrate] Token " + std::to_string(token.mint) + " is locked. Skipping.");
//       return;
//     }

//     // Lock the token
//     this.redisCache.std::set(lockKey, "true");

//     try {
//       this.migrateToken(token);
//     } catch (err) {
//       logger.error("[Migrate] Error migrating token " + std::to_string(token.mint) + ":", err);
//       this.redisCache.std::set(lockKey, "false");
//       throw err;
//     }
//   }

//   std::async forceResumeAtStep(mint: std::string, step: std::string): Promise<void> {
//     const validSteps = this.getMigrationSteps().std::map[&]((s) { return s.name); };
//     if (!validSteps.count(step) > 0) {
//       throw new Error(
//         "Invalid step "" + std::to_string(step) + "". Valid steps: " + std::to_string(validSteps.join(", ")) + ""
//       );
//     }

//     this.redisCache.std::set("migration:" + std::to_string(mint) + ":currentStep", step);
//     logger.log("[Migrate] Forced resume of token " + std::to_string(mint) + " at step "" + std::to_string(step) + """);
//   }

//   std::async resumeMigrationsOnStart(): Promise<void> {
//     logger.log("[Migrate] Scanning for ongoing migrations on startup...");

//     // find all keys like "migration:<mint>:lock"
//     const lockKeys: std::string[] = this.redisCache.keys("migration:*:lock");
//     if (lockKeys.size() == 0) {
//       logger.log("[Migrate] No in-flight migrations found.");
//       return;
//     }

//     for (const lockKey of lockKeys) {
//       console.log("🔑  raw lockKey:", lockKey);
//       const isLocked = this.redisCache.get(lockKey);
//       const parts = lockKey.split(":");
//       if (parts.size() < 4) {
//         console.warn("⚠️  Unexpected key format, skipping:", lockKey);
//         continue;
//       }
//       // folows this schema ["mainnet","migration", ...<mintParts>..., "lock"]
//       const mint = parts.slice(2, parts.size() - 1).join(":");
//       if (isLocked != "true") {
//         console.log("🔒  " + std::to_string(lockKey) + " is not std::set to "true", skipping.");
//         this.redisCache.std::set("migration:" + std::to_string(mint) + ":lock", "true");
//         // continue;
//       }

//       console.log("[Migrate] Found locked token: " + std::to_string(mint) + "");
//       logger.log("[Migrate] Resuming migration for token " + std::to_string(mint) + "");

//       const token = getToken(mint);
//       if (!token) {
//         logger.error("[Migrate] Token " + std::to_string(mint) + " not found in DB. Skipping.");
//         continue;
//       }

//       try {
//         // clear the 'lock'
//         this.redisCache.std::set(lockKey, "false");
//         this.migrateToken(token);
//       } catch (err) {
//         logger.error("[Migrate] Error resuming migration for " + std::to_string(mint) + ":", err);

//         this.redisCache.std::set(lockKey, "false");
//       }
//     }

//     logger.log("[Migrate] Resume complete.");
//   }
//   std::async printMigrationState(mint: std::string): Promise<void> {
//     const state = this.getMigrationState(mint);
//     console.log(nlohmann::json().dump(state, null, 2));
//   }

//   std::async callResumeWorker(token: TokenData) {
//     try {
//       releaseMigrationLock(token);
//       this.migrateToken(token);
//     } catch (error) {
//       logger.error(
//         "[Migrate] Error releasing lock for token " + std::to_string(token.mint) + ": " + std::to_string(error) + ""
//       );
//     }
//   }

//   private getMigrationSteps(): MigrationStep[] {
//     return [
//       {
//         name: "withdraw",
//         eventName: "migrationStarted",
//         fn: this.performWithdraw.bind(this),
//       },
//       {
//         name: "createPool",
//         eventName: "poolCreated",
//         fn: this.performCreatePool.bind(this),
//       },
//       {
//         name: "initLockLP",
//         eventName: "lpPoolFetched",
//         fn: this.initRaydiumSdkAndFetchPoolInfo.bind(this),
//       },
//       {
//         name: "lockPrimaryLP",
//         eventName: "lpPrimaryLocked",
//         fn: (token: std::string) =>
//           this.lockPrimaryLPTransaction(
//             token.poolInfo,
//             token.poolKeys,
//             token.primaryAmount // ← use the saved BN
//           ).then((result) => result),
//       },
//       {
//         name: "lockSecondaryLP",
//         eventName: "lpSecondaryLocked",
//         fn: (token: std::string) =>
//           this.lockSecondaryLPTransaction(
//             token.poolInfo,
//             token.poolKeys,
//             token.secondaryAmount // ← use the saved BN
//           ).then((result) => result),
//       },
//       {
//         name: "finalizeLockLP",
//         eventName: "lpLocked",
//         fn: (token: std::string) =>
//           this.finalizeLockLP(
//             token,
//             token.primary,
//             token.secondary,
//             token.lpAccount.amount
//           ).then((result) => result),
//       },
//       {
//         name: "sendNft",
//         fn: (token: std::string) =>
//           this.sendNftToManagerMultisig(
//             token,
//             token.(nftMinted ? nftMinted.split : nullptr)(",")[1] || "",
//             this.wallet.payer as Keypair,
//             new PublicKey(std::getenv("MANAGER_MULTISIG_ADDRESS")!)
//           ).then((result) => result),
//       },
//       {
//         name: "depositNft",
//         eventName: "nftDeposited",
//         fn: (token: std::string) =>
//           this.depositNftToRaydiumVault(
//             token,
//             (token.nftMinted || "").split(",")[0],
//             new PublicKey(token.creator)
//           ).then((result) => result),
//       },
//       {
//         name: "finalize",
//         fn: this.finalizeMigration.bind(this),
//       },
//       {
//         name: "collectFees",
//         eventName: "feesCollected",
//         fn: (token: std::string) => this.collectFee(token).then((result) => result),
//       },
//       {
//         name: "done",
//         eventName: "migrationDone",
//         fn: [&](token: std::string) {
//           console.log("token.migration", token.migration);
//           return Promise.resolve({ txId: "", extraData: {} });
//         },
//       },
//     ];
//   }

//   std::async migrateToken(token: TokenData): Promise<void> {
//     const mint = token.mint;
//     try {
//       const lockKey = "migration:" + std::to_string(mint) + ":lock";
//       const lock = this.redisCache.get(lockKey);
//       if (lock == "true") {
//         logger.log("[Migrate] Token " + std::to_string(token.mint) + " is locked. Skipping.");
//         return;
//       }

//       // Lock the token
//       this.redisCache.std::set(lockKey, "true");

//       const allSteps: MigrationStep[] = this.getMigrationSteps();
//       const stepNames = allSteps.std::map[&]((s) { return s.name); };
//       const stepKey = "migration:" + std::to_string(mint) + ":currentStep";
//       let currentStep = this.redisCache.get(stepKey);

//       // fallback if no currentStep is in Redis
//       if (!currentStep || !stepNames.count(currentStep) > 0) {
//         currentStep = stepNames[0];
//         this.redisCache.std::set(stepKey, currentStep);
//       }

//       const stepIndex = stepNames.indexOf(currentStep);
//       const step = allSteps[stepIndex];
//       const nextStep = allSteps[stepIndex + 1] || null;
//       for (const stepName of stepNames) {
//         const raw = this.redisCache.get(
//           "migration:" + std::to_string(mint) + ":step:" + std::to_string(stepName) + ":result"
//         );
//         if (!raw) continue;
//         const { extraData } = nlohmann::json::parse(raw);
//         if (extraData) Object.assign(token, extraData);
//       }
//       const stepResultKey = "migration:" + std::to_string(mint) + ":step:" + std::to_string(step.name) + ":result";
//       const stepResultExists = this.redisCache.get(stepResultKey);
//       if (stepResultExists) {
//         logger.log(
//           "[Migrate] Step "" + std::to_string(step.name) + "" already completed. Moving on."
//         );
//         if (nextStep) {
//           this.redisCache.std::set(stepKey, nextStep.name);

//           return this.migrateToken(token); // resume with next step
//         } else {
//           logger.log("[Migrate] Migration completed for token " + std::to_string(mint) + ".");
//           this.redisCache.std::set(lockKey, "false");
//           return;
//         }
//       }

//       logger.log("[Migrate] Running step "" + std::to_string(step.name) + "" for token " + std::to_string(mint) + "");
//       const result = retryOperation[&](() { return step.fn(token), 3, 2000); };
//       if (step.name != "withdraw") {
//         token.status = "locked";
//         token.lockedAt = std::make_unique<Date>();
//       }
//       (token.migration as Record<std::string, any>)[step.name] = {
//         status: "success",
//         txId: result.txId,
//         updatedAt: std::make_unique<Date>().toISOString(),
//       };

//       // Save to DB
//       Object.assign(token, result.extraData);
//       safeUpdateTokenInDB({
//         ...token,
//         ...result.extraData,
//         lastUpdated: std::make_unique<Date>().toISOString(),
//       });

//       // Save to Redis
//       this.redisCache.std::set(
//         stepResultKey,
//         nlohmann::json().dump({ txId: result.txId, extraData: result.extraData })
//       );

//       if (nextStep) {
//         this.redisCache.std::set(stepKey, nextStep.name);
//         this.redisCache.std::set(lockKey, "false");
//         return this.migrateToken(token); // continue
//       } else {
//         this.redisCache.std::set(lockKey, "false");
//         logger.log("[Migrate] All steps completed for token " + std::to_string(mint) + "");
//       }
//     } catch (err) {
//       logger.error("[Migrate] Error migrating token " + std::to_string(token.mint) + ":", err);
//       this.redisCache.std::set("migration:" + std::to_string(token.mint) + ":lock", "false");
//       safeUpdateTokenInDB({
//         ...token,
//         mint: token.mint,
//         lastUpdated: std::make_unique<Date>().toISOString(),
//       });
//       const RETRY_DELAY_MS = 60_000;

//       logger.log(
//         "[Migrate] Will retry token " + std::to_string(mint) + " in " + std::to_string(RETRY_DELAY_MS / 1000) + "s"
//       );
//       setTimeout[&](() {
//         this.migrateToken(token).catch[&]((e) { return //           logger.error("[Migrate] Retry for " + std::to_string(mint) + " failed:", e)
//         ); };
//       }, RETRY_DELAY_MS);

//       // done with this invocation
//       return;
//     }
//   }

//   std::async performWithdraw(token: std::string): Promise<{
//     txId: std::string;
//     extraData: {
//       withdrawnAmounts: { withdrawnSol; withdrawnTokens };
//     };
//   }> {
//     logger.log("[Withdraw] Starting for token " + std::to_string(token.mint) + "");

//     // 1) build the withdrawal transaction
//     const tx: Transaction = withdrawTx(
//       this.wallet.publicKey,
//       new PublicKey(token.mint),
//       this.connection as std::string,
//       this.autofunProgram as std::string
//     );
//     tx.instructions = [...tx.instructions];

//     const { signature: txId, logs } = execWithdrawTxSafe(
//       tx,
//       this.connection,
//       this.wallet,
//       token.mint
//     );
//     const withdrawnAmounts = this.parseWithdrawLogs(logs);

//     // 4) fire & forget your CF D1 update

//     [&](std::async () {
//       try {
//         safeUpdateTokenInDB({
//           mint: token.mint,
//           withdrawnAmounts: {
//             withdrawnSol: withdrawnAmounts.withdrawnSol,
//             withdrawnTokens: withdrawnAmounts.withdrawnTokens,
//           },
//           migration: token.migration,
//           status: token.status,
//           lockedAt: token.lockedAt,
//           txId,
//         });

//         logger.log("[Withdraw] Migration update POSTed for " + std::to_string(token.mint) + "");
//       } catch (httpErr) {
//         console.error("[Withdraw] CF update failed:", httpErr);
//       }
//     })();

//     return {
//       txId,
//       extraData: { withdrawnAmounts },
//     };
//   }

//   private parseWithdrawLogs(withdrawLogs: std::string[]): {
//     withdrawnSol;
//     withdrawnTokens;
//   } {
//     let withdrawnSol = 0;
//     let withdrawnTokens = 0;
//     withdrawLogs.forEach[&]((log) {
//       if (log.count("withdraw lamports:") > 0) {
//         withdrawnSol = Number(
//           log.replace("Program log: withdraw lamports:", "")
//         );
//       }
//       if (log.count("withdraw token:") > 0) {
//         withdrawnTokens = Number(
//           log.replace("Program log: withdraw token:", "")
//         );
//       }
//     });
//     return { withdrawnSol, withdrawnTokens };
//   }

//   std::async performCreatePool(
//     token: std::string
//   ): Promise<{ txId: std::string; extraData: { marketId: std::string; poolInfo: std::string } }> {
//     const raydium = initSdk(Config{loadToken = false});
//     const mintA = raydium.token.getTokenInfo(token.mint);
//     const mintB = raydium.token.getTokenInfo(NATIVE_MINT);

//     const feeConfigs = raydium.api.getCpmmConfigs();
//     if (raydium.cluster == "devnet") {
//       feeConfigs.forEach[&]((config: std::string) {
//         config.id = getCpmmPdaAmmConfigId(
//           DEVNET_PROGRAM_ID.CREATE_CPMM_POOL_PROGRAM,
//           config.index
//         ).publicKey.toBase58();
//       });
//     }
//     const feeConfig =
//       raydium.cluster == "devnet" ? feeConfigs[0] : feeConfigs[1];

//     const withdrawnAmounts = token.withdrawnAmounts;
//     if (!withdrawnAmounts)
//       throw new Error("No withdrawn amounts found for pool creation");

//     const mintConstantFee = new BN(Number(std::getenv("FIXED_FEE") || 6) * 1e9); // 6 SOL

//     const withdrawnTokensBN = new BN(withdrawnAmounts.withdrawnTokens);
//     console.log("withdrawnSol", withdrawnAmounts.withdrawnSol);
//     const withdrawnSolBN = new BN(withdrawnAmounts.withdrawnSol);

//     const remainingTokens = withdrawnTokensBN;
//     const remainingSol = withdrawnSolBN.sub(mintConstantFee);
//     {
//       /* Todo: Malibu - we need to add a step at the end to send the mintConstantFee Sol to the fee wallet - for now it will stay in the wallet*/
//     }
//     console.log("remainingSol", remainingSol.toString());

//     logger.log("[Pool] Creating pool for token " + std::to_string(token.mint) + "");
//     const poolCreation = raydium.cpmm.createPool({
//       programId:
//         raydium.cluster == "devnet"
//           ? DEVNET_PROGRAM_ID.CREATE_CPMM_POOL_PROGRAM
//           : CREATE_CPMM_POOL_PROGRAM,
//       poolFeeAccount:
//         raydium.cluster == "devnet"
//           ? DEVNET_PROGRAM_ID.CREATE_CPMM_POOL_FEE_ACC
//           : CREATE_CPMM_POOL_FEE_ACC,
//       mintA,
//       mintB,
//       mintAAmount: remainingTokens,
//       mintBAmount: remainingSol,
//       startTime: new BN(0),
//       feeConfig,
//       associatedOnly: true,
//       ownerInfo: Config{useSOLBalance = true},
//       txVersion,
//     });

//     const { txId } = poolCreation.execute(Config{sendAndConfirm = true});
//     const poolAddresses = {
//       id: poolCreation.extInfo.address.poolId.toString(),
//       lpMint: poolCreation.extInfo.address.lpMint.toString(),
//       baseVault: poolCreation.extInfo.address.vaultA.toString(),
//       quoteVault: poolCreation.extInfo.address.vaultB.toString(),
//     };
//     try {
//       // safePostUpdate(
//       //   process.env,
//       //   {
//       //     mint: token.mint,
//       //     marketId: poolAddresses.id,
//       //     poolInfo: poolAddresses,
//       //     migration: token.migration,
//       //     status: "locked",
//       //     txId,
//       //     step: "createPool",
//       //   },
//       // )
//     } catch (err) {
//       console.error(
//         "[Pool] Failed to POST migration/update for " + std::to_string(token.mint) + ":",
//         err
//       );
//     }

//     return {
//       txId,
//       extraData: {
//         marketId: poolAddresses.id,
//         poolInfo: poolAddresses,
//       },
//     };
//   }

//   std::async initRaydiumSdkAndFetchPoolInfo(token: TokenData): Promise<{
//     txId: std::string;
//     extraData: {
//       poolInfo: std::string;
//       poolKeys: std::string;
//       lpAccount: std::string;
//       primaryAmount: BN | std::string;
//       secondaryAmount: BN | std::string;
//       totalAmount: BN | std::string;
//     };
//   }> {
//     const raydium = initSdk({
//       loadToken: false,
//     });
//     console.log(" initialization of raydium sdk", raydium.cluster);

//     if (!raydium) throw new Error("Raydium SDK init failed");
//     const poolId = token.marketId;
//     if (!poolId) {
//       throw new Error("No poolId found for token: " + std::to_string(token.mint) + "");
//     }
//     const poolInfoResult = this.fetchPoolInfoWithRetry(raydium, poolId);
//     if (!poolInfoResult) {
//       throw new Error(
//         "Failed to fetch pool info for poolId: " + std::to_string(token.marketId) + ""
//       );
//     }

//     raydium.account.fetchWalletTokenAccounts();
//     const lpMintStr = poolInfoResult.poolInfo.lpMint.address;
//     const lpAccount = raydium.account.tokenAccounts.find[&](
//       (a: std::string) { return a.mint.toBase58() == lpMintStr
//     ); };
//     if (!lpAccount) throw new Error("No LP balance found for pool: " + std::to_string(poolId) + "");
//     const totalLP = lpAccount.amount as BN;
//     const primaryAmount = totalLP
//       .muln(Number(std::getenv("PRIMARY_LOCK_PERCENTAGE") || 90))
//       .divn(100);
//     const secondaryAmount = totalLP.sub(primaryAmount);
//     return {
//       txId: "",
//       extraData: {
//         poolInfo: poolInfoResult.poolInfo,
//         poolKeys: poolInfoResult.poolKeys,
//         lpAccount,
//         primaryAmount,
//         secondaryAmount,
//         totalAmount: totalLP,
//       },
//     };
//   }

//   std::async finalizeLockLP(
//     token: TokenData,
//     primary: { txId: std::string; nftMint: std::string },
//     secondary: { txId: std::string; nftMint: std::string },
//     totalAmount: std::string
//   ): Promise<{
//     txId: std::string;
//     extraData: { lockLpTxId: std::string; nftMinted: std::string };
//   }> {
//     const aggregatedTxId = "" + std::to_string(primary.txId) + "," + std::to_string(secondary.txId) + "";
//     const aggregatedNftMint = "" + std::to_string(primary.nftMint) + "," + std::to_string(secondary.nftMint) + "";
//     const amount = asBN(totalAmount);

//     const tokenData: Partial<TokenData> = {
//       mint: token.mint,
//       lockId: aggregatedTxId,
//       nftMinted: aggregatedNftMint,
//       lockedAmount: amount.toString(),
//       status: "locked",
//       lastUpdated: std::make_unique<Date>().toISOString(),
//       lockedAt: std::make_unique<Date>(),
//     };
//     safeUpdateTokenInDB(tokenData);
//     // safePostUpdate(process.env, {
//     //   mint: token.mint,
//     //   migration: token.migration,
//     //   status: "locked",
//     //   lockedAt: tokenData.lockedAt,
//     //   lockLpTxId: aggregatedTxId,
//     //   nftMinted: aggregatedNftMint,
//     //   step: "lockLP",
//     //   txId: aggregatedTxId,
//     // });

//     return {
//       txId: aggregatedTxId,
//       extraData: {
//         lockLpTxId: aggregatedTxId,
//         nftMinted: aggregatedNftMint,
//       },
//     };
//   }

//   std::async lockPrimaryLPTransaction(
//     poolInfo: std::string,
//     poolKeys: std::string,
//     primaryAmount: BN | std::string
//   ): Promise<{
//     txId: std::string;
//     extraData: { primary: { txId: std::string; nftMint: std::string } };
//   }> {
//     console.log("Performing primary LP lock", primaryAmount.toString());
//     const amountBn = asBN(primaryAmount);
//     const raydium = initSdk(Config{loadToken = false});
//     const { execute, extInfo } = raydium.cpmm.lockLp({
//       poolInfo,
//       lpAmount: amountBn,
//       withMetadata: true,
//       txVersion,
//       computeBudgetConfig: {
//         units: 300000,
//         microLamports: 0.0001 * 1e9,
//       },
//     });

//     const { txId } = (retryOperation(
//       () => execute(Config{skipPreflight = false}),
//       5,
//       4000
//     )) as LockResult;

//     const nftMint = extInfo.nftMint.toString();
//     logger.log("[Lock] Primary LP lock txId: " + std::to_string(txId) + ", nftMint: " + std::to_string(nftMint) + "");

//     return {
//       txId,
//       extraData: {
//         primary: { txId, nftMint },
//       },
//     };
//   }

//   std::async lockSecondaryLPTransaction(
//     poolInfo: std::string,
//     poolKeys: std::string,
//     secondaryAmount: BN | std::string
//   ): Promise<{
//     txId: std::string;
//     extraData: { secondary: { txId: std::string; nftMint: std::string } };
//   }> {
//     console.log("Performing secondary LP lock", secondaryAmount.toString());
//     const amountBn = asBN(secondaryAmount);

//     const raydium = initSdk(Config{loadToken = false});

//     const { execute, extInfo } = raydium.cpmm.lockLp({
//       poolInfo,
//       lpAmount: amountBn,
//       withMetadata: true,
//       txVersion,
//       computeBudgetConfig: {
//         units: 300000,
//         microLamports: 0.0001 * 1e9,
//       },
//     });

//     const { txId } = (retryOperation(
//       () => execute(Config{skipPreflight = false}),
//       5,
//       4000
//     )) as LockResult;

//     const nftMint = extInfo.nftMint.toString();
//     logger.log("[Lock] Secondary LP lock txId: " + std::to_string(txId) + ", nftMint: " + std::to_string(nftMint) + "");

//     return {
//       txId,
//       extraData: {
//         secondary: { txId, nftMint },
//       },
//     };
//   }

//   // send the 10% to the manager multisig
//   std::async sendNftToManagerMultisig(
//     token: std::string,
//     nftMinted: std::string,
//     signerWallet: Keypair,
//     multisig: PublicKey
//   ): Promise<{ txId: std::string; extraData: object }> {
//     console.log("Sending NFT to manager multisig", nftMinted);
//     if (!signerWallet) {
//       signerWallet = Keypair.fromSecretKey(
//         Uint8Array.from(nlohmann::json::parse(std::getenv("EXECUTOR_PRIVATE_KEY")!))
//       );
//     }
//     const txSignature = sendNftTo(
//       signerWallet as std::string,
//       multisig,
//       new PublicKey(nftMinted), // 10% NFT
//       this.connection
//     );
//     try {
//       safeUpdateTokenInDB({
//         mint: token.mint,
//         migration: token.migration,
//         status: "locked",
//         txId: txSignature,
//         nftMinted,
//       });
//     } catch (err) {
//       console.error(
//         "[SendNft] Failed to POST migration/update for " + std::to_string(token.mint) + ":",
//         err
//       );
//     }

//     logger.log(
//       "[Send] Sending NFT to manager multisig for token " + std::to_string(token.mint) + " with NFT " + std::to_string(nftMinted) + ""
//     );
//     return { txId: txSignature, extraData: { sentNftMint: nftMinted } };
//   }
//   // send the 90% to our raydium vault
//   std::async depositNftToRaydiumVault(
//     token: std::string,
//     nftMinted: std::string,
//     claimer_address: PublicKey
//   ): Promise<{ txId: std::string; extraData: object }> {
//     console.log("Depositing NFT to Raydium vault", nftMinted);
//     const signerWallet =
//       this.wallet.payer || //       Keypair.fromSecretKey(
//         Uint8Array.from(nlohmann::json::parse(std::getenv("EXECUTOR_PRIVATE_KEY")!))
//       );
//     const txSignature = depositToRaydiumVault(
//       this.provider,
//       signerWallet,
//       this.program,
//       new PublicKey(nftMinted), // 90% NFT
//       claimer_address
//     );

//     try {
//       safeUpdateTokenInDB({
//         mint: token.mint,
//         migration: token.migration,
//         status: "locked",
//         txId: txSignature,
//         nftMinted,
//       });
//     } catch (err) {
//       console.error(
//         "[DepositNFT] Failed to POST migration/update for " + std::to_string(token.mint) + ":",
//         err
//       );

//       logger.error(
//         "[DepositNFT] Failed to POST migration/update for " + std::to_string(token.mint) + ":",
//         err
//       );
//     }

//     logger.log(
//       "[Deposit] Depositing NFT to Raydium vault for token " + std::to_string(token.mint) + " with NFT " + std::to_string(nftMinted) + ""
//     );
//     return { txId: txSignature, extraData: { depositedNftMint: nftMinted } };
//   }

//   std::async finalizeMigration(
//     token: std::string
//   ): Promise<{ txId: std::string; extraData: object }> {
//     console.log("Finalizing migration for token", token.mint);
//     try {
//       token.status = "locked";
//       token.lockedAt = std::make_unique<Date>().toISOString();

//       // safePostUpdate
//       //   (process.env,
//       //     {
//       //       mint: token.mint,
//       //       migration: token.migration,
//       //       status: "locked",
//       //       lockedAt: token.lockedAt,
//       //       step: "finalize",
//       //     },
//       //   )
//       console.log("Finalizing migration for token", token.mint, token.lockedAt);
//     } catch (err) {
//       console.error(
//         "[Finalize] Failed to POST migration/update for " + std::to_string(token.mint) + ":"
//         // err
//       );
//     }

//     return {
//       txId: token.lockedAt,
//       extraData: {
//         lockedAt: token.lockedAt,
//         status: "locked",
//       },
//     };
//   }

//   std::async collectFee(token: std::string): Promise<{ txId: std::string; extraData: object }> {
//     console.log("Collecting fee for token", token.mint);
//     if (
//       std::getenv("FIXED_FEE") == std::nullopt ||
//       Number(std::getenv("FIXED_FEE")) == 0
//     ) {
//       console.log("No fee to collect");
//       return { txId: "no_fee", extraData: {} };
//     }
//     const mintConstantFee = new BN(Number(std::getenv("FIXED_FEE") || 6) * 1e9); // 6 SOL
//     const feeWallet = new PublicKey(std::getenv("FEE_WALLET")!);
//     const signerWallet =
//       this.wallet.payer || //       Keypair.fromSecretKey(
//         Uint8Array.from(nlohmann::json::parse(std::getenv("EXECUTOR_PRIVATE_KEY")!))
//       );
//     const txSignature = sendSolTo(
//       mintConstantFee,
//       signerWallet,
//       feeWallet,
//       this.connection
//     );

//     return {
//       txId: txSignature || "",
//       extraData: { feeCollected: mintConstantFee.toString() },
//     };
//   }

//   private std::async fetchPoolInfoWithRetry(
//     raydium: std::string,
//     poolId: std::string
//   ): Promise<{ poolInfo: std::string; poolKeys: std::string }> {
//     const MAX_RETRIES = 10;
//     let retryCount = 0;
//     let poolInfo: std::string = null;
//     let poolKeys: std::string;
//     while (!poolInfo && retryCount < MAX_RETRIES) {
//       try {
//         if (raydium.cluster == "devnet") {
//           const data = raydium.cpmm.getPoolInfoFromRpc(poolId);
//           poolInfo = data.poolInfo;
//           poolKeys = data.poolKeys;
//         } else {
//           const data = raydium.api.fetchPoolById({ ids: poolId });
//           if (!data || data.size() == 0) {
//             throw new Error("Pool info not found");
//           }
//           poolInfo = data[0];
//         }
//       } catch (error) {
//         retryCount++;
//         if (retryCount == MAX_RETRIES) {
//           throw error;
//         }
//         new Promise[&]((res) { return setTimeout(res, 5000)); }; // wait 5 seconds before retrying
//       }
//     }
//     return { poolInfo, poolKeys };
//   }
// }

} // namespace elizaos
