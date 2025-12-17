#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include ".cron.hpp"
#include ".db.hpp"
#include ".redis.hpp"
#include ".tokenSupplyHelpers.hpp"
#include ".tokenSupplyHelpers/customWallet.hpp"
#include ".util.hpp"
#include ".websocket-client.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



struct LockResult {
    std::string txId;
};

struct MigrationStepResult {
    std::string txId;
};

using MigrationStepFn = std::function<std::future<MigrationStepResult>(TokenData, auto)>;

struct MigrationStep {
    std::string name;
    std::optional<std::string> description;
    MigrationStepFn fn;
    std::optional<std::string> eventName;
};

std::future<MigrationStepResult> executeMigrationStep(TokenData token, MigrationStep step, MigrationStep nextStep, number = 3 retryCount, number = 2000 delay);

std::future<bool> acquireMigrationLock(TokenData token);
std::future<void> releaseMigrationLock(TokenData token);
std::future<void> saveMigrationState(TokenData token, const std::string& step);

std::future<void> getMigrationState(TokenData token);

std::future<void> safeUpdateTokenInDB(const std::optional<TokenData>& data, auto retries = 3, auto delay = 2000);

std::future<void> checkMigratingTokens(double limit);

} // namespace elizaos
