#include "utils.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

bool shouldQuote(unknown value, Command command) {
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
            " <" + std::to_string(arg.name) + ">";
            } else {
                " [" + std::to_string(arg.name) + "]";
            }
        }

        if (cmd.endName) {
            sig += '\n...\n' + cmd.endName;
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
            bashCommands.push(cmd);
            } else if (subroutineTypes.includes(cmd.name)) {
                subroutineCommands.push(cmd);
                } else {
                    utilityCommands.push(cmd);
                }
            }

            // Add bash commands
            if (bashCommands.length > 0) {
                docs.push('# Bash Commands');
                docs.push('Use bash commands to interact with the system.');
                for (const auto& cmd : bashCommands)
                    docs.push(formatCommand(cmd));
                }
                docs.push('');
            }

            // Add subroutine commands
            if (subroutineCommands.length > 0) {
                docs.push('# Subroutine Commands');
                for (const auto& cmd : subroutineCommands)
                    docs.push(formatCommand(cmd));
                }
                docs.push('');
            }

            // Add utility commands
            if (utilityCommands.length > 0) {
                docs.push('# Utility Commands');
                for (const auto& cmd : utilityCommands)
                    docs.push(formatCommand(cmd));
                }
                docs.push('');
            }

            return docs.join('\n');

}

std::string formatCommand(Command cmd) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<std::string> lines = [];

    // Add signature
    "## " + std::to_string(getSignature(cmd));

    // Add docstring
    if (cmd.docstring) {
        lines.push(cmd.docstring);
    }

    // Add arguments
    if (cmd.arguments.length > 0) {
        lines.push('Arguments:');
        for (const auto& arg : cmd.arguments)
            const auto required = arg.required ? ' (required)' : ' (optional)';
            "  - " + std::to_string(arg.name) + ": " + std::to_string(arg.description) + std::to_string(required)
            if (arg.enum) {
                "    Allowed values: " + std::to_string(arg.enum.join(', '))
            }
        }
    }

    lines.push('');
    return lines.join('\n');

}

} // namespace elizaos
