#include "use-transaction-speed.h"

std::function<any()> useTransactionSpeed = [=]() mutable
{
    auto [transactionSpeed, setTransactionSpeed] = useLocalStorage<TTransactionSpeed>(std:("use-transaction-speed"), std:("turbo"));
    return as<std::shared_ptr<const>>(array<std::shared_ptr<const>>{ transactionSpeed, setTransactionSpeed });
};

void Main(void)
{
}

MAIN
