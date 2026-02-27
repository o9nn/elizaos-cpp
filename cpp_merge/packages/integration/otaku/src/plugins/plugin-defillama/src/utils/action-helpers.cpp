#include "action-helpers.h"

any sanitizeChainName(any value)
{
    if (!value) {
        return undefined;
    }
    auto trimmed = value->trim();
    if (!trimmed) {
        return undefined;
    }
    return (CHAIN_NAME_PATTERN->test(trimmed)) ? any(trimmed) (undefined);
};


any sanitizeFilterSegment(any value)
{
    if (!value) {
        return undefined;
    }
    auto trimmed = value->trim()->toLowerCase();
    if (!trimmed) {
        return undefined;
    }
    return (FILTER_PATTERN->test(trimmed)) ? any(trimmed) (undefined);
};


std::shared_ptr<RegExp> CHAIN_NAME_PATTERN = (new RegExp(std:("^[A-Za-z0-9 .\-_/()]{2,}")));
std::shared_ptr<RegExp> FILTER_PATTERN = (new RegExp(std:("^[a-z\-]{2,}")));

void Main(void)
{
}

MAIN
