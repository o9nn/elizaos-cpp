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
 * Command definitions for tools
 * Converted from sweagent/tools/commands.py
 */

/**
 * Extract keys from a format string
 */
std::unordered_set<std::string> extractKeys(const std::string& formatString);

/**
 * Command argument definition
 */
struct IArgument {
    std::string name;
    std::string type;
    std::optional<std::unordered_map<std::string, std::string>> items;
    std::string description;
    bool required;
    std::optional<string[] | null> enum;
    std::string argumentFormat;
};

/**
 * Command argument implementation
 */
class ArgumentImpl implements IArgument {
  name: string;
  type: string;
  items?: Record<string, string> | null;
  description: string;
  required: boolean;
  enum?: string[] | null;
  argumentFormat: string;

  constructor(config: {
    name: string;
    type: string;
    items?: Record<string, string> | null;
    description: string;
    required: boolean;
    enum?: string[] | null;
    argumentFormat?: string;
  }) {
    this.name = config.name;
    this.type = config.type;
    this.items = config.items || null;
    this.description = config.description;
    this.required = config.required;
    this.enum = config.enum || null;
    this.argumentFormat = config.argumentFormat || '{{value}}';

    this.validateArgumentFormat();
  }

/**
 * Command definition
 */
class Command {
  name: string;
  docstring: string | null;
  signature: string | null;
  endName?: string;
  arguments: IArgument[];

  constructor(config: {
    name: string;
    docstring?: string | null;
    signature?: string | null;
    endName?: string;
    arguments?: IArgument[];
  }) {
    this.name = config.name;
    this.docstring = config.docstring || null;
    this.signature = config.signature || null;
    this.endName = config.endName;
    this.arguments = config.arguments || [];

    this.validateArguments();
  }

    // If there's a custom signature, process it
      // Replace angle brackets (and optional brackets) with curly braces for arguments
      // Handle both <arg> and [<arg>] patterns

    // Build the default invocation format string

    // Check for required arguments after optional ones

    // Check for duplicate argument names

      // Validate argument name pattern

      // Validate argument type

    // If there's a signature, validate that all arguments appear in it

      // Check for extra arguments in signature

// Export ArgumentImpl as Argument for tests that expect it as a class

} // namespace elizaos
