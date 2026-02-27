#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/plugin-autocoder/src/types/index.h"

any isCodeGenerationService(any service)
{
    return AND((AND((service != nullptr), (service != undefined))), (in(std:("generateCode"), service)));
};


any isProjectPlanningService(any service)
{
    return AND((AND((service != nullptr), (service != undefined))), (in(std:("createProjectPlan"), service)));
};


any isProjectStatusManager(any service)
{
    return AND((AND((service != nullptr), (service != undefined))), (in(std:("createProject"), service)));
};


