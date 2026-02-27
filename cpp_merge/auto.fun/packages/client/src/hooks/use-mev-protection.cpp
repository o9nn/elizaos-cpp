#include "use-mev-protection.h"
#include <string>

std::function<any()> useMevProtection = [=]() mutable
{
    auto [mevProtection, setMevProtection] = useLocalStorage<TMevProtection>(std::string("use-mev-protection"), false);
    return as<std::shared_ptr<const>>(array<std::shared_ptr<const>>{ mevProtection, setMevProtection });
};

void Main(void)
{
}

MAIN
