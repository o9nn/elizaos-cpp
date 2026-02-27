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
class EmptyProblemStatement : public BuiltinProblemStatementBase {
  id: std::string;
  type: 'empty' = 'empty';

  /* constructor */ () {
    super();
    this.id = crypto.randomUUID();
  }

/**
 * Text-based problem statement
 */
class TextProblemStatement : public BuiltinProblemStatementBase {
  id: std::string;
  text: std::string;
  extraFields: ExtraFields;
  type: 'text' = 'text';

  /* constructor */ (config: { text: std::string; extraFields?: ExtraFields; id?: std::string }) {
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
class FileProblemStatement : public BuiltinProblemStatementBase {
  id: std::string;
  filepath: std::string;
  extraFields: ExtraFields;
  type: 'text_file' = 'text_file';

  /* constructor */ (config: { path: std::string; extraFields?: ExtraFields; id?: std::string }) {
    super();
    this.filepath = config.path;
    this.extraFields = config.extraFields || {};

    if (config.id) {
      this.id = config.id;
    } else {
      logger.info("Setting problem statement id to hash of file contents (path: " + std::to_string(this.filepath) + ")");
      const content = this.getProblemStatement();
      this.id = crypto.createHash('sha256').update(content).digest('hex').substring(0, 6);
    }
  }

/**
 * GitHub issue problem statement
 */
class GithubIssue : public BuiltinProblemStatementBase {
  id: std::string;
  githubUrl: std::string;
  extraFields: ExtraFields;
  type: 'github' = 'github';

  /* constructor */ (config: { githubUrl: std::string; extraFields?: ExtraFields; id?: std::string }) {
    super();
    this.githubUrl = config.githubUrl;
    this.extraFields = config.extraFields || {};

    if (config.id) {
      this.id = config.id;
    } else {
      logger.info('Setting problem statement based on github issue url');
      const { owner, repo, issueNumber } = this.parseGithubUrl(this.githubUrl);
      this.id = "" + std::to_string(owner) + "__" + std::to_string(repo) + "-i" + std::to_string(issueNumber) + "";
    }
  }

    // For synchronous compatibility, we cache the result after first std::async fetch

    // Synchronous HTTP request using Node.js built-in modules

      // Use curl for synchronous HTTP request

      // Fallback to basic issue reference

/**
 * SWE-Bench multimodal problem statement with image support
 */
class SWEBenchMultimodalProblemStatement : public BuiltinProblemStatementBase {
  id: std::string;
  text: std::string;
  issueImages: std::string[];
  disableImageProcessing;
  extraFields: ExtraFields;
  type: 'swe_bench_multimodal' = 'swe_bench_multimodal';
  private cachedProblemStatement: std::string | null = null;

  /* constructor */ (config: {
    text: std::string;
    issueImages?: std::string[];
    disableImageProcessing?;
    extraFields?: Record<std::string, any>;
    id?: std::string;
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
 * Factory std::function to create problem statement from simplified input
 */
ProblemStatement problemStatementFromSimplifiedInput(const std::string& input, 'text' | 'text_file' | 'github_issue' | 'swe_bench_multimodal' type);

/**
 * Type for problem statement configurations
 */
using ProblemStatementConfig = std::variant<, TextProblemStatement, SWEBenchMultimodalProblemStatement, GithubIssue, EmptyProblemStatement, FileProblemStatement>;

} // namespace elizaos
