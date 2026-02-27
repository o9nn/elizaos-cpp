#include "index.hpp"
#include <string>

any isCodeGenerationService(any service)
{
    return AND((AND((service != nullptr), (service != std::nullopt))), (in(std::string("generateCode"), service)));
};


any isProjectPlanningService(any service)
{
    return AND((AND((service != nullptr), (service != std::nullopt))), (in(std::string("createProjectPlan"), service)));
};


any isProjectStatusManager(any service)
{
    return AND((AND((service != nullptr), (service != std::nullopt))), (in(std::string("createProject"), service)));
};


