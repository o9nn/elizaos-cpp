#pragma once
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include "lib/base-client.hpp"

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use



/**
 * Query parameters for session messages API
 */
struct SessionMessageQueryParams {
    std::optional<std::string> limit;
    std::optional<std::string> before;
    std::optional<std::string> after;
};

/**
 * Validates and converts a date parameter to timestamp string
 * @param value Date, string, or number to convert
 * @param paramName Name of the parameter for error messages
 * @returns Timestamp string or undefined if value is invalid
 */
std::string toTimestampString(const std::variant<std::chrono::system_clock::time_point, std::string, double>& value, const std::string& paramName);

/**
 * Validates required parameters
 * @param value Parameter value to validate
 * @param paramName Name of the parameter for error messages
 * @throws Error if the parameter is invalid
 */

/**
 * Service for managing messaging sessions between users and agents
 */
class SessionsService {
public:
    std::future<SessionsHealthResponse> checkHealth();
    std::future<CreateSessionResponse> createSession(CreateSessionParams params);
    std::future<SessionInfoResponse> getSession(const std::string& sessionId);
    std::future<MessageResponse> sendMessage(const std::string& sessionId, SendMessageParams params);
    std::future<GetMessagesResponse> getMessages(const std::string& sessionId, std::optional<GetMessagesParams> params);
    Promise< deleteSession(const std::string& sessionId);
    std::future<ListSessionsResponse> listSessions();
};


} // namespace elizaos
