#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/degenTrader/utils/validator.h"

boolean validateSolanaAddress(any address)
{
    if (!address) return false;
    try
    {
        if (address->startsWith(std::string("0x"))) {
            auto isValidBase = (new RegExp(std::string("^0x[a-fA-F0-9]{40}")))->test(address);
            logger->log(std::string("Base address validation: ") + address + string_empty, object{
                object::pair{std::string("isValid"), isValidBase}
            });
            return isValidBase;
        }
        if (!(new RegExp(std::string("^[1-9A-HJ-NP-Za-km-z]{32,44}")))->test(address)) {
            logger->warn(std::string("Invalid Solana address format: ") + address + string_empty);
            return false;
        }
        auto pubKey = std::make_shared<PublicKey>(address);
        auto isValid = Boolean(pubKey->toBase58());
        logger->log(std::string("Solana address validation: ") + address + string_empty, object{
            object::pair{std::string("isValid"), std::string("isValid")}
        });
        return isValid;
    }
    catch (const any& error)
    {
        logger->error(std::string("Address validation error: ") + address + string_empty, object{
            object::pair{std::string("error"), std::string("error")}
        });
        return false;
    }
};


