#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

service is CodeGenerationService isCodeGenerationService(const std::optional<Service>& service) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return service != nullptr && service != std::nullopt && 'generateCode' in service;

}

service is ProjectPlanningServiceInterface isProjectPlanningService(const std::optional<Service>& service) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return service != nullptr && service != std::nullopt && 'createProjectPlan' in service;

}

service is ProjectStatusManagerInterface isProjectStatusManager(const std::optional<Service>& service) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    return service != nullptr && service != std::nullopt && 'createProject' in service;

}

} // namespace elizaos
