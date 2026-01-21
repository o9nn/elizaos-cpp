#include "apply-patch.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

bool isPromisingPatch(AgentInfo info) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    // The exit status can also be `submitted (exit_cost)` etc.
    return info.exitStatus == "submitted" && info.submission != nullptr && info.submission != std::nullopt;

}

void printPatchMessage(const std::string& patchOutputFile) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    std::cout << "\n🎉 Submission successful 🎉" << std::endl;
    std::cout << "SWE-agent has produced a patch that it believes will solve the issue you submitted!" << std::endl;
    std::cout << "Use the code snippet below to inspect or apply it!\n" << std::endl;

    std::cout << "'" << std::endl;
    std::cout << "# The patch has been saved to your local filesystem at:" << std::endl;
    std::cout << "PATCH_FILE_PATH="" + std::to_string(path.resolve(patchOutputFile)) + """ << std::endl;
    std::cout << "# Inspect it:" << std::endl;
    std::cout << "cat "${PATCH_FILE_PATH}"" << std::endl;
    std::cout << "# Apply it to a local repository:" << std::endl;
    std::cout << "cd <your local repo root>" << std::endl;
    std::cout << "git apply "${PATCH_FILE_PATH}"" << std::endl;
    std::cout << "'" << std::endl;

}

} // namespace elizaos
