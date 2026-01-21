#include "module-loader.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

ModuleLoader getModuleLoader() {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (!defaultLoader) {
        defaultLoader = new ModuleLoader();
    }
    return defaultLoader;

}

} // namespace elizaos
