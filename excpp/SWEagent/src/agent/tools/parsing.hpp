#pragma once
#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <tuple>
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
    std::tuple<std::string, std::string> call(const std::variant<ModelOutput, std::string>& modelResponse, std::optional<std::vector<Command>> _commands, bool strict = false);
};

/**
 * Action-only parser (no thought parsing)
 */
class ActionOnlyParser {
public:
    std::tuple<std::string, std::string> call(const std::variant<ModelOutput, std::string>& modelResponse, std::optional<std::vector<Command>> _commands, bool strict = false);
};

/**
 * XML-based thought-action parser
 */
class XMLThoughtActionParser {
public:
    std::tuple<std::string, std::string> call(const std::variant<ModelOutput, std::string>& modelResponse, std::optional<std::vector<Command>> _commands, bool strict = false);
};

/**
 * Edit format parser for special edit commands
 */
class EditFormatParser {
public:
    std::tuple<std::string, std::string> call(const std::variant<ModelOutput, std::string>& modelResponse, std::optional<std::vector<Command>> _commands, bool strict = false);
};

/**
 * Function calling parser for OpenAI-style function calls
 */
class FunctionCallingParser {
public:
    std::string formatErrorMessage(std::optional<std::any> error);
    std::tuple<std::string, std::string> call(const std::variant<ModelOutput, std::string>& modelResponse, std::optional<std::vector<Command>> _commands, bool strict = false);
};

/**
 * Single bash code block parser
 */
class SingleBashCodeBlockParser {
public:
    std::tuple<std::string, std::string> call(const std::variant<ModelOutput, std::string>& modelResponse, std::optional<std::vector<Command>> _commands, bool strict = false);
};

/**
 * Multiple bash code blocks parser
 */
class MultipleBashCodeBlocksParser {
public:
    std::tuple<std::string, std::string> call(const std::variant<ModelOutput, std::string>& modelResponse, std::optional<std::vector<Command>> _commands, bool strict = false);
};

/**
 * Identity parser - returns input as both thought and action
 */
class Identity {
public:
    std::tuple<std::string, std::string> call(const std::variant<ModelOutput, std::string>& modelResponse, std::optional<std::vector<Command>> _commands, bool _strict = false);
};

/**
 * Last line parser - uses only the last line as action
 */
class LastLineParser {
public:
    std::tuple<std::string, std::string> call(const std::variant<ModelOutput, std::string>& modelResponse, std::optional<std::vector<Command>> _commands, bool strict = false);
};

/**
 * Factory function to get parser by name
 */
AbstractParseFunction getParser(const std::string& parserName);

/**
 * Main parse function used by tools
 */
std::tuple<std::string, std::string> parseAction(const std::variant<ModelOutput, std::string>& modelResponse, std::variant<std::string, AbstractParseFunction> parseFunction = "thought_action", std::optional<std::vector<Command>> commands, bool strict = false);

// Export types
using ParseFunction = AbstractParseFunction;

// Identity parser for testing
class IdentityParser {
public:
    std::tuple<std::string, std::string> call(const std::variant<ModelOutput, std::string>& modelResponse, std::optional<std::vector<Command>> _commands, std::optional<bool> _strict);
};

// Additional exports for compatibility
class JsonParser {
public:
    std::tuple<std::string, std::string> call(const std::variant<ModelOutput, std::string>& modelResponse, std::optional<std::vector<Command>> _commands, std::optional<bool> strict);
};

/**
 * Create a parser instance based on the specified type
 * @param type - The parser type to create
 * @returns An instance of the appropriate parser
 */
AbstractParseFunction createParser(const std::string& type);

} // namespace elizaos
