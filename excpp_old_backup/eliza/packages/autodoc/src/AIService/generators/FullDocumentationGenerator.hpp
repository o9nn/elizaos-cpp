#include "..TypeScriptParser.js.hpp"
#include "..utils/prompts.hpp"
#include ".AIService.js.hpp"
#include ".utils/CodeFormatter.js.hpp"
#include ".utils/DocumentOrganizer.js.hpp"
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
 * Interface representing a Frequently Asked Question (FAQ) with a question and its corresponding answer.
 * @typedef {Object} FAQ
 * @property {string} question - The question being asked.
 * @property {string} answer - The answer to the question.
 */
struct FAQ {
    std::string question;
    std::string answer;
};

/**
 * Interface representing a troubleshooting issue.
 * @typedef {object} TroubleshootingIssue
 * @property {string} issue - The description of the issue.
 * @property {string} cause - The cause of the issue.
 * @property {string} solution - The solution to the issue.
 */
struct TroubleshootingIssue {
    std::string issue;
    std::string cause;
    std::string solution;
};

/**
 * Interface representing Troubleshooting information.
 * @interface
 */
struct Troubleshooting {
    std::vector<TroubleshootingIssue> commonIssues;
    std::vector<std::string> debuggingTips;
};

/**
 * Class representing a FullDocumentationGenerator.
 * @class
 */

class FullDocumentationGenerator {
  private typeScriptParser: TypeScriptParser;
  private codeFormatter: CodeFormatter;
  private documentOrganizer: DocumentOrganizer;
  private aiService: AIService;

  /**
   * Constructor for initializing the ChatOpenAI instance.
   *
   * @param {Configuration} configuration - The configuration instance to be used
   * @throws {Error} If OPENAI_API_KEY environment variable is not set
   */
  /**
   * Constructor for initializing a new Instance.
   *
   * @param {Configuration} configuration - The configuration for the instance.
   */
  constructor(private configuration: Configuration) {
    this.typeScriptParser = new TypeScriptParser();
    this.codeFormatter = new CodeFormatter();
    this.documentOrganizer = new DocumentOrganizer();
    this.aiService = new AIService(configuration);
  }

    // Generate overview, FAQ, and troubleshooting together

    // Format the FAQ and troubleshooting sections

    // Remove markdown code block syntax if present

                   // other configuration...
                       // other plugins...

    // write fileGroups to a json file

    // Generate documentation for each file without individual intros

  /**
   * Generates troubleshooting guide based on documentation and common patterns
   */
  // toDo - integrate w/ @Jin's discord scraper to pull solutions for known issues

    // Add TypeScript code block for the file path to indicate it's a TypeScript module

  /**
   * Generates TODO section documentation from found TODO comments
   */
  // toDo - integrate w/ @Jin's discord scraper to auto create GH issues/bounties

    // Remove leading ./ if present

    // Ensure the path has .ts extension

    // Join with the absolute path and src directory

  ///////////////////////////////
  /// Eliza Specific Constructs//
  ///////////////////////////////

      // Remove ./ prefix and ensure path includes src directory and .ts extension

        // Use PROMPT_TEMPLATES.actionDoc

      // Remove ./ prefix and ensure path includes src directory and .ts extension

        // Create a provider object with relevant information
          // Extract provider properties

      // Remove ./ prefix and ensure path includes src directory and .ts extension


} // namespace elizaos
