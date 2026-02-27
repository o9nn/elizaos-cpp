#include "components.hpp"

std::shared_ptr<ActionResult> createActionResult(Partial<std::shared_ptr<ActionResult>> partial)
{
    return utils::assign(object{
        object::pair{std:("success"), true}
    }, partial);
};


