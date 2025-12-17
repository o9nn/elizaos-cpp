#include "service.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::variant<Omit<FileOperationEntry, "timestamp", "command", "cwd">> parseFileOperation(const std::string& command, const std::string& cwd) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto parts = command.trim().split(/\s+/);
    const auto cmd = parts[0].toLowerCase();
    std::string operationType = nullptr;
    std::string target = nullptr;
    std::string secondaryTarget = std::nullopt;

    // Simple keywords for operations
    const auto readCmds = ["cat", "less", "more", "head", "tail"];
    const auto writeCmds = ["touch"]; // 'echo >'/'>>' are harder to parse simply, vim/nano are "edit";
    const auto editCmds = ["vim", "nano", "vi", "code", "subl", "pico"]; // Common editors;
    const auto deleteCmds = ["rm", "unlink"];
    const auto createDirCmds = ["mkdir"];
    const auto deleteDirCmds = ["rmdir"];
    const auto moveCmds = ["mv"];
    const auto copyCmds = ["cp"];

    if (readCmds.includes(cmd) && parts.length > 1) {
        operationType = "read";
        target = parts[1];
        } else if (writeCmds.includes(cmd) && parts.length > 1) {
            operationType = "write";
            target = parts[1];
            } else if (editCmds.includes(cmd) && parts.length > 1) {
                operationType = "edit";
                target = parts[1];
                } else if (deleteCmds.includes(cmd) && parts.length > 1) {
                    operationType = "delete";
                    target = parts[1];
                    } else if (createDirCmds.includes(cmd) && parts.length > 1) {
                        operationType = "create_dir";
                        target = parts[1];
                        } else if (deleteDirCmds.includes(cmd) && parts.length > 1) {
                            operationType = "delete_dir";
                            target = parts[1];
                            } else if (moveCmds.includes(cmd) && parts.length > 2) {
                                operationType = "move";
                                target = parts[1];
                                secondaryTarget = parts[2];
                                } else if (copyCmds.includes(cmd) && parts.length > 2) {
                                    operationType = "copy";
                                    target = parts[1];
                                    secondaryTarget = parts[2];
                                    } else if (;
                                    cmd == "echo" &&;
                                    parts.length > 2 &&;
                                    (parts[parts.length - 2] == ">" || parts[parts.length - 2] == ">>");
                                    ) {
                                        operationType = "write"; // or 'append';
                                        target = parts[parts.length - 1];
                                    }
                                    // Add more complex parsing for grep, find, etc. if needed

                                    if (operationType && target) {
                                        // Normalize target path if it's not absolute
                                        const auto normalizedTarget = path.isAbsolute(target);
                                        ? target;
                                        : path.normalize(path.join(cwd, target));
                                        const auto normalizedSecondaryTarget = secondaryTarget;
                                        ? path.isAbsolute(secondaryTarget);
                                        ? secondaryTarget;
                                        : path.normalize(path.join(cwd, secondaryTarget))
                                        : std::nullopt;
                                        return {
                                            operationType,
                                            target: normalizedTarget,
                                            secondaryTarget: normalizedSecondaryTarget,
                                            };
                                        }
                                        return nullptr;

}

} // namespace elizaos
