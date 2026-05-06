#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace elizaos {

// NOTE: This is auto-generated approximate C++ code
// Manual refinement required for production use

/**
 * Type definitions
 * Converted from sweagent/types.py
 */

// CORE TYPE DEFINITIONS (moved here to be available for interfaces below)

/**
 * Tool call structure for LLM function calling
 */
struct ToolCall {
    std::optional<std::string> id;
    std::optional<'function'> type;
    std::string name;
};

/**
 * Query object in step output
 */
struct QueryObject {
    std::optional<std::string> type;
    std::optional<std::string> content;
};

/**
 * Thinking block structure
 */
struct ThinkingBlock {
    'thinking' type;
    std::string content;
    std::optional<double> startTime;
    std::optional<double> endTime;
};

/**
 * Cache control configuration
 */
struct CacheControl {
    std::variant<'ephemeral', 'persistent'> type;
    std::optional<double> maxAge;
    std::optional<std::variant<'user', 'global'>> scope;
};

/**
 * Output from a single agent step
 */
struct StepOutput {
    std::vector<QueryObject> query;
    std::string thought;
    std::string action;
    std::string output;
    std::string observation;
    double executionTime;
    bool done;
    std::optional<std::variant<double, std::string>> exitStatus;
    std::optional<std::optional<std::string>> submission;
    std::optional<std::optional<std::vector<ToolCall>>> toolCalls;
    std::optional<std::optional<std::vector<std::string>>> toolCallIds;
    std::optional<std::optional<std::vector<ThinkingBlock>>> thinkingBlocks;
};

/**
 * Implementation of StepOutput
 */
class StepOutputImpl {
public:
    std::variant<Record<string, string, double, bool, undefined>> toTemplateFormatDict();
};

/**
 * A single step in the agent's trajectory
 */
struct TrajectoryStep {
    std::string action;
    std::string observation;
    std::string response;
    std::string thought;
    double executionTime;
    std::vector<QueryObject> query;
};

/**
 * Complete trajectory (array of steps)
 */
using Trajectory = std::vector<TrajectoryStep>;

/**
 * Base history item interface
 */
struct BaseHistoryItem {
    std::string role;
    std::optional<std::string> text;
    std::optional<std::variant<'thought', 'action', 'observation', 'system', 'user', 'assistant', 'demonstration'>> messageType;
};

/**
 * Extended history item with optional fields
 */

/**
 * History is an array of history items
 */
using History = std::vector<HistoryItem>;

/**
 * Agent information dictionary
 */
struct AgentInfo {
    std::optional<std::optional<std::string>> exitStatus;
    std::optional<std::optional<std::string>> submission;
    std::optional<std::string> editedFiles30;
    std::optional<std::string> editedFiles50;
    std::optional<std::string> editedFiles70;
    std::optional<std::string> sweAgentHash;
    std::optional<std::string> sweAgentVersion;
    std::optional<std::string> sweRexVersion;
    std::optional<std::string> sweRexHash;
};

/**
 * Result from an agent run
 */
struct AgentRunResult {
    AgentInfo info;
    Trajectory trajectory;
};

// ADDITIONAL TYPE DEFINITIONS TO REPLACE 'any' TYPES

/**
 * Model response from LLM
 */
struct ModelResponse {
    std::string message;
    std::optional<std::vector<ToolCall>> toolCalls;
    std::optional<std::vector<ToolCall>> tool_calls;
    std::optional<std::string> role;
    std::optional<std::string> text;
};

/**
 * Environment variable configuration
 */
using EnvVariables = std::variant<Record<string, string, double, boolean>>;

/**
 * Registry variables configuration
 */
using RegistryVariables = std::variant<Record<string, string, double, boolean>>;

/**
 * Parsed command arguments
 */
struct ParsedArguments {
};

/**
 * Trajectory data for inspector
 */
struct TrajectoryData {
    std::optional<std::string> thought;
    std::optional<std::string> action;
    std::optional<std::string> observation;
    std::optional<std::string> response;
    std::optional<double> execution_time;
    std::optional<std::vector<HistoryItem>> history;
    std::optional<ReplayConfig> replay_config;
};

/**
 * Replay configuration
 */
struct ReplayConfig {
};

/**
 * Spinner task state
 */
struct SpinnerTask {
    std::string status;
    double startTime;
    std::optional<std::string> instanceId;
    std::optional<std::string> message;
};

/**
 * Instance statistics
 */
struct InstanceStats {
    std::optional<std::string> exitStatus;
    std::optional<std::string> result;
    std::optional<double> cost;
    std::optional<double> apiCalls;
};

/**
 * Command property for function calling tools
 */
struct CommandProperty {
    std::string type;
    std::string description;
    std::optional<std::vector<std::string>> enum;
};

/**
 * Run hook context
 */
struct RunContext {
    std::string outputDir;
    std::optional<double> instanceCount;
};

/**
 * Patch file info
 */
struct PatchInfo {
    std::string source;
    std::string target;
    double sourceStart;
    double sourceLines;
    double targetStart;
    double targetLines;
    std::vector<std::string> lines;
};

/**
 * File data type that can be JSON, YAML, or raw text
 */
using FileData = std::variant<std::unordered_map<std::string, unknown>, std::string, std::vector<unknown>>;

/**
 * GitHub API issue response
 */
struct GithubIssue {
    std::string title;
    std::optional<std::string> body;
    double number;
    std::string state;
    std::string created_at;
    std::string updated_at;
    std::string login;
    std::string avatar_url;
    std::string name;
    std::string color;
};

/**
 * GitHub API event response
 */
struct GithubEvent {
    std::string event;
    std::optional<std::string> commit_id;
    std::string created_at;
};

/**
 * Template context for rendering
 */
using TemplateContext = std::variant<Record<

/**
 * Serializable data structure
 */
using SerializableData = std::variant<, std::string, double, bool, nullptr, std::nullopt, std::vector<SerializableData>, std::any>;

/**
 * Command line argument value
 */
using ArgValue = std::variant<std::string, double, bool, std::vector<std::string>, std::nullopt>;

/**
 * Parsed command line arguments
 */
struct ParsedArgs {
};

/**
 * Error with code property
 */

} // namespace elizaos
