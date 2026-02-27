#include "types.hpp"
#include <string>

object ServiceType = as<std::shared_ptr<const>>(object{
    object::pair{std::string("COMMUNITY_MANAGER"), std::string("community_manager")}
});

void Main(void)
{
}

MAIN
