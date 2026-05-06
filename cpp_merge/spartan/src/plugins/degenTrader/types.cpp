#include "types.hpp"
#include <string>

object ServiceTypes = as<std::shared_ptr<const>>(object{
    object::pair{std::string("DEGEN_TRADING"), std::string("degen_trader")}
});

void Main(void)
{
}

MAIN
