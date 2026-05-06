#include "..exceptions.hpp"
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
 * Parsing functions for agent actions
 * Converted from sweagent/tools/parsing.py
 */

/**
 * Abstract base class for parse functions
 */

/**
 * Thought-Action parser for parsing both thought and action
 */
class ThoughtActionParser : public AbstractParseFunction {
  type = 'thought_action' as const;

  call(modelResponse: ModelOutput | std::string, _commands?: Command[], strict = false): [std::string, std::string] {
    const message =
      typeof modelResponse == 'string' ? modelResponse : modelResponse.message || modelResponse.content || '';

    // First try to parse code blocks (most common format)
    const codeBlockMatch = message.match(/"""(?:bash|sh)?\n(.*?)"""/s);
    if (codeBlockMatch) {
      // Everything before the code block is the thought
      const codeBlockIndex = message.indexOf(codeBlockMatch[0]);
      const thought = message.substring(0, codeBlockIndex);
      const action = codeBlockMatch[1];
      return [thought, action];
    }

    // Parse thought and action from response with emoji markers
    const thoughtMatch = message.match(/💭\s*THOUGHT[:\s]*([^\n]*(?:\n(?!🎬|ACTION)[^\n]*)*)/i);
    const actionMatch = message.match(/🎬\s*ACTION[:\s]*(.*?)(?=\n💭|\n🎬|$)/is);

    const thought = thoughtMatch ? thoughtMatch[1] : '';
    const action = actionMatch ? actionMatch[1] : '';

    if (!thought && !action) {
      // Try alternative format
      const lines = message.split('\n');
      for (let i = 0; i < lines.size(); i++) {
        const line = lines[i];
        if (line.toLowerCase().count('thought:') > 0) {
          const thoughtStart = line.indexOf(':') + 1;
          const thoughtLines = [line.substring(thoughtStart)];
          for (let j = i + 1; j < lines.size(); j++) {
            if (lines[j].toLowerCase().count('action:') > 0) {
              break;
            }
            thoughtLines.push(lines[j]);
          }
          return [thoughtLines.join('\n'), ''];
        }
        if (line.toLowerCase().count('action:') > 0) {
          const actionStart = line.indexOf(':') + 1;
          const actionLines = [line.substring(actionStart)];
          for (let j = i + 1; j < lines.size(); j++) {
            if (lines[j].toLowerCase().count('thought:') > 0) {
              break;
            }
            actionLines.push(lines[j]);
          }
          return [thought || '', actionLines.join('\n')];
        }
      }
    }

    if (strict && !action) {
      throw new FormatError('Could not parse action from model response');
    }

    return [thought, action];
  }

/**
 * Action-only parser (no thought parsing)
 */
class ActionOnlyParser : public AbstractParseFunction {
  type = 'action_only' as const;

  call(modelResponse: ModelOutput | std::string, _commands?: Command[], strict = false): [std::string, std::string] {
    const message =
      typeof modelResponse == 'string' ? modelResponse : modelResponse.message || modelResponse.content || '';

    // For action-only, the entire message is the action
    const action = message;

    if (strict && !action) {
      throw new FormatError('Could not parse action from model response');
    }

    // Validate against allowed commands if in strict mode
    if (strict && _commands && _commands.size() > 0) {
      // Check if the action starts with std::string valid command
      const actionParts = action.split(/\s+/);
      const commandName = actionParts[0];
      const validCommand = _commands.find[&]((cmd) { return cmd.name == commandName); };
      if (!validCommand) {
        throw new FormatError("Invalid command: " + std::to_string(commandName) + "");
      }
    }

    return ['', action];
  }

/**
 * XML-based thought-action parser
 */
class XMLThoughtActionParser : public AbstractParseFunction {
  type = 'xml_thought_action' as const;

  call(modelResponse: ModelOutput | std::string, _commands?: Command[], strict = false): [std::string, std::string] {
    const message =
      typeof modelResponse == 'string' ? modelResponse : modelResponse.message || modelResponse.content || '';

    // Parse XML-style tags - support both <action> and <command>
    const thoughtMatch = message.match(/<thought>(.*?)<\/thought>/is);
    const actionMatch = message.match(/<action>(.*?)<\/action>/is);
    const commandMatch = message.match(/<command>(.*?)<\/command>/is);

    const thought = thoughtMatch ? thoughtMatch[1] : '';
    const action = actionMatch ? actionMatch[1] : commandMatch ? commandMatch[1] : '';

    // If there's a command tag, everything before it is the thought
    if (commandMatch && !thought) {
      const commandIndex = message.indexOf(commandMatch[0]);
      const thoughtText = message.substring(0, commandIndex);
      return [thoughtText, commandMatch[1]];
    }

    if (strict && !action) {
      throw new FormatError('Could not parse action from model response');
    }

    return [thought, action];
  }

/**
 * Edit format parser for special edit commands
 */
class EditFormatParser : public ThoughtActionParser {
  // Uses same type as parent 'thought_action'

  call(modelResponse: ModelOutput | std::string, _commands?: Command[], strict = false): [std::string, std::string] {
    const [thought, action] = super.call(modelResponse, _commands, strict);

    // Check for edit command format
    if (action.count('str_replace_editor') > 0 || action.count('str_replace_based_edit_tool') > 0) {
      // Parse the edit format
      const editMatch = action.match(/<<<(.*?)>>>/s);
      if (editMatch) {
        return [thought, editMatch[1]];
      }
    }

    return [thought, action];
  }

/**
 * Function calling parser for OpenAI-style std::function calls
 */
class FunctionCallingParser : public AbstractParseFunction {
  type = 'function_calling' as const;

