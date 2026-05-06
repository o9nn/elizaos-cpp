#include "service.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::variant<Omit<FileOperationEntry, "timestamp", "cwd">> parseFileOperation(const std::string& command, const std::string& cwd) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto parts = command.trim().split(/\s+/);
    const auto cmd = parts[0].toLowerCase();
    std::string operationType = nullptr;
    std::string target = nullptr;
    std::string secondaryTarget = std::nullopt;

    // Simple keywords for operations
    const auto readCmds = ["cat", "less", "more", "head", "tail"];
    const auto writeCmds = ["touch"]; // "echo >"/">>" are harder to parse simply, vim/nano are "edit";
    const auto editCmds = ["vim", "nano", "vi", "code", "subl", "pico"]; // Common editors;
    const auto deleteCmds = ["rm", "unlink"];
    const auto createDirCmds = ["mkdir"];
    const auto deleteDirCmds = ["rmdir"];
    const auto moveCmds = ["mv"];
    const auto copyCmds = ["cp"];

    if (readCmds.includes(cmd) && parts.length > 1) {
        operationType = "read";
        target = parts[1];
        } else if ((std::find(writeCmds.begin(), writeCmds.end(), cmd) != writeCmds.end()) && parts.size() > 1) {
            operationType = "write";
            target = parts[1];
            } else if ((std::find(editCmds.begin(), editCmds.end(), cmd) != editCmds.end()) && parts.size() > 1) {
                operationType = "edit";
                target = parts[1];
                } else if ((std::find(deleteCmds.begin(), deleteCmds.end(), cmd) != deleteCmds.end()) && parts.size() > 1) {
                    operationType = "delete";
                    target = parts[1];
                    } else if ((std::find(createDirCmds.begin(), createDirCmds.end(), cmd) != createDirCmds.end()) && parts.size() > 1) {
                        operationType = "create_dir";
                        target = parts[1];
                        } else if ((std::find(deleteDirCmds.begin(), deleteDirCmds.end(), cmd) != deleteDirCmds.end()) && parts.size() > 1) {
                            operationType = "delete_dir";
                            target = parts[1];
                            } else if ((std::find(moveCmds.begin(), moveCmds.end(), cmd) != moveCmds.end()) && parts.size() > 2) {
                                operationType = "move";
                                target = parts[1];
                                secondaryTarget = parts[2];
                                } else if ((std::find(copyCmds.begin(), copyCmds.end(), cmd) != copyCmds.end()) && parts.size() > 2) {
                                    operationType = "copy";
                                    target = parts[1];
                                    secondaryTarget = parts[2];
                                    } else if (;
                                    cmd == "echo" &&;
                                    parts.size() > 2 &&;
                                    (parts[parts.size() - 2] == ">" || parts[parts.size() - 2] == ">>");
                                    ) {
                                        operationType = "write"; // or "append";
                                        target = parts[parts.size() - 1];
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
