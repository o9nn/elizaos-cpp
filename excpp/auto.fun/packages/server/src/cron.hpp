#pragma once
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "chart.hpp"
#include "db.hpp"
#include "externalToken.hpp"
#include "generation.hpp"
#include "mcap.hpp"
#include "migration/migrations.hpp"
#include "points.hpp"
#include "redis.hpp"
#include "tokenSupplyHelpers.hpp"
#include "util.hpp"
#include "websocket-client.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



 // S3 Import

 // Buffer import
 // Import crypto for lock value

std::optional<Token> sanitizeTokenForWebSocket(const std::optional<Token>& token, auto maxBytes);
std::optional<TokenDBData> convertTokenDataToDBData(const std::optional<TokenData>& tokenData);

std::future<Token> updateTokenInDB(const std::optional<TokenData>& tokenData);
using ProcessResult = {

using HandlerResult = std::variant<ProcessResult, nullptr>;
std::future<ProcessResult> processTransactionLogs(const std::vector<std::string>& logs, const std::string& signature, std::optional<WebSocketClient> wsClient);

std::future<HandlerResult> handleNewToken(const std::vector<std::string>& logs, const std::string& signature, WebSocketClient wsClient);

std::future<HandlerResult> handleCurveComplete(const std::vector<std::string>& logs, const std::string& signature, WebSocketClient wsClient);

// Renamed to be the primary for cron tasks
std::future<void> runCronTasks();

} // namespace elizaos
