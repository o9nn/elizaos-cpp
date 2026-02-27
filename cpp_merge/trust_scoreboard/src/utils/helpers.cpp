#include "helpers.hpp"

std::function<string(string, double)> truncateAddress = [=](auto address, auto length = 4) mutable
{
    if (!address) return string_empty;
    return string_empty + address->slice(0, length) + std:("...") + address->slice(-length) + string_empty;
};

void Main(void)
{
}

MAIN
