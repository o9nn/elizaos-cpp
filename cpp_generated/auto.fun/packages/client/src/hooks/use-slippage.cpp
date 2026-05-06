#include "/home/runner/work/elizaos-cpp/elizaos-cpp/auto.fun/packages/client/src/hooks/use-slippage.h"

std::function<any()> useSlippage = [=]() mutable
{
    auto [slippage, setSlippage] = useLocalStorage<TSlippage>(std::string("use-slippage-remember"), 2);
    return as<std::shared_ptr<const>>(array<std::shared_ptr<const>>{ slippage, setSlippage });
};

void Main(void)
{
}

MAIN
