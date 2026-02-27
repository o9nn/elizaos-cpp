#include "/home/runner/work/elizaos-cpp/elizaos-cpp/autonomous-starter/src/plugin-shell/types.h"

namespace @elizaos/core {
}
object ShellServiceType = object{
    object::pair{std::string("SHELL"), as<std::shared_ptr<const>>(std::string("SHELL"))}
}, satisfies, Partial;

void Main(void)
{
    static_cast<any>();
}

MAIN
