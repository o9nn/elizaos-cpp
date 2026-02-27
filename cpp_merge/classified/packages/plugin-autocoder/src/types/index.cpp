#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-autocoder/src/types/index.h"

any isCodeGenerationService(any service)
{
    return AND((AND((service != nullptr), (service != undefined))), (in(std::string("generateCode"), service)));
};


any isProjectPlanningService(any service)
{
    return AND((AND((service != nullptr), (service != undefined))), (in(std::string("createProjectPlan"), service)));
};


any isProjectStatusManager(any service)
{
    return AND((AND((service != nullptr), (service != undefined))), (in(std::string("createProject"), service)));
};


