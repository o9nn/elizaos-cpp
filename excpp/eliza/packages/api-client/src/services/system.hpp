#pragma once
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
    *   GET /api/system/env/local  -> route(auto packages/server/src/api/system);
    std::future<std::unordered_map<std::string, std::string>> getEnvironment();
    *   POST /api/system/env/local  -> route(auto packages/server/src/api/system);
    Promise< updateLocalEnvironment(const std::variant<, LocalEnvironmentUpdateParams, { content: Record<string>& params, auto string> }
    void if(auto 'variables' in params);
    void if(auto 'content' in params);
    void if(auto !response.ok);
    std::string buildUrl(const std::string& path, std::optional<{ params: Record<string> options, auto any> });
    std::unordered_map<std::string, std::string> getHeaders();
    std::future<void> deleteLog(const std::string& logId);


} // namespace elizaos
