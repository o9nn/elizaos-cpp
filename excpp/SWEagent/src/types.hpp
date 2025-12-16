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

// ============================================================================
// CORE TYPE DEFINITIONS (moved here to be available for interfaces below)
// ============================================================================

/**
 * Tool call structure for LLM ;


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
    'ephemeral' | 'persistent' type;
    std::optional<double> maxAge;
    std::optional<'user' | 'global'> scope;
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
    std::optional<number | string | null> exitStatus;
    std::optional<string | null> submission;
    std::unordered_map<std::string, std::string> state;
    std::optional<ToolCall[] | null> toolCalls;
    std::optional<string[] | null> toolCallIds;
    std::optional<ThinkingBlock[] | null> thinkingBlocks;
    std::unordered_map<std::string, unknown> extraInfo;
};


/**
 * Implementation of StepOutput
 */
class StepOutputImpl implements StepOutput {
  query: QueryObject[] = [{}];
  thought: string = '';
  action: string = '';
  output: string = '';
  observation: string = '';
  executionTime: number = 0.0;
  done: boolean = false;
  exitStatus?: number | string | null = null;
  submission?: string | null = null;
  state: Record<string, string> = {};
  toolCalls?: ToolCall[] | null = null;
  toolCallIds?: string[] | null = null;
  thinkingBlocks?: ThinkingBlock[] | null = null;
  extraInfo: Record<string, unknown> = {};

  toTemplateFormatDict(): Record<string, string | number | boolean | null | undefined> {
    return {
      thought: this.thought,
      action: this.action,
      output: this.output,
      observation: this.observation,
      executionTime: this.executionTime,
      done: this.done,
      exitStatus: this.exitStatus,
      submission: this.submission,
    };
  }
}

/**
 * A single step in the agent's trajectory
 */
struct TrajectoryStep {
    std::string action;
    std::string observation;
    std::string response;
    std::unordered_map<std::string, std::string> state;
    std::string thought;
    double executionTime;
    std::vector<QueryObject> query;
    std::unordered_map<std::string, unknown> extraInfo;
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
    string | Array<{ type: string; text?: string; [key: string]: unknown }> content;
    std::optional<'thought' | 'action' | 'observation' | 'system' | 'user' | 'assistant' | 'demonstration'> messageType;
};


/**
 * Extended history item with optional fields
 */
interface HistoryItem extends BaseHistoryItem {
  agent?: string;
  isDemo?: boolean;
  thought?: string;
  action?: string | null;
  toolCalls?: ToolCall[] | null;
  toolCallIds?: string[] | null;
  tags?: string[];
  cacheControl?: CacheControl | null;
  thinkingBlocks?: ThinkingBlock[] | null;
}

/**
 * History is an array of history items
 */
using History = std::vector<HistoryItem>;

/**
 * Agent information dictionary
 */
struct AgentInfo {
    std::optional<std::unordered_map<std::string, double>> modelStats;
    std::optional<string | null> exitStatus;
    std::optional<string | null> submission;
    std::optional<std::unordered_map<std::string, unknown>> review;
    std::optional<std::string> editedFiles30;
    std::optional<std::string> editedFiles50;
    std::optional<std::string> editedFiles70;
    std::optional<std::unordered_map<std::string, unknown>> summarizer;
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


// ============================================================================
// ADDITIONAL TYPE DEFINITIONS TO REPLACE 'any' TYPES
// ============================================================================

/**
 * Model response from LLM
 */
struct ModelResponse {
    std::string message;
    std::optional<std::vector<ToolCall>> toolCalls;
    std::optional<std::vector<ToolCall>> tool_calls;
    std::optional<std::string> role;
    std::optional<string | Array<{ type: string; text?: string; [key: string]: unknown }>> content;
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
    Array<{ trajectory;
    std::optional<std::string> thought;
    std::optional<std::string> action;
    std::optional<std::string> observation;
    std::optional<std::string> response;
    std::optional<double> execution_time;
    std::optional<std::unordered_map<std::string, std::string>> state;
    std::unordered_map<std::string, unknown> info;
    std::optional<std::vector<HistoryItem>> history;
    std::optional<ReplayConfig> replay_config;
};


/**
 * Replay configuration
 */
struct ReplayConfig {
    std::optional<std::unordered_map<std::string, unknown>> environment;
    std::optional<std::unordered_map<std::string, unknown>> agent;
    std::optional<std::unordered_map<std::string, unknown>> tools;
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
    string | null exitStatus;
    std::optional<std::string> result;
    std::optional<double> cost;
    std::optional<double> apiCalls;
};


/**
 * Command property for ;


/**
 * Run hook context
 */
struct RunContext {
    std::string outputDir;
    std::optional<std::unordered_map<std::string, unknown>> config;
    std::optional<double> instanceCount;
};


/**
 * Patch file info
 */
struct PatchInfo {
    std::string source;
    std::string target;
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
using FileData = std::variant<std::unordered_map<std::string, unknown>, std::string, std::vector<unknown>>;

/**
 * GitHub API issue response
 */
struct GithubIssue {
    std::string title;
    string | null body;
    double number;
    std::string state;
    std::string created_at;
    std::string updated_at;
    { user;
    std::string login;
    std::string avatar_url;
    Array<{ labels;
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
  string,
  string, double, bool, std::vector<unknown>, std::unordered_map<std::string, unknown>, nullptr, undefined
>>;

/**
 * Serializable data structure
 */
using SerializableData = std::variant<, std::string, double, bool, nullptr, std::nullopt, std::vector<SerializableData>, { [key: string]: SerializableData }>;

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
interface CodedError extends Error {
  code?: string;
  status?: number;
  stdout?: string;
  stderr?: string;
}

} // namespace elizaos