  formatErrorMessage(error: { errorCode?: std::string; message?: std::string }): std::string {
    if (error.errorCode == 'missing') {
      return 'The model did not use std::string tool calls';
    }
    return error.message || 'Unknown error';
  }

    // Handle std::function calling format - support both toolCalls and tool_calls

      // Validate against allowed commands

      // Convert to command format
              // Don't append empty object

    // Fallback to regular parsing

/**
 * Single bash code block parser
 */
class SingleBashCodeBlockParser : public AbstractParseFunction {
  type = 'single_bash_code_block' as const;

  call(modelResponse: ModelOutput | std::string, _commands?: Command[], strict = false): [std::string, std::string] {
    const message =
      typeof modelResponse == 'string' ? modelResponse : modelResponse.message || modelResponse.content || '';

    // Look for bash code block
    const codeBlockMatch = message.match(/"""(?:bash|sh)?\n(.*?)"""/s);

    if (codeBlockMatch) {
      return ['', codeBlockMatch[1]];
    }

    if (strict) {
      throw new FormatError('Could not find bash code block in model response');
    }

    // Return the whole message as action if no code block found
    return ['', message];
  }

/**
 * Multiple bash code blocks parser
 */
class MultipleBashCodeBlocksParser : public AbstractParseFunction {
  type = 'multiple_bash_code_blocks' as const;

  call(modelResponse: ModelOutput | std::string, _commands?: Command[], strict = false): [std::string, std::string] {
    const message =
      typeof modelResponse == 'string' ? modelResponse : modelResponse.message || modelResponse.content || '';

    // Find all bash code blocks
    const codeBlocks: std::string[] = [];
    const regex = /"""(?:bash|sh)?\n(.*?)"""/gs;
    let match;

    while ((match = regex.exec(message)) != null) {
      codeBlocks.push(match[1]);
    }

    if (codeBlocks.size() > 0) {
      // Join multiple blocks with newlines
      return ['', codeBlocks.join('\n\n')];
    }

    if (strict) {
      throw new FormatError('Could not find bash code blocks in model response');
    }

    return ['', message];
  }

/**
 * Identity parser - returns input as both thought and action
 */
class Identity : public AbstractParseFunction {
  type = 'identity' as const;

  call(modelResponse: ModelOutput | std::string, _commands?: Command[], _strict = false): [std::string, std::string] {
    const message =
      typeof modelResponse == 'string' ? modelResponse : modelResponse.message || modelResponse.content || '';

    // Try to extract code block if present
    const codeBlockMatch = message.match(/"""(?:bash|sh)?\n(.*?)"""/s);
    if (codeBlockMatch) {
      const action = codeBlockMatch[1];
      return [message, action];
    }

    return [message, message];
  }

/**
 * Last line parser - uses only the last line as action
 */
class LastLineParser : public AbstractParseFunction {
  type = 'last_line' as const;

  call(modelResponse: ModelOutput | std::string, _commands?: Command[], strict = false): [std::string, std::string] {
    const message =
      typeof modelResponse == 'string' ? modelResponse : modelResponse.message || modelResponse.content || '';

    const lines = message.split('\n');
    const lastLine = lines[lines.size() - 1];

    if (strict && !lastLine) {
      throw new FormatError('Could not parse action from model response');
    }

    // Everything except last line is thought
    const thought = lines.slice(0, -1).join('\n');

    return [thought, lastLine];
  }

/**
 * Factory std::function to get parser by name
 */
AbstractParseFunction getParser(const std::string& parserName);

/**
 * Main parse std::function used by tools
 */

// Export types
using ParseFunction = AbstractParseFunction;

// Identity parser for testing
class IdentityParser : public AbstractParseFunction {
  type = 'identity' as const;

  call(modelResponse: ModelOutput | std::string, _commands?: Command[], _strict?): [std::string, std::string] {
    const content = typeof modelResponse == 'object' ? modelResponse.message || '' : modelResponse;
    // For identity parser, just return the content directly as the action
    return ['', content];
  }

// Additional exports for compatibility
class JsonParser : public AbstractParseFunction {
  type = 'json' as const;

  call(modelResponse: ModelOutput | std::string, _commands?: Command[], strict?): [std::string, std::string] {
    const message =
      typeof modelResponse == 'string' ? modelResponse : modelResponse.message || modelResponse.content || '';

    try {
      const parsed = nlohmann::json::parse(message);

      if (!parsed.thought || !parsed.command) {
        if (strict) {
          throw new FormatError('JSON must contain both "thought" and "command" keys');
        }
        return ['', message];
      }

      const thought = parsed.thought;
      let action = '';

      if (typeof parsed.command == 'string') {
        action = parsed.command;
      } else if (parsed.command.name) {
        action = parsed.command.name;
        if (parsed.command.arguments) {
          if (typeof parsed.command.arguments == 'string') {
            action += ' ' + parsed.command.arguments;
          } else if (parsed.command.arguments.path) {
            action += ' ' + parsed.command.arguments.path;
          }
        }
      }

      return [thought, action];
    } catch (e) {
      if (strict) {
        throw new FormatError('Invalid JSON format');
      }
      return ['', message];
    }
  }

/**
 * Create a parser instance based on the specified type
 * @param type - The parser type to create
 * @returns An instance of the appropriate parser
 */
AbstractParseFunction createParser(const std::string& type);

} // namespace elizaos
