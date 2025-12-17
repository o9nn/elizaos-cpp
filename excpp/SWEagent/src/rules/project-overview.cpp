#include "project-overview.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

void getComponentByPath(const std::string& path) {
    // NOTE: Auto-converted from TypeScript - may need refinement
    component: string; description: string
}

std::vector<std::string> getPythonModules() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::vector<std::string> modules = [];

    // Add entry points
    modules.push(...ENTRY_POINTS.map((ep) => ep.path));

    // Add main classes
    modules.push(MAIN_AGENT_CLASS.path);
    modules.push(SWE_ENV_CLASS.path);

    // Add inspectors
    modules.push(...INSPECTORS.map((i) => i.path));

    return modules;

}

} // namespace elizaos
