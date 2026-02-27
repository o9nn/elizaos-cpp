#include "validation.hpp"

string maskedValue(string value)
{
    if (!value) return string_empty;
    if (AND((value->get_length() > 20), (!value->includes(std:(" "))))) {
        return string_empty + value->substring(0, 4) + std:("...") + value->substring(value->get_length() - 4) + string_empty;
    }
    return value;
};


