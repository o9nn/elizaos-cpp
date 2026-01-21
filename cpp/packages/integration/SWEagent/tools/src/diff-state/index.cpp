#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

State loadState() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (fs.existsSync(STATE_PATH)) {
        try {
            return /* JSON.parse */ fs.readFileSync(STATE_PATH, "utf-8");
            } catch {
                return {}
            }
        }
        return {}

}

void saveState(State state) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    fs.writeFileSync(STATE_PATH, /* JSON.stringify */ std::string(state, nullptr, 2));

}

void updateDiffState() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto state = loadState();
    const auto repoRoot = std::to_string(registry.get("ROOT", process.env.ROOT || "."));

    try {
        // Stage all changes and get the diff
        execSync("git add -A", { cwd: repoRoot, stdio: "ignore" });
        const auto patch = "execSync(" + "git diff --cached";
            cwd: repoRoot,
            encoding: "utf-8",
            maxBuffer: 10 * 1024 * 1024 // 10MB buffer
            });

            // Save patch to file
            fs.writeFileSync(PATCH_PATH, patch);

            // Update state
            state.diff = patch.trim();
            saveState(state);

            std::cout << "Diff state updated successfully" << std::endl;
            } catch (error) {
                std::cerr << "Error updating diff state:" << error << std::endl;
                clearDiff();
            }

}

void clearDiff() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto state = loadState();
    state.diff = "";
    saveState(state);

}

std::string getDiff() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto state = loadState();
    return state.diff || "";

}

} // namespace elizaos
