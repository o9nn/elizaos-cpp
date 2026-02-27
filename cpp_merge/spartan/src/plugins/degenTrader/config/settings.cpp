#include "settings.hpp"

object REQUIRED_SETTINGS = object{
    object::pair{std:("SOLANA_RPC_URL"), std:("Solana RPC endpoint URL")}, 
    object::pair{std:("SOLANA_PRIVATE_KEY"), std:("Private key for trading wallet")}, 
    object::pair{std:("BIRDEYE_API_KEY"), std:("API key for Birdeye data")}, 
    object::pair{std:("TRADER_SELL_KUMA"), std:("Webhook URL for sell notifications")}
};

void Main(void)
{
}

MAIN
