#include ".TypeScriptParser.js.hpp"
#include "utils/CodeFormatter.js.hpp"
#include "utils/DocumentOrganizer.js.hpp"
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
 * Service for interacting with OpenAI chat API.
 */
/**
 * Class representing an AI service for generating comments based on prompts.
 */

class AIService {
  private chatModel: ChatOpenAI;
  private codeFormatter: CodeFormatter;
  private chatModelFAQ: ChatOpenAI;

  /**
   * Constructor for initializing the ChatOpenAI instance.
   *
   * @param {Configuration} configuration - The configuration instance to be used
   * @throws {Error} If OPENAI_API_KEY environment variable is not set
   */
  constructor(private configuration: Configuration) {
    if (!process.env.OPENAI_API_KEY) {
      throw new Error("OPENAI_API_KEY is not set");
    }
    this.chatModel = new ChatOpenAI({ apiKey: process.env.OPENAI_API_KEY });
    this.chatModelFAQ = new ChatOpenAI({
      apiKey: process.env.OPENAI_API_KEY,
      model: "gpt-4o",
    });
    this.codeFormatter = new CodeFormatter();
  }

  /**
   * Generates a comment based on the specified prompt by invoking the chat model.
   * @param {string} prompt - The prompt for which to generate a comment
   * @returns {Promise<string>} The generated comment
   */
      // First try with generous limit

          // Try with more aggressive truncation
              // Final attempt with minimal context

  /**
   * Handle API errors by logging the error message and throwing the error.
   *
   *
   * @param {Error} error The error object to handle
   * @returns {void}
   */

} // namespace elizaos
