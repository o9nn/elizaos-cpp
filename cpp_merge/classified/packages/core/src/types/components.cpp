#include "components.hpp"
#include <string>

std::shared_ptr<ActionResult> createActionResult(Partial<std::shared_ptr<ActionResult>> partial)
{
    return utils::assign(object{
        object::pair{std::string("success"), true}
    }, partial);
};


