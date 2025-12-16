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
 * Main exports TypeScript implementation
 */

// Core types
* from './types';
* from './exceptions';

// Agent module - selective exports to avoid conflicts
{
  AbstractAgent,
  DefaultAgent,
  RetryAgent,
  AgentConfig,
  DefaultAgentConfig,
  RetryAgentConfig,
  ShellAgentConfig,
  TemplateConfig,
  getAgentFromConfig,
} from './agent/agents';

{
  AbstractModel,
  HumanModel,
  LiteLLMModel,
  ReplayModel,
  InstantEmptySubmitModel,
  HumanThoughtModel,
  ModelConfig,
  GenericAPIModelConfig,
  HumanModelConfig,
  ReplayModelConfig,
  GlobalStats,
  InstanceStats,
  getModel,
} from './agent/models';

{
  ProblemStatement,
  ProblemStatementConfig,
  EmptyProblemStatement,
  TextProblemStatement,
  FileProblemStatement,
  GithubIssue,
  SWEBenchMultimodalProblemStatement,
  problemStatementFromSimplifiedInput,
} from './agent/problem-statement';

{
  AbstractReviewer,
  AbstractRetryLoop,
  ReviewSubmission,
  ReviewerResult,
  PreselectorOutput,
  ChooserOutput,
  Reviewer,
  Chooser,
  ChooserRetryLoop,
  ScoreRetryLoop,
  ReviewerConfig,
  ChooserConfig,
  PreselectorConfig,
  ChooserRetryLoopConfig,
  ScoreRetryLoopConfig,
  RetryLoopConfig,
  getRetryLoopFromConfig,
} from './agent/reviewer';

{
  AbstractActionSampler,
  ActionSamplerOutput,
  AskColleagues,
  AskColleaguesConfig,
  BinaryTrajectoryComparison,
  BinaryTrajectoryComparisonConfig,
  ActionSamplerConfig,
  createActionSampler,
} from './agent/action-sampler';

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
} from './agent/history-processors';

// Environment module
{ SWEEnv, EnvironmentConfig } from './environment/swe-env';

{
  Repo,
  PreExistingRepo,
  LocalRepo,
  GithubRepo,
  PreExistingRepoConfig,
  LocalRepoConfig,
  GithubRepoConfig,
  RepoConfig,
  repoFromSimplifiedInput,
} from './environment/repo';

{
  AbstractDeployment,
  DockerDeployment,
  DockerDeploymentConfig,
  DeploymentConfig,
  getDeployment,
} from './environment/deployment';

{
  AbstractRuntime,
  BashAction,
  BashActionResult,
  BashInterruptAction,
  CreateBashSessionRequest,
  Command,
  CommandResult,
  ReadFileRequest,
  ReadFileResponse,
  WriteFileRequest,
  UploadRequest,
} from './environment/runtime';

// Utils module - selective exports
{ getLogger, AgentLogger, setThreadName, getThreadName, setLogLevel, log } from './utils/log';

{
  convertPathRelativeToRepoRoot,
  convertPathToAbspath,
  convertPathsToAbspath,
  loadEnvironmentVariables,
  stripAbspathFromDict,
  couldBeAPath,
} from './utils/config';

{ loadFile } from './utils/files';

{
  isGithubRepoUrl,
  isGithubIssueUrl,
  parseGhIssueUrl,
  parseGhRepoUrl,
  getGhIssueData,
  getProblemStatementFromGithubIssue,
  getAssociatedCommitUrls,
  InvalidGithubURL,
} from './utils/github';

{ warnProbablyWrongJinjaSyntax } from './utils/jinja-warnings';

{ PatchFormatter } from './utils/patch-formatter';

{ convertToYamlLiteralString, yamlSerializationWithLinebreaks, mergeNestedDicts } from './utils/serialization';

// Tools module - selective exports
{ Bundle, BundleConfig } from './tools/bundle';

{ Command as ToolCommand, Argument } from './tools/commands';

{
  AbstractParseFunction,
  ActionParser,
  ActionOnlyParser,
  ThoughtActionParser,
  XMLThoughtActionParser,
  FunctionCallingParser,
  JsonParser,
  IdentityParser,
  ParseFunction,
  createParser,
} from './tools/parsing';

{ ToolHandler, ToolConfig, ToolFilterConfig, defaultToolConfig, defaultToolFilterConfig } from './tools/tools';

{ guardMultilineInput, shouldQuote, getSignature, generateCommandDocs } from './tools/utils';

// Run module - selective exports
{
  RunSingle,
  RunSingleConfig,
  RunSingleActionConfig,
  runFromConfig as runSingleFromConfig,
} from './run/run-single';

{ RunBatch, RunBatchConfig, runBatchFromConfig } from './run/run-batch';

{ RunReplay, RunReplayConfig, runReplayFromConfig } from './run/run-replay';

{ RunShell } from './run/run-shell';

{
  BatchInstance,
  SimpleBatchInstance,
  AbstractInstanceSource,
  InstancesFromFile,
  SWEBenchInstances,
  BatchInstanceSourceConfig,
  createInstanceSource,
  filterBatchItems,
} from './run/batch-instances';

{ RunHook, AbstractRunHook, CombinedRunHooks } from './run/hooks/types';
{ SaveApplyPatchHook } from './run/hooks/apply-patch';
{ OpenPRHook } from './run/hooks/open-pr';
{ SweBenchEvaluate } from './run/hooks/swe-bench-evaluate';

{
  shortenString,
  shortenStrings,
  savePredictions,
  isPromisingPatch,
  createNestedDict,
  parseArgsToNestedDict,
} from './run/common';

{ run } from './run/run';

// Version info
const VERSION = '1.0.0';

/**
 * Get agent commit hash
 */


/**
 * Get REX commit hash
 */


/**
 * Get REX version
 */


/**
 * Get agent version info
 */
`;
}

} // namespace elizaos
