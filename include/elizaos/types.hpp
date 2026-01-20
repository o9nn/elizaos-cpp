#pragma once
/**
 * @file types.hpp
 * @brief Common type definitions for ElizaOS C++ implementation
 * 
 * This header provides fundamental types used across the ElizaOS framework,
 * including basic data structures, enumerations, and type aliases.
 */

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace elizaos {

// ============================================================================
// Forward Declarations
// ============================================================================

class Agent;
class Memory;
class Action;
class Plugin;
class Runtime;

// ============================================================================
// Type Aliases
// ============================================================================

using UUID = std::string;
using Timestamp = std::chrono::system_clock::time_point;
using JsonValue = std::variant<std::nullptr_t, bool, int64_t, double, std::string, 
                                std::vector<struct JsonNode>, std::map<std::string, struct JsonNode>>;

struct JsonNode {
    JsonValue value;
    
    JsonNode() : value(nullptr) {}
    JsonNode(std::nullptr_t) : value(nullptr) {}
    JsonNode(bool b) : value(b) {}
    JsonNode(int64_t i) : value(i) {}
    JsonNode(int i) : value(static_cast<int64_t>(i)) {}
    JsonNode(double d) : value(d) {}
    JsonNode(const std::string& s) : value(s) {}
    JsonNode(const char* s) : value(std::string(s)) {}
    JsonNode(std::vector<JsonNode> arr) : value(std::move(arr)) {}
    JsonNode(std::map<std::string, JsonNode> obj) : value(std::move(obj)) {}
};

using Json = JsonNode;
using Content = std::unordered_map<std::string, JsonNode>;
using Metadata = std::unordered_map<std::string, std::string>;
using Headers = std::unordered_map<std::string, std::string>;
using QueryParams = std::unordered_map<std::string, std::string>;
using Callback = std::function<void()>;
using ErrorCallback = std::function<void(const std::string&)>;

// ============================================================================
// Enumerations
// ============================================================================

/**
 * @brief Log levels for the logging system
 */
enum class LogLevel {
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR = 4,
    FATAL = 5
};

/**
 * @brief Agent states
 */
enum class AgentState {
    IDLE,
    THINKING,
    ACTING,
    WAITING,
    ERROR,
    STOPPED
};

/**
 * @brief Memory types
 */
enum class MemoryType {
    SHORT_TERM,
    LONG_TERM,
    EPISODIC,
    SEMANTIC,
    PROCEDURAL
};

/**
 * @brief Action result status
 */
enum class ActionStatus {
    SUCCESS,
    FAILURE,
    PENDING,
    CANCELLED,
    TIMEOUT
};

/**
 * @brief Message roles in conversations
 */
enum class MessageRole {
    SYSTEM,
    USER,
    ASSISTANT,
    FUNCTION,
    TOOL
};

/**
 * @brief Token status for trading/token systems
 */
enum class TokenStatus {
    ACTIVE,
    INACTIVE,
    PENDING,
    GRADUATED,
    FAILED,
    UNKNOWN
};

// Type alias for backward compatibility
using TTokenStatus = TokenStatus;

/**
 * @brief HTTP methods
 */
enum class HttpMethod {
    GET,
    POST,
    PUT,
    DELETE_,
    PATCH,
    HEAD,
    OPTIONS
};

/**
 * @brief WebSocket connection states
 */
enum class WebSocketState {
    CONNECTING,
    OPEN,
    CLOSING,
    CLOSED
};

// ============================================================================
// Core Structures
// ============================================================================

/**
 * @brief Represents a message in a conversation
 */
struct Message {
    UUID id;
    MessageRole role;
    std::string content;
    std::optional<std::string> name;
    std::optional<Timestamp> timestamp;
    Metadata metadata;
    
    Message() = default;
    Message(MessageRole r, const std::string& c) : role(r), content(c) {}
};

/**
 * @brief Represents a memory entry
 */
struct MemoryEntry {
    UUID id;
    MemoryType type;
    std::string content;
    std::vector<float> embedding;
    Timestamp created_at;
    Timestamp updated_at;
    double importance;
    Metadata metadata;
};

/**
 * @brief Represents an action definition
 */
struct ActionDefinition {
    std::string name;
    std::string description;
    std::vector<std::string> parameters;
    std::function<ActionStatus(const Content&)> handler;
};

/**
 * @brief Represents an action result
 */
struct ActionResult {
    ActionStatus status;
    std::optional<std::string> output;
    std::optional<std::string> error;
    Metadata metadata;
};

/**
 * @brief Configuration for an agent
 */
struct AgentConfig {
    std::string name;
    std::string model;
    std::string system_prompt;
    double temperature = 0.7;
    int max_tokens = 2048;
    std::vector<std::string> plugins;
    Metadata settings;
};

/**
 * @brief User information
 */
struct User {
    UUID id;
    std::string username;
    std::optional<std::string> email;
    std::optional<std::string> display_name;
    std::optional<std::string> avatar_url;
    Timestamp created_at;
    Metadata metadata;
};

/**
 * @brief Token/cryptocurrency information
 */
struct Token {
    std::string id;
    std::string name;
    std::string symbol;
    std::string mint;
    std::optional<std::string> creator;
    TokenStatus status;
    double price;
    double market_cap;
    double volume_24h;
    Timestamp created_at;
    Metadata metadata;
};

// Alias for compatibility
using IToken = Token;

/**
 * @brief Social media links
 */
struct SocialLinks {
    std::optional<std::string> twitter;
    std::optional<std::string> discord;
    std::optional<std::string> telegram;
    std::optional<std::string> website;
    std::optional<std::string> github;
};

/**
 * @brief Transaction information
 */
struct Transaction {
    UUID id;
    std::string type;
    std::string from_address;
    std::string to_address;
    double amount;
    std::optional<std::string> token_mint;
    std::string signature;
    Timestamp timestamp;
    std::string status;
};

/**
 * @brief Media generation request/response
 */
struct MediaGeneration {
    std::string id;
    std::string mint;
    std::string type;
    std::string prompt;
    std::string media_url;
    std::optional<std::string> negative_prompt;
    std::optional<int> num_inference_steps;
    std::optional<int> seed;
    std::optional<int> num_frames;
    std::optional<int> fps;
    std::optional<int> motion_bucket_id;
    std::optional<double> duration;
    std::optional<double> duration_seconds;
    std::optional<int> bpm;
    std::optional<std::string> creator;
    Timestamp timestamp;
};

/**
 * @brief HTTP request structure
 */
struct HttpRequest {
    HttpMethod method;
    std::string url;
    Headers headers;
    std::optional<std::string> body;
    QueryParams params;
    int timeout_ms = 30000;
};

/**
 * @brief HTTP response structure
 */
struct HttpResponse {
    int status_code;
    Headers headers;
    std::string body;
    std::optional<std::string> error;
};

/**
 * @brief WebSocket message
 */
struct WebSocketMessage {
    std::string data;
    bool is_binary;
    Timestamp timestamp;
};

/**
 * @brief Event structure for pub/sub systems
 */
struct Event {
    std::string type;
    std::string source;
    Content data;
    Timestamp timestamp;
    std::optional<UUID> correlation_id;
};

/**
 * @brief Plugin metadata
 */
struct PluginInfo {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::vector<std::string> dependencies;
    bool enabled;
};

/**
 * @brief Error information
 */
struct Error {
    std::string code;
    std::string message;
    std::optional<std::string> details;
    std::optional<std::string> stack_trace;
    
    Error() = default;
    Error(const std::string& c, const std::string& m) : code(c), message(m) {}
};

/**
 * @brief Result type for operations that can fail
 */
template<typename T>
struct Result {
    std::optional<T> value;
    std::optional<Error> error;
    
    bool is_ok() const { return value.has_value(); }
    bool is_err() const { return error.has_value(); }
    
    static Result<T> ok(T val) {
        Result<T> r;
        r.value = std::move(val);
        return r;
    }
    
    static Result<T> err(Error e) {
        Result<T> r;
        r.error = std::move(e);
        return r;
    }
};

/**
 * @brief Pagination information
 */
struct Pagination {
    int page = 1;
    int per_page = 20;
    int total_items = 0;
    int total_pages = 0;
};

/**
 * @brief Paginated response wrapper
 */
template<typename T>
struct PaginatedResponse {
    std::vector<T> items;
    Pagination pagination;
};

// ============================================================================
// Utility Functions
// ============================================================================

/**
 * @brief Convert LogLevel to string
 */
inline std::string logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::TRACE: return "TRACE";
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARN: return "WARN";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

/**
 * @brief Convert string to LogLevel
 */
inline LogLevel stringToLogLevel(const std::string& str) {
    if (str == "TRACE") return LogLevel::TRACE;
    if (str == "DEBUG") return LogLevel::DEBUG;
    if (str == "INFO") return LogLevel::INFO;
    if (str == "WARN") return LogLevel::WARN;
    if (str == "ERROR") return LogLevel::ERROR;
    if (str == "FATAL") return LogLevel::FATAL;
    return LogLevel::INFO;
}

/**
 * @brief Convert TokenStatus to string
 */
inline std::string tokenStatusToString(TokenStatus status) {
    switch (status) {
        case TokenStatus::ACTIVE: return "active";
        case TokenStatus::INACTIVE: return "inactive";
        case TokenStatus::PENDING: return "pending";
        case TokenStatus::GRADUATED: return "graduated";
        case TokenStatus::FAILED: return "failed";
        default: return "unknown";
    }
}

} // namespace elizaos
