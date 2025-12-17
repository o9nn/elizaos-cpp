#include ".exceptions.hpp"
#include ".types.hpp"
#include "commands.hpp"
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
 * Tool parsing functions
 * Converted from sweagent/tools/parsing.py
 */

/**
 * Abstract base class for parsing functions
 */

/**
 * Action parser - expects single command
 */
class ActionParser extends AbstractParseFunction {
  type = 'action';
  errorMessage = `The command you provided was not recognized. Please specify one of the commands (+ any necessary arguments) from the following list in your response. Do not include any other text.

COMMANDS:
{command_docs}`;

/**
 * Action-only parser
 */
class ActionOnlyParser extends AbstractParseFunction {
  type = 'action_only';
  errorMessage = 'No message found in model response.';

  parse(modelResponse: ModelResponse, _commands: Command[], _strict: boolean = false): [string, string] {
    const message = modelResponse.message || '';
    return ['', message];
  }

/**
 * Thought-action parser - expects discussion followed by backtick-wrapped command
 */
class ThoughtActionParser extends AbstractParseFunction {
  type = 'thought_action';
  errorMessage = `Your output was not formatted correctly. You must always include one discussion and one command as part of your response. Make sure you do not have multiple discussion/command tags.
Please make sure your output precisely matches the following format:
DISCUSSION
Discuss here with yourself about what your planning and what you're going to do in this step.

\`\`\`
command(s) that you're going to run
\`\`\``;

  parse(modelResponse: ModelResponse, _commands: Command[], strict: boolean = false): [string, string] {
    const message = modelResponse.message || '';

    // Find backtick blocks
    const backtickPattern = /```([\s\S]*?)```/g;
    const matches = Array.from(message.matchAll(backtickPattern));

    if (matches.length === 0) {
      if (strict) {
        throw new FormatError('No code block found in response');
      }
      return [message, ''];
    }

    if (matches.length > 1 && strict) {
      throw new FormatError('Multiple code blocks found in response');
    }

    const match = matches[0] as RegExpMatchArray;
    const codeBlock = match[1].trim();
    const thought = message.substring(0, match.index || 0).trim();

    return [thought, codeBlock];
  }

/**
 * XML thought-action parser
 */
class XMLThoughtActionParser extends AbstractParseFunction {
  type = 'xml_thought_action';
  errorMessage = `Your output was not formatted correctly. You must always include one discussion and one command as part of your response. Make sure you do not have multiple discussion/command tags.`;

  parse(modelResponse: ModelResponse, _commands: Command[], strict: boolean = false): [string, string] {
    const message = modelResponse.message || '';

    // Find XML command blocks
    const commandPattern = /<command>([\s\S]*?)<\/command>/g;
    const matches = Array.from(message.matchAll(commandPattern));

    if (matches.length === 0) {
      if (strict) {
        throw new FormatError('No <command> tag found in response');
      }
      return [message, ''];
    }

    if (matches.length > 1 && strict) {
      throw new FormatError('Multiple <command> tags found in response');
    }

    const match = matches[0] as RegExpMatchArray;
    const command = match[1].trim();
    const thought = message.substring(0, match.index || 0).trim();

    return [thought, command];
  }

/**
 * Function calling parser - expects LiteLLM tool calls
 */
class FunctionCallingParser extends AbstractParseFunction {
  type = 'function_calling';
  errorMessage = `{%- if error_code == "missing" -%}

    // Find the command

    // Parse arguments

    // Build command string

      // Quote if contains spaces or special characters
    return String(value);

    // Simple template processing for error messages

    // Handle conditional blocks based on error code

      // Extract the "missing" block
      // Extract the "multiple" block
      // Extract the else block

    // Replace template variables

/**
 * JSON parser
 */
class JsonParser extends AbstractParseFunction {
  type = 'json';
  errorMessage = `Your output could not be parsed as JSON. Please make sure your output 1) is valid JSON and 2) Includes the "thought" and "command" fields.`;

  parse(modelResponse: ModelResponse, _commands: Command[], _strict: boolean = false): [string, string] {
    const message = modelResponse.message || '';

    try {
      const parsed = JSON.parse(message);
      const thought = parsed.thought || '';
      const command = parsed.command || parsed.action || '';

      if (!command) {
        throw new FormatError('No command/action field in JSON');
      }

      return [thought, command];
    } catch (error) {
      if (error instanceof FormatError) {
        throw error;
      }
      throw new FormatError('Invalid JSON in response');
    }
  }

/**
 * EditFormat parser - expects discussion followed by replacement text in backticks
 */
class EditFormat extends ThoughtActionParser {
  type = 'edit_format';
  errorMessage = `Your output was not formatted correctly. You must wrap the replacement text in backticks (\`\`\`).
Please make sure your output precisely matches the following format:
COMMENTS
You can write comments here about what you're going to do if you want.

\`\`\`
New window contents.
Make sure you copy the entire contents of the window here, with the required indentation.
Make the changes to the window above directly in this window.
Remember that all of the window's contents will be replaced with the contents of this window.
Don't include line numbers in your response.
\`\`\``;
}

/**
 * Identity parser - returns message as both thought and action
 */
class Identity extends AbstractParseFunction {
  type = 'identity';
  errorMessage = 'It seems like something went wrong with your output. Please try again.';

  parse(modelResponse: ModelResponse, _commands: Command[], _strict: boolean = false): [string, string] {
    const message = modelResponse.message || '';
    return [message, message];
  }

// Export Identity as IdentityParser for backward compatibility

/**
 * Parse function type
 */
using ParseFunction = std::variant<, ActionParser, ActionOnlyParser, ThoughtActionParser, XMLThoughtActionParser, FunctionCallingParser, JsonParser, EditFormat, Identity>;

/**
 * Create parser from config
 */
AbstractParseFunction createParser(string | { type: string } config);

} // namespace elizaos
