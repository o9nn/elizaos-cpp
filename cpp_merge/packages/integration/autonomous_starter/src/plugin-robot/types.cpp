#include "types.hpp"
#include <string>

namespace @elizaos/core {
}
object RobotServiceType = object{
    object::pair{std::string("ROBOT"), as<std::shared_ptr<const>>(std::string("ROBOT"))}
}, satisfies, Partial;

void Main(void)
{
    static_cast<any>();
}

MAIN
