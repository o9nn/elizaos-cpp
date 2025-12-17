#pragma once
#include <any>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".db.hpp"
#include ".env.hpp"
#include ".redis.hpp"
#include "migrations.hpp"

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
// void asBN(const std::variant<BN, std::string>& x);
// class TokenMigrator {
// //     public redisCache);
    std::future<void> resetMigration(const std::string& mint);
    std::future<std::any> getMigrationState(const std::string& mint);
//);
    void catch(auto err);
    void if(auto !gotLock && !forced);
    void if(auto !step);
    void for(auto const stepName of stepNames);
    void for(auto const stepName of stepNames);
    void if(auto next);
    void if(auto gotLock);
    void catch(auto err);
    std::future<void> resumeMigrationForToken(const std::string& mint, auto forced = false);
    std::future<void> forceResumeAtStep(const std::string& mint, const std::string& step);
    std::future<void> resumeMigrationsOnStart();
    std::future<void> printMigrationState(const std::string& mint);
    void callResumeWorker(TokenData token);
    std::vector<MigrationStep> getMigrationSteps();
    std::future<void> migrateToken(TokenData token);
    void catch(auto httpErr);
// //     token);
    void catch(auto err);
    void if(auto !poolId);
    void if(auto !poolInfoResult);
// //     totalAmount);
//>& //     primaryAmount);
//>& //     secondaryAmount);
// //     multisig);
    void if(auto !signerWallet);
    void catch(auto err);
// //     claimer_address);
    void catch(auto err);
// //     token);
    void catch(auto err);
// //     poolId);
    void while(auto !poolInfo && retryCount < MAX_RETRIES);


} // namespace elizaos
