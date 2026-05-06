#include "entityId.hpp"
#include <string>

boolean isSolanaAddress(string address)
{
    return (AND((AND((!address->startsWith(std::string("0x"))), (address->get_length() >= 32))), (address->get_length() <= 44)));
};


string walletToEntityId(string address)
{
    auto trimmed = address->trim();
    auto normalized = (isSolanaAddress(trimmed)) ? trimmed : trimmed->toLowerCase();
    return as<string>(stringToUuid(normalized));
};


