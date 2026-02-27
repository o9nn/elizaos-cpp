#include "/home/runner/work/elizaos-cpp/elizaos-cpp/classified/packages/core/src/types/components.h"

std::shared_ptr<ActionResult> createActionResult(Partial<std::shared_ptr<ActionResult>> partial)
{
    return utils::assign(object{
        object::pair{std:("success"), true}
    }, partial);
};


