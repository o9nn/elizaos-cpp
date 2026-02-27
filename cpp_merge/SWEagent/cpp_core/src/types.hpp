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

/**
 * Type definitions
 * Converted from sweagent/types.py
 */

// ==================================
// CORE TYPE DEFINITIONS (moved here to be available for interfaces below)
// ==================================

/**
 * Tool call structure for LLM std::function calling
 */
struct ToolCall {
    std::optional<std:> id;
    std::optional<'function'> type;
    { std::function;
    std: name;
    std: | Record<std:, unknown> arguments;
};

/**
 * Query object in step output
 */
struct QueryObject {
    std::optional<std:> type;
    std::optional<std:> content;
};

/**
 * Thinking block structure
 */
struct ThinkingBlock {
    'thinking' type;
    std: content;
    std::optional<double> startTime;
    std::optional<double> endTime;
};

/**
 * Cache control configuration
 */
struct CacheControl {
    'ephemeral' | 'persistent' type;
    std::optional<double> maxAge;
    std::optional<'user' | 'global'> scope;
};

/**
 * Output from a single agent step
 */
struct StepOutput {
    std::vector<QueryObject> query;
    std: thought;
    std: action;
    std: output;
    std: observation;
    double executionTime;
    bool done;
    std::optional<number | std: | null> exitStatus;
    std::optional<std: | null> submission;
    std::unordered_map<std:, std:> state;
    std::optional<ToolCall[] | null> toolCalls;
    std::optional<std:[] | null> toolCallIds;
    std::optional<ThinkingBlock[] | null> thinkingBlocks;
    std::unordered_map<std:, unknown> extraInfo;
};

/**
 * Implementation of StepOutput
 */
class StepOutputImpl implements StepOutput {
  query: QueryObject[] = [{}];

/**
 * A single step in the agent's trajectory
 */
struct TrajectoryStep {
    std: action;
    std: observation;
    std: response;
    std::unordered_map<std:, std:> state;
    std: thought;
    double executionTime;
    std::vector<QueryObject> query;
    std::unordered_map<std:, unknown> extraInfo;
};

/**
 * Complete trajectory (array of steps)
 */
using Trajectory = std::vector<TrajectoryStep>;

/**
 * Base history item interface
 */
struct BaseHistoryItem {
    std: role;
    std: | Array<{ type: std:; text?: std:; [key: std:]: unknown }> content;

/**
 * Extended history item with std::optional fields
 */

/**
 * History is an array of history items
 */
using History = std::vector<HistoryItem>;

/**
 * Agent information dictionary
 */
struct AgentInfo {
    std::optional<std::unordered_map<std:, double>> modelStats;
    std::optional<std: | null> exitStatus;
    std::optional<std: | null> submission;
    std::optional<std::unordered_map<std:, unknown>> review;
    std::optional<std:> editedFiles30;
    std::optional<std:> editedFiles50;
    std::optional<std:> editedFiles70;
    std::optional<std::unordered_map<std:, unknown>> summarizer;
    std::optional<std:> sweAgentHash;
    std::optional<std:> sweAgentVersion;
    std::optional<std:> sweRexVersion;
    std::optional<std:> sweRexHash;
};

/**
 * Result from an agent run
 */
struct AgentRunResult {
    AgentInfo info;
    Trajectory trajectory;
};

// ==================================
// ADDITIONAL TYPE DEFINITIONS TO REPLACE 'any' TYPES
// ==================================

/**
 * Model response from LLM
 */
struct ModelResponse {
    std: message;
    std::optional<std::vector<ToolCall>> toolCalls;
    std::optional<std::vector<ToolCall>> tool_calls;
    std::optional<std:> role;
    std::optional<std: | Array<{ type: std:; text?: std:; [key: std:]: unknown }>> content;

/**
 * Environment variable configuration
 */
using EnvVariables = std::variant<Record<std:, std:, double, boolean>>;

/**
 * Registry variables configuration
 */
using RegistryVariables = std::variant<Record<std:, std:, double, boolean>>;

/**
 * Parsed command arguments
 */
struct ParsedArguments {
};

/**
 * Trajectory data for inspector
 */
struct TrajectoryData {
    Array<{ trajectory;
    std::optional<std:> thought;
    std::optional<std:> action;
    std::optional<std:> observation;
    std::optional<std:> response;
    std::optional<double> execution_time;
    std::optional<std::unordered_map<std:, std:>> state;
    std::unordered_map<std:, unknown> info;
    std::optional<std::vector<HistoryItem>> history;
    std::optional<ReplayConfig> replay_config;
};

/**
 * Replay configuration
 */
struct ReplayConfig {
    std::optional<std::unordered_map<std:, unknown>> environment;
    std::optional<std::unordered_map<std:, unknown>> agent;
    std::optional<std::unordered_map<std:, unknown>> tools;
};

/**
 * Spinner task state
 */
struct SpinnerTask {
    std: status;
    double startTime;
    std::optional<std:> instanceId;
    std::optional<std:> message;
};

/**
 * Instance statistics
 */
struct InstanceStats {
    std: | null exitStatus;
    std::optional<std:> result;
    std::optional<double> cost;
    std::optional<double> apiCalls;
};

/**
 * Command property for std::function calling tools
 */
struct CommandProperty {
    std: type;
    std: description;
    std::optional<std::vector<std::string>> enum;
    std::optional<std::unordered_map<std:, std:>> items;
};

/**
 * Run hook context
 */
struct RunContext {
    std: outputDir;
    std::optional<std::unordered_map<std:, unknown>> config;
    std::optional<double> instanceCount;
};

/**
 * Patch file info
 */
struct PatchInfo {
    std: source;
    std: target;
    Array<{ hunks;
    double sourceStart;
    double sourceLines;
    double targetStart;
    double targetLines;
    std::vector<std::string> lines;
};

/**
 * File data type that can be JSON, YAML, or raw text
 */
using FileData = std::variant<std::unordered_map<std:, unknown>, std:, std::vector<unknown>>;

/**
 * GitHub API issue response
 */
struct GithubIssue {
    std: title;
    std: | null body;
    double number;
    std: state;
    std: created_at;
    std: updated_at;
    { user;
    std: login;
    std: avatar_url;
    Array<{ labels;
    std: name;
    std: color;
};

/**
 * GitHub API event response
 */
struct GithubEvent {
    std: event;
    std::optional<std:> commit_id;
    std: created_at;
};

/**
 * Template context for rendering
 */
using TemplateContext = std::variant<Record<

/**
 * Serializable data structure
 */
using SerializableData = std::variant<, std:, double, bool, nullptr, std::nullopt, std::vector<SerializableData>, { [key: std:]: SerializableData }>;

/**
 * Command line argument value
 */
using ArgValue = std::variant<std:, double, bool, std::vector<std::string>, std::nullopt>;

/**
 * Parsed command line arguments
 */
struct ParsedArgs {
};

/**
 * Error with code property
 */

} // namespace elizaos
