#include "types.hpp"
#include <string>

namespace @elizaos/core {
}
object GoalServiceType = object{
    object::pair{std::string("GOALS"), as<std::shared_ptr<const>>(std::string("goals"))}
}, satisfies, Partial;

void Main(void)
{
    static_cast<any>();
}

MAIN
