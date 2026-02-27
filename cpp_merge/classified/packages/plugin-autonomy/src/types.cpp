#include "types.hpp"
#include <string>

namespace @elizaos/core {
}
object AutonomousServiceType = object{
    object::pair{std::string("AUTONOMOUS"), as<std::shared_ptr<const>>(std::string("AUTONOMOUS"))}
}, satisfies, Partial;

void Main(void)
{
    static_cast<any>();
}

MAIN
