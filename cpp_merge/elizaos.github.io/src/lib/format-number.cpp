#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/format-number.h"

string formatCompactNumber(double number, double maximumFractionDigits)
{
    auto formatter = Intl::NumberFormat(std::string("en"), object{
        object::pair{std::string("notation"), std::string("compact")}, 
        object::pair{std::string("maximumFractionDigits"), std::string("maximumFractionDigits")}
    });
    return formatter->format(number);
};


