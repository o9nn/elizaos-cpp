#include "agent-templates.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

AgentTemplate getTemplateById(const std:& id) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return agentTemplates.find[&]((template) { return template.id == id); };

}

} // namespace elizaos
