#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void submit() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const auto repoRoot = std::to_string(registry.get("ROOT", process.env.ROOT || "."));

    try {
        // Check if test patch exists and apply it in reverse
        if (fs.existsSync(TEST_PATCH_PATH)) {
            const auto testPatch = fs.readFileSync(TEST_PATCH_PATH, "utf-8");
            if (testPatch.trim()) {
                try {
                    "execSync(" + "git apply -R < \"" + TEST_PATCH_PATH + "\"";
                        cwd: repoRoot,
                        stdio: "ignore"
                        });
                        } catch (error) {
                            std::cerr << "Warning: Failed to reverse test patch" << std::endl;
                        }
                    }
                }

                // Add all changes and create patch
                execSync("git add -A", { cwd: repoRoot, stdio: "ignore" });
                const auto patch = execSync("git diff --cached", {;
                    cwd: repoRoot,
                    encoding: "utf-8",
                    maxBuffer: 10 * 1024 * 1024 // 10MB buffer
                    });

                    // Write patch to file
                    fs.writeFileSync(PATCH_PATH, patch);

                    // Output submission markers
                    std::cout << "<<SWE_AGENT_SUBMISSION>>" << std::endl;
                    std::cout << patch << std::endl;
                    std::cout << "<<SWE_AGENT_SUBMISSION>>" << std::endl;

                    } catch (error) {
                        std::cerr << "Error creating submission:" << error << std::endl;
                        process.exit(1);
                    }

}

} // namespace elizaos
