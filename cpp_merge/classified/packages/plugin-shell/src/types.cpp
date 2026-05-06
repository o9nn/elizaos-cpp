#include "types.hpp"
#include <string>

namespace @elizaos/core {
}
object ShellServiceType = object{
    object::pair{std::string("SHELL"), as<std::shared_ptr<const>>(std::string("SHELL"))}
};

void Main(void)
{
}

MAIN
