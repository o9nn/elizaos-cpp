#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/client/src/hooks/use-mev-protection.h"

std::function<any()> useMevProtection = [=]() mutable
{
    auto [mevProtection, setMevProtection] = useLocalStorage<TMevProtection>(std::string("use-mev-protection"), false);
    return as<std::shared_ptr<const>>(array<std::shared_ptr<const>>{ mevProtection, setMevProtection });
};

void Main(void)
{
}

MAIN
