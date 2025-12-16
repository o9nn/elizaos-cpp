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
 * Main exports for the agent module
 */

// Core agent classes
{
  AbstractAgent,
  DefaultAgent,
  AgentConfig,
  DefaultAgentConfig,
  RetryAgentConfig,
  ShellAgentConfig,
  TemplateConfig,
  ToolConfig,
  ToolHandler,
  getAgentFromConfig,
} from './agents';

// Model classes
{
  AbstractModel,
  HumanModel,
  HumanThoughtModel,
  LiteLLMModel,
  ReplayModel,
  InstantEmptySubmitModel,
  GlobalStats,
  InstanceStats,
  RetryConfig,
  GenericAPIModelConfig,
  getModel,
} from './models';

// Hook classes
{ AbstractAgentHook, CombinedAgentHook, SetStatusAgentHook } from './hooks';

// History processors
{
  AbstractHistoryProcessor,
  DefaultHistoryProcessor,
  LastNObservations,
  TagToolCallObservations,
  ClosedWindowHistoryProcessor,
  CacheControlHistoryProcessor,
  RemoveRegex,
  ImageParsingHistoryProcessor,
  createHistoryProcessor,
} from './history-processors';

// Problem statement classes
{
  ProblemStatement,
  EmptyProblemStatement,
  TextProblemStatement,
  FileProblemStatement,
  GithubIssue,
  SWEBenchMultimodalProblemStatement,
  ProblemStatementConfig,
  problemStatementFromSimplifiedInput,
} from './problem-statement';

// Action sampler classes
{
  AbstractActionSampler,
  ActionSamplerOutput,
  AskColleagues,
  AskColleaguesConfig,
  BinaryTrajectoryComparison,
  BinaryTrajectoryComparisonConfig,
  ActionSamplerConfig,
  createActionSampler,
} from './action-sampler';

// Reviewer and retry loop classes
{
  ReviewSubmission,
  ReviewSubmissionImpl,
  ReviewerResult,
  PreselectorOutput,
  ChooserOutput,
  AbstractReviewer,
  AbstractRetryLoop,
  TrajFormatterConfig,
  ReviewerConfig,
  ChooserConfig,
  PreselectorConfig,
  ChooserRetryLoopConfig,
  ScoreRetryLoopConfig,
  RetryLoopConfig,
  TrajectoryFormatter,
  Reviewer,
  Chooser,
  ChooserRetryLoop,
  ScoreRetryLoop,
  getRetryLoopFromConfig,
} from './reviewer';

// Extra agent implementations
{ ShellAgent } from './extra';

} // namespace elizaos
