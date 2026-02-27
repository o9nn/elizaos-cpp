#include "use-transaction-speed.h"
#include <string>

std::function<any()> useTransactionSpeed = [=]() mutable
{
    auto [transactionSpeed, setTransactionSpeed] = useLocalStorage<TTransactionSpeed>(std::string("use-transaction-speed"), std::string("turbo"));
    return as<std::shared_ptr<const>>(array<std::shared_ptr<const>>{ transactionSpeed, setTransactionSpeed });
};

void Main(void)
{
}

MAIN
