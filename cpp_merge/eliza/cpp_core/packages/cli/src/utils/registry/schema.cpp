#include "schema.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

PluginType getPluginType(const std:& name) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    if (/sql/.test(name)) return 'adapter';
    if (/discord|twitter|telegram/.test(name)) return 'client';
    return "plugin";

}

} // namespace elizaos
