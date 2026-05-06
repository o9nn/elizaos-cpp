#include "use-slippage.h"
#include <string>

std::function<any()> useSlippage = [=]() mutable
{
    auto [slippage, setSlippage] = useLocalStorage<TSlippage>(std::string("use-slippage-remember"), 2);
    return as<std::shared_ptr<const>>(array<std::shared_ptr<const>>{ slippage, setSlippage });
};

void Main(void)
{
}

MAIN
