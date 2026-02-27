#include "/home/runner/work/elizaos-cpp/elizaos-cpp/elizaos.github.io/src/lib/format-number.h"

string formatCompactNumber(double number, double maximumFractionDigits)
{
    auto formatter = Intl::NumberFormat(std:("en"), object{
        object::pair{std:("notation"), std:("compact")}, 
        object::pair{std:("maximumFractionDigits"), std:("maximumFractionDigits")}
    });
    return formatter->format(number);
};


