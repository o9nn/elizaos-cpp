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
 * Environment module exports
 */

// Main environment class
{ SWEEnv, EnvironmentConfig, EnvironmentConfigSchema } from './swe-env';

// Repository classes and types
{
  Repo,
  RepoConfig,
  RepoConfigSchema,
  PreExistingRepo,
  PreExistingRepoConfig,
  PreExistingRepoConfigSchema,
  LocalRepo,
  LocalRepoConfig,
  LocalRepoConfigSchema,
  GithubRepo,
  GithubRepoConfig,
  GithubRepoConfigSchema,
  repoFromSimplifiedInput,
} from './repo';

// Deployment classes and types
{
  AbstractDeployment,
  DockerDeployment,
  DockerDeploymentConfig,
  DockerDeploymentConfigSchema,
  DeploymentConfig,
  DeploymentConfigSchema,
} from './deployment';

// Runtime abstractions
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
} from './runtime';

// Hooks
{ EnvHook, CombinedEnvHooks, SetStatusEnvironmentHook, StatusCallback } from './hooks';

} // namespace elizaos
