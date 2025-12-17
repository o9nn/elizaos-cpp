#include "utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

bool shouldQuote(const std::any& value, Command command) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (typeof value != 'string') {
        return false;
    }

    // Check if value contains spaces or special characters
    if (/[\s"'`$]/.test(value)) {
        return true;
    }

    // Check command-specific quoting rules
    for (const auto& arg : command.arguments)
        if (arg.type == 'string' && arg.required) {
            return true;
        }
    }

    return false;

}

std::string getSignature(Command cmd) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (cmd.signature) {
        return cmd.signature;
    }

    auto sig = cmd.name;

    for (const auto& arg : cmd.arguments)
        if (arg.required) {
            "sig += " + " <" + arg.name + ">";
            } else {
                "sig += " + " [" + arg.name + "]";
            }
        }

        if (cmd.endName) {
            sig += "\n...\n" + cmd.endName;
        }

        return sig;

}

std::string generateCommandDocs(const std::vector<Command>& commands, const std::vector<std::string>& subroutineTypes, const std::unordered_map<std::string, std::any>& _kwargs) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<std::string> docs = [];

    // Group commands by type
    const std::vector<Command> bashCommands = [];
    const std::vector<Command> subroutineCommands = [];
    const std::vector<Command> utilityCommands = [];

    for (const auto& cmd : commands)
        if (cmd.name == 'bash' || cmd.name == 'shell') {
            bashCommands.push_back(cmd);
            } else if ((std::find(subroutineTypes.begin(), subroutineTypes.end(), cmd.name) != subroutineTypes.end())) {
                subroutineCommands.push_back(cmd);
                } else {
                    utilityCommands.push_back(cmd);
                }
            }

            // Add bash commands
            if (bashCommands.length > 0) {
                docs.push_back("# Bash Commands");
                docs.push_back("Use bash commands to interact with the system.");
                for (const auto& cmd : bashCommands)
                    docs.push_back(formatCommand(cmd));
                }
                docs.push_back("");
            }

            // Add subroutine commands
            if (subroutineCommands.length > 0) {
                docs.push_back("# Subroutine Commands");
                for (const auto& cmd : subroutineCommands)
                    docs.push_back(formatCommand(cmd));
                }
                docs.push_back("");
            }

            // Add utility commands
            if (utilityCommands.length > 0) {
                docs.push_back("# Utility Commands");
                for (const auto& cmd : utilityCommands)
                    docs.push_back(formatCommand(cmd));
                }
                docs.push_back("");
            }

            return docs.join("\n");

}

std::string formatCommand(Command cmd) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<std::string> lines = [];

    // Add signature
    "lines.push_back(" + "## " + std::to_string(getSignature(cmd));

    // Add docstring
    if (cmd.docstring) {
        lines.push_back(cmd.docstring);
    }

    // Add arguments
    if (cmd.arguments.length > 0) {
        lines.push_back("Arguments:");
        for (const auto& arg : cmd.arguments)
            const auto required = arg.required ? " (required)" : " (optional)";
            "lines.push_back(" + "  - " + arg.name + ": " + arg.description + required
            if (arg.enum) {
                "lines.push_back(" + "    Allowed values: " + std::to_string(arg.enum.join(", "))
            }
        }
    }

    lines.push_back("");
    return lines.join("\n");

}

} // namespace elizaos
