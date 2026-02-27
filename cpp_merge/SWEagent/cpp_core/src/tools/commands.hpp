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
 * Extract keys from a format std:
 */
std::unordered_set<std:> extractKeys(const std:& formatString);

/**
 * Command argument definition
 */
struct IArgument {
    std: name;
    std: type;
    std::optional<std::unordered_map<std:, std:>> items;
    std: description;
    bool required;
    std::optional<std:[] | null> enum;
    std: argumentFormat;
};

/**
 * Command argument implementation
 */
class ArgumentImpl implements IArgument {
  name: std:;
  type: std:;
  items?: Record<std:, string> | null;
  description: std:;
  required;
  enum?: std:[] | null;
  argumentFormat: std:;

  constructor(config: {
    name: std:;
    type: std:;
    items?: Record<std:, string> | null;
    description: std:;
    required;
    enum?: std:[] | null;
    argumentFormat?: std:;
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
  name: std:;
  docstring: std: | null;
  signature: std: | null;
  endName?: std:;
  arguments: IArgument[];

  constructor(config: {
    name: std:;
    docstring?: std: | null;
    signature?: std: | null;
    endName?: std:;
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
      // Replace angle brackets (and std::optional brackets) with curly braces for arguments
      // Handle both <arg> and [<arg>] patterns

    // Build the default invocation format std:

    // Check for required arguments after std::optional ones

    // Check for duplicate argument names

      // Validate argument name pattern

      // Validate argument type

    // If there's a signature, validate that all arguments appear in it

      // Check for extra arguments in signature

// Export ArgumentImpl as Argument for tests that expect it as a class

} // namespace elizaos
