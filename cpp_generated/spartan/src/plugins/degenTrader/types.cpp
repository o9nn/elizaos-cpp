#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/degenTrader/types.h"

object ServiceTypes = as<std::shared_ptr<const>>(object{
    object::pair{std::string("DEGEN_TRADING"), std::string("degen_trader")}
});

void Main(void)
{
}

MAIN
