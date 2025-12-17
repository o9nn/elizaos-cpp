#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include "..exceptions.hpp"
#include ".types.hpp"

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
class ThoughtActionParser {
public:
    [string, string] call(const std::variant<ModelOutput, std::string>& modelResponse, std::optional<std::vector<Command>> _commands, boolean = false strict);
};

/**
 * Action-only parser (no thought parsing)
 */
class ActionOnlyParser {
public:
    [string, string] call(const std::variant<ModelOutput, std::string>& modelResponse, std::optional<std::vector<Command>> _commands, boolean = false strict);
};

/**
 * XML-based thought-action parser
 */
class XMLThoughtActionParser {
public:
    [string, string] call(const std::variant<ModelOutput, std::string>& modelResponse, std::optional<std::vector<Command>> _commands, boolean = false strict);
};

/**
 * Edit format parser for special edit commands
 */
class EditFormatParser {
public:
    [string, string] call(const std::variant<ModelOutput, std::string>& modelResponse, std::optional<std::vector<Command>> _commands, boolean = false strict);
};

/**
 * Function calling parser for OpenAI-style function calls
 */
class FunctionCallingParser {
public:
    std::string formatErrorMessage(std::optional<std::any> error);
    [string, string] call(const std::variant<ModelOutput, std::string>& modelResponse, std::optional<std::vector<Command>> _commands, boolean = false strict);
};

/**
 * Single bash code block parser
 */
class SingleBashCodeBlockParser {
public:
    [string, string] call(const std::variant<ModelOutput, std::string>& modelResponse, std::optional<std::vector<Command>> _commands, boolean = false strict);
};

/**
 * Multiple bash code blocks parser
 */
class MultipleBashCodeBlocksParser {
public:
    [string, string] call(const std::variant<ModelOutput, std::string>& modelResponse, std::optional<std::vector<Command>> _commands, boolean = false strict);
};

/**
 * Identity parser - returns input as both thought and action
 */
class Identity {
public:
    [string, string] call(const std::variant<ModelOutput, std::string>& modelResponse, std::optional<std::vector<Command>> _commands, boolean = false _strict);
};

/**
 * Last line parser - uses only the last line as action
 */
class LastLineParser {
public:
    [string, string] call(const std::variant<ModelOutput, std::string>& modelResponse, std::optional<std::vector<Command>> _commands, boolean = false strict);
};

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
class IdentityParser {
public:
    [string, string] call(const std::variant<ModelOutput, std::string>& modelResponse, std::optional<std::vector<Command>> _commands, std::optional<bool> _strict);
};

// Additional exports for compatibility
class JsonParser {
public:
    [string, string] call(const std::variant<ModelOutput, std::string>& modelResponse, std::optional<std::vector<Command>> _commands, std::optional<bool> strict);
};

/**
 * Create a parser instance based on the specified type
 * @param type - The parser type to create
 * @returns An instance of the appropriate parser
 */
AbstractParseFunction createParser(const std::string& type);

} // namespace elizaos
