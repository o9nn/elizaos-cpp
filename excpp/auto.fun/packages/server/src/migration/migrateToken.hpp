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
    std::future<void> resetMigration(const std::string& mint);
    std::future<std::any> getMigrationState(const std::string& mint);
    std::future<void> resumeMigrationForToken(const std::string& mint, auto forced);
    std::future<void> forceResumeAtStep(const std::string& mint, const std::string& step);
    std::future<void> resumeMigrationsOnStart();
    std::future<void> printMigrationState(const std::string& mint);
    void callResumeWorker(TokenData token);
    std::vector<MigrationStep> getMigrationSteps();
    std::future<void> migrateToken(TokenData token);


} // namespace elizaos
