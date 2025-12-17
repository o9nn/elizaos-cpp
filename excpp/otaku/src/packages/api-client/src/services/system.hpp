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
#include ".lib/base-client.hpp"
#include ".types/system.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



class SystemService {
public:
    *   GET /api/system/env/local  -> route();
    std::future<std::unordered_map<std::string, std::string>> getEnvironment();
    Promise< updateLocalEnvironment(const std::variant<, LocalEnvironmentUpdateParams, std::any, std::unordered_map<std::string, std::string>>& params);
    Promise< getGlobalLogs(std::optional<std::any> params);
    std::string buildUrl(const std::string& path, std::optional<std::any> options);
    std::unordered_map<std::string, std::string> getHeaders();
    Promise< deleteGlobalLogs();
    std::future<void> deleteLog(const std::string& logId);
};


} // namespace elizaos
