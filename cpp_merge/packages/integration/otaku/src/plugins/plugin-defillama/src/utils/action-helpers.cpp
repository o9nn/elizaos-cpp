#include "action-helpers.h"
#include <string>

any sanitizeChainName(any value)
{
    if (!value) {
        return std::nullopt;
    }
    auto trimmed = value->trim();
    if (!trimmed) {
        return std::nullopt;
    }
    return (CHAIN_NAME_PATTERN->test(trimmed)) ? any(trimmed) (std::nullopt);
};


any sanitizeFilterSegment(any value)
{
    if (!value) {
        return std::nullopt;
    }
    auto trimmed = value->trim()->toLowerCase();
    if (!trimmed) {
        return std::nullopt;
    }
    return (FILTER_PATTERN->test(trimmed)) ? any(trimmed) (std::nullopt);
};


std::shared_ptr<RegExp> CHAIN_NAME_PATTERN = (new RegExp(std::string("^[A-Za-z0-9 .\-_/()]{2,}")));
std::shared_ptr<RegExp> FILTER_PATTERN = (new RegExp(std::string("^[a-z\-]{2,}")));

void Main(void)
{
}

MAIN
