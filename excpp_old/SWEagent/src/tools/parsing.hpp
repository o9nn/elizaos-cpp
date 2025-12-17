#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include ".exceptions.hpp"
#include ".types.hpp"
#include "commands.hpp"

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
class ActionParser {
public:
    [string, string] parse(ModelResponse modelResponse, const std::vector<Command>& _commands, boolean = false _strict);
};

/**
 * Action-only parser
 */
class ActionOnlyParser {
public:
    [string, string] parse(ModelResponse modelResponse, const std::vector<Command>& _commands, boolean = false _strict);
};

/**
 * Thought-action parser - expects discussion followed by backtick-wrapped command
 */
class ThoughtActionParser {
public:
    [string, string] parse(ModelResponse modelResponse, const std::vector<Command>& _commands, boolean = false strict);
};

/**
 * XML thought-action parser
 */
class XMLThoughtActionParser {
public:
    [string, string] parse(ModelResponse modelResponse, const std::vector<Command>& _commands, boolean = false strict);
};

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

  parse(modelResponse: ModelResponse, _commands: Command[], _strict: boolean = false): [string, string] {
    const message = modelResponse.message || '';

    try {
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
AbstractParseFunction createParser(const std::variant<std::string, std::any>& config);

} // namespace elizaos
