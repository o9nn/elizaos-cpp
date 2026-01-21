#include "..exceptions.hpp"
#include ".types.hpp"
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
class ThoughtActionParser extends AbstractParseFunction {
  type = 'thought_action' as const;

  call(modelResponse: ModelOutput | string, _commands?: Command[], strict: boolean = false): [string, string] {
    const message =
      typeof modelResponse === 'string' ? modelResponse : modelResponse.message || modelResponse.content || '';

    // First try to parse code blocks (most common format)
    const codeBlockMatch = message.match(/```(?:bash|sh)?\n(.*?)```/s);
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

    const thought = thoughtMatch ? thoughtMatch[1].trim() : '';
    const action = actionMatch ? actionMatch[1].trim() : '';

    if (!thought && !action) {
      // Try alternative format
      const lines = message.split('\n');
      for (let i = 0; i < lines.length; i++) {
        const line = lines[i];
        if (line.toLowerCase().includes('thought:')) {
          const thoughtStart = line.indexOf(':') + 1;
          const thoughtLines = [line.substring(thoughtStart).trim()];
          for (let j = i + 1; j < lines.length; j++) {
            if (lines[j].toLowerCase().includes('action:')) {
              break;
            }
            thoughtLines.push(lines[j]);
          }
          return [thoughtLines.join('\n').trim(), ''];
        }
        if (line.toLowerCase().includes('action:')) {
          const actionStart = line.indexOf(':') + 1;
          const actionLines = [line.substring(actionStart).trim()];
          for (let j = i + 1; j < lines.length; j++) {
            if (lines[j].toLowerCase().includes('thought:')) {
              break;
            }
            actionLines.push(lines[j]);
          }
          return [thought || '', actionLines.join('\n').trim()];
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
class ActionOnlyParser extends AbstractParseFunction {
  type = 'action_only' as const;

  call(modelResponse: ModelOutput | string, _commands?: Command[], strict: boolean = false): [string, string] {
    const message =
      typeof modelResponse === 'string' ? modelResponse : modelResponse.message || modelResponse.content || '';

    // For action-only, the entire message is the action
    const action = message.trim();

    if (strict && !action) {
      throw new FormatError('Could not parse action from model response');
    }

    // Validate against allowed commands if in strict mode
    if (strict && _commands && _commands.length > 0) {
      // Check if the action starts with any valid command
      const actionParts = action.split(/\s+/);
      const commandName = actionParts[0];
      const validCommand = _commands.find((cmd) => cmd.name === commandName);
      if (!validCommand) {
        throw new FormatError(`Invalid command: ${commandName}`);
      }
    }

    return ['', action];
  }

/**
 * XML-based thought-action parser
 */
class XMLThoughtActionParser extends AbstractParseFunction {
  type = 'xml_thought_action' as const;

  call(modelResponse: ModelOutput | string, _commands?: Command[], strict: boolean = false): [string, string] {
    const message =
      typeof modelResponse === 'string' ? modelResponse : modelResponse.message || modelResponse.content || '';

    // Parse XML-style tags - support both <action> and <command>
    const thoughtMatch = message.match(/<thought>(.*?)<\/thought>/is);
    const actionMatch = message.match(/<action>(.*?)<\/action>/is);
    const commandMatch = message.match(/<command>(.*?)<\/command>/is);

    const thought = thoughtMatch ? thoughtMatch[1].trim() : '';
    const action = actionMatch ? actionMatch[1].trim() : commandMatch ? commandMatch[1].trim() : '';

    // If there's a command tag, everything before it is the thought
    if (commandMatch && !thought) {
      const commandIndex = message.indexOf(commandMatch[0]);
      const thoughtText = message.substring(0, commandIndex).trim();
      return [thoughtText, commandMatch[1].trim()];
    }

    if (strict && !action) {
      throw new FormatError('Could not parse action from model response');
    }

    return [thought, action];
  }

/**
 * Edit format parser for special edit commands
 */
class EditFormatParser extends ThoughtActionParser {
  // Uses same type as parent 'thought_action'

  call(modelResponse: ModelOutput | string, _commands?: Command[], strict: boolean = false): [string, string] {
    const [thought, action] = super.call(modelResponse, _commands, strict);

    // Check for edit command format
    if (action.includes('str_replace_editor') || action.includes('str_replace_based_edit_tool')) {
      // Parse the edit format
      const editMatch = action.match(/<<<(.*?)>>>/s);
      if (editMatch) {
        return [thought, editMatch[1].trim()];
      }
    }

    return [thought, action];
  }

/**
 * Function calling parser for OpenAI-style function calls
 */
class FunctionCallingParser extends AbstractParseFunction {
  type = 'function_calling' as const;

  formatErrorMessage(error: { errorCode?: string; message?: string }): string {
    if (error.errorCode === 'missing') {
      return 'The model did not use any tool calls';
    }
    return error.message || 'Unknown error';
  }

    // Handle function calling format - support both toolCalls and tool_calls

      // Validate against allowed commands

      // Convert to command format
              // Don't append empty object

    // Fallback to regular parsing

/**
 * Single bash code block parser
 */
class SingleBashCodeBlockParser extends AbstractParseFunction {
  type = 'single_bash_code_block' as const;

  call(modelResponse: ModelOutput | string, _commands?: Command[], strict: boolean = false): [string, string] {
    const message =
      typeof modelResponse === 'string' ? modelResponse : modelResponse.message || modelResponse.content || '';

    // Look for bash code block
    const codeBlockMatch = message.match(/```(?:bash|sh)?\n(.*?)```/s);

    if (codeBlockMatch) {
      return ['', codeBlockMatch[1].trim()];
    }

    if (strict) {
      throw new FormatError('Could not find bash code block in model response');
    }

    // Return the whole message as action if no code block found
    return ['', message.trim()];
  }

/**
 * Multiple bash code blocks parser
 */
class MultipleBashCodeBlocksParser extends AbstractParseFunction {
  type = 'multiple_bash_code_blocks' as const;

  call(modelResponse: ModelOutput | string, _commands?: Command[], strict: boolean = false): [string, string] {
    const message =
      typeof modelResponse === 'string' ? modelResponse : modelResponse.message || modelResponse.content || '';

    // Find all bash code blocks
    const codeBlocks: string[] = [];
    const regex = /```(?:bash|sh)?\n(.*?)```/gs;
    let match;

    while ((match = regex.exec(message)) !== null) {
      codeBlocks.push(match[1].trim());
    }

    if (codeBlocks.length > 0) {
      // Join multiple blocks with newlines
      return ['', codeBlocks.join('\n\n')];
    }

    if (strict) {
      throw new FormatError('Could not find bash code blocks in model response');
    }

    return ['', message.trim()];
  }

/**
 * Identity parser - returns input as both thought and action
 */
class Identity extends AbstractParseFunction {
  type = 'identity' as const;

  call(modelResponse: ModelOutput | string, _commands?: Command[], _strict: boolean = false): [string, string] {
    const message =
      typeof modelResponse === 'string' ? modelResponse : modelResponse.message || modelResponse.content || '';

    // Try to extract code block if present
    const codeBlockMatch = message.match(/```(?:bash|sh)?\n(.*?)```/s);
    if (codeBlockMatch) {
      const action = codeBlockMatch[1].trim();
      return [message, action];
    }

    return [message, message];
  }

/**
 * Last line parser - uses only the last line as action
 */
class LastLineParser extends AbstractParseFunction {
  type = 'last_line' as const;

  call(modelResponse: ModelOutput | string, _commands?: Command[], strict: boolean = false): [string, string] {
    const message =
      typeof modelResponse === 'string' ? modelResponse : modelResponse.message || modelResponse.content || '';

    const lines = message.trim().split('\n');
    const lastLine = lines[lines.length - 1].trim();

    if (strict && !lastLine) {
      throw new FormatError('Could not parse action from model response');
    }

    // Everything except last line is thought
    const thought = lines.slice(0, -1).join('\n').trim();

    return [thought, lastLine];
  }

/**
 * Factory function to get parser by name
 */
AbstractParseFunction getParser(const std::string& parserName);

/**
 * Main parse function used by tools
 */

// Export types
using ParseFunction = AbstractParseFunction;

// Identity parser for testing
class IdentityParser extends AbstractParseFunction {
  type = 'identity' as const;

  call(modelResponse: ModelOutput | string, _commands?: Command[], _strict?: boolean): [string, string] {
    const content = typeof modelResponse === 'object' ? modelResponse.message || '' : modelResponse;
    // For identity parser, just return the content directly as the action
    return ['', content.trim()];
  }

// Additional exports for compatibility
class JsonParser extends AbstractParseFunction {
  type = 'json' as const;

  call(modelResponse: ModelOutput | string, _commands?: Command[], strict?: boolean): [string, string] {
    const message =
      typeof modelResponse === 'string' ? modelResponse : modelResponse.message || modelResponse.content || '';

    try {
      const parsed = JSON.parse(message);

      if (!parsed.thought || !parsed.command) {
        if (strict) {
          throw new FormatError('JSON must contain both "thought" and "command" keys');
        }
        return ['', message];
      }

      const thought = parsed.thought;
      let action = '';

      if (typeof parsed.command === 'string') {
        action = parsed.command;
      } else if (parsed.command.name) {
        action = parsed.command.name;
        if (parsed.command.arguments) {
          if (typeof parsed.command.arguments === 'string') {
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
