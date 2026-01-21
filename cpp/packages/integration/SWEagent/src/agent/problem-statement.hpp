#include ".utils/log.hpp"
#include "types.hpp"
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
 * Problem statement classes for defining agent tasks
 * Converted from sweagent/agent/problem_statement.py
 */

// Constants for image processing

/**
 * Interface for problem statements
 * Any class that implements this interface can be used as a problem statement
 */
struct ProblemStatement {
    std::string id;
};

/**
 * Base class for built-in problem statements
 */

/**
 * Empty problem statement
 */
class EmptyProblemStatement extends BuiltinProblemStatementBase {
  id: string;
  type: 'empty' = 'empty';

  constructor() {
    super();
    this.id = crypto.randomUUID();
  }

/**
 * Text-based problem statement
 */
class TextProblemStatement extends BuiltinProblemStatementBase {
  id: string;
  text: string;
  extraFields: ExtraFields;
  type: 'text' = 'text';

  constructor(config: { text: string; extraFields?: ExtraFields; id?: string }) {
    super();
    this.text = config.text;
    this.extraFields = config.extraFields || {};

    if (config.id) {
      this.id = config.id;
    } else {
      logger.info('Setting problem statement id to hash of text');
      this.id = crypto.createHash('sha256').update(this.text).digest('hex').substring(0, 6);
    }
  }

/**
 * File-based problem statement
 */
class FileProblemStatement extends BuiltinProblemStatementBase {
  id: string;
  filepath: string;
  extraFields: ExtraFields;
  type: 'text_file' = 'text_file';

  constructor(config: { path: string; extraFields?: ExtraFields; id?: string }) {
    super();
    this.filepath = config.path;
    this.extraFields = config.extraFields || {};

    if (config.id) {
      this.id = config.id;
    } else {
      logger.info(`Setting problem statement id to hash of file contents (path: ${this.filepath})`);
      const content = this.getProblemStatement();
      this.id = crypto.createHash('sha256').update(content).digest('hex').substring(0, 6);
    }
  }

/**
 * GitHub issue problem statement
 */
class GithubIssue extends BuiltinProblemStatementBase {
  id: string;
  githubUrl: string;
  extraFields: ExtraFields;
  type: 'github' = 'github';

  constructor(config: { githubUrl: string; extraFields?: ExtraFields; id?: string }) {
    super();
    this.githubUrl = config.githubUrl;
    this.extraFields = config.extraFields || {};

    if (config.id) {
      this.id = config.id;
    } else {
      logger.info('Setting problem statement based on github issue url');
      const { owner, repo, issueNumber } = this.parseGithubUrl(this.githubUrl);
      this.id = `${owner}__${repo}-i${issueNumber}`;
    }
  }

    // For synchronous compatibility, we cache the result after first async fetch

    // Synchronous HTTP request using Node.js built-in modules

      // Use curl for synchronous HTTP request

      // Fallback to basic issue reference

/**
 * SWE-Bench multimodal problem statement with image support
 */
class SWEBenchMultimodalProblemStatement extends BuiltinProblemStatementBase {
  id: string;
  text: string;
  issueImages: string[];
  disableImageProcessing: boolean;
  extraFields: ExtraFields;
  type: 'swe_bench_multimodal' = 'swe_bench_multimodal';
  private cachedProblemStatement: string | null = null;

  constructor(config: {
    text: string;
    issueImages?: string[];
    disableImageProcessing?: boolean;
    extraFields?: Record<string, any>;
    id?: string;
  }) {
    super();
    this.text = config.text;
    this.issueImages = config.issueImages || [];
    this.disableImageProcessing = config.disableImageProcessing || false;
    this.extraFields = config.extraFields || {};

    if (config.id) {
      this.id = config.id;
    } else {
      logger.info('Setting problem statement id to hash of text');
      this.id = crypto.createHash('sha256').update(this.text).digest('hex').substring(0, 6);
    }
  }

    // Return text without images for environment

    // For backwards compatibility, use the synchronous version
    // Tests can use getProblemStatementAsync() instead

    // Process images synchronously

    // Process images asynchronously

    // Only allow HTTP and HTTPS protocols

      // Download image using curl and get headers

      // Check content type

      // Download the actual image

/**
 * Factory function to create problem statement from simplified input
 */
ProblemStatement problemStatementFromSimplifiedInput(const std::string& input, 'text' | 'text_file' | 'github_issue' | 'swe_bench_multimodal' type);

/**
 * Type for problem statement configurations
 */
using ProblemStatementConfig = std::variant<, TextProblemStatement, SWEBenchMultimodalProblemStatement, GithubIssue, EmptyProblemStatement, FileProblemStatement>;

} // namespace elizaos
