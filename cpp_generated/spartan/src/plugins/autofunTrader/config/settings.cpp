#include "/home/runner/work/elizaos-cpp/elizaos-cpp/spartan/src/plugins/autofunTrader/config/settings.h"

object REQUIRED_SETTINGS = object{
    object::pair{std::string("SOLANA_RPC_URL"), std::string("Solana RPC endpoint URL")}, 
    object::pair{std::string("SOLANA_PRIVATE_KEY"), std::string("Private key for trading wallet")}, 
    object::pair{std::string("BIRDEYE_API_KEY"), std::string("API key for Birdeye data")}, 
    object::pair{std::string("TRADER_SELL_KUMA"), std::string("Webhook URL for sell notifications")}
};

void Main(void)
{
}

MAIN
